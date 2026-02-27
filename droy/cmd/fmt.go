package cmd

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/spf13/cobra"
)

var (
	fmtWrite bool
	fmtList  bool
	fmtDiff  bool
)

var fmtCmd = &cobra.Command{
	Use:     "fmt [files...]",
	Aliases: []string{"format"},
	Short:   "Format Droy source files",
	Long: `Format Droy source files according to the standard style.
If no files are specified, formats all .droy files in the current directory.`,
	Example: `  droy-pm fmt                    # Format all files
  droy-pm fmt src/main.droy      # Format specific file
  droy-pm fmt -w                 # Write changes (default)
  droy-pm fmt -l                 # List files that need formatting
  droy-pm fmt -d                 # Show diff`,
	Run: func(cmd *cobra.Command, args []string) {
		var files []string

		if len(args) == 0 {
			// Find all .droy files
			var err error
			files, err = findDroyFiles(".")
			if err != nil {
				logger.Error("Failed to find files: %v", err)
				return
			}
		} else {
			files = args
		}

		if len(files) == 0 {
			logger.Info("No Droy files found")
			return
		}

		logger.Info("Formatting %d file(s)...", len(files))

		formatted := 0
		unchanged := 0
		errors := 0

		for _, file := range files {
			result, err := formatFile(file)
			if err != nil {
				logger.Error("Failed to format %s: %v", file, err)
				errors++
				continue
			}

			if result {
				formatted++
				if fmtList {
					fmt.Println(file)
				}
			} else {
				unchanged++
			}
		}

		if fmtList {
			return
		}

		logger.Success("Formatted %d, unchanged %d", formatted, unchanged)
		if errors > 0 {
			logger.Warning("Errors: %d", errors)
		}
	},
}

var lintCmd = &cobra.Command{
	Use:   "lint [files...]",
	Short: "Lint Droy source files",
	Long:  `Check Droy source files for common issues and style violations.`,
	Run: func(cmd *cobra.Command, args []string) {
		var files []string

		if len(args) == 0 {
			var err error
			files, err = findDroyFiles(".")
			if err != nil {
				logger.Error("Failed to find files: %v", err)
				return
			}
		} else {
			files = args
		}

		if len(files) == 0 {
			logger.Info("No Droy files found")
			return
		}

		logger.Info("Linting %d file(s)...", len(files))

		issues := 0
		for _, file := range files {
			fileIssues := lintFile(file)
			issues += fileIssues
		}

		if issues == 0 {
			logger.Success("No issues found!")
		} else {
			logger.Warning("Found %d issue(s)", issues)
		}
	},
}

func findDroyFiles(root string) ([]string, error) {
	var files []string

	excludedDirs := map[string]bool{
		"droy_modules": true,
		"node_modules": true,
		".git":         true,
		"dist":         true,
		"build":        true,
	}

	err := filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if info.IsDir() {
			if excludedDirs[info.Name()] {
				return filepath.SkipDir
			}
			return nil
		}

		if strings.HasSuffix(path, ".droy") {
			files = append(files, path)
		}

		return nil
	})

	return files, err
}

func formatFile(file string) (bool, error) {
	// Read file
	content, err := os.ReadFile(file)
	if err != nil {
		return false, err
	}

	original := string(content)
	formatted := formatDroyCode(original)

	if original == formatted {
		return false, nil
	}

	if fmtDiff {
		showDiff(file, original, formatted)
	}

	if fmtWrite {
		if err := os.WriteFile(file, []byte(formatted), 0644); err != nil {
			return false, err
		}
		logger.Info("Formatted: %s", file)
	}

	return true, nil
}

func formatDroyCode(code string) string {
	// Basic formatting rules for Droy
	lines := strings.Split(code, "\n")
	var result []string
	indent := 0

	for _, line := range lines {
		trimmed := strings.TrimSpace(line)

		// Decrease indent for closing braces
		if strings.HasPrefix(trimmed, "}") || strings.HasPrefix(trimmed, ")") {
			if indent > 0 {
				indent--
			}
		}

		// Add proper indentation
		if trimmed != "" {
			result = append(result, strings.Repeat("    ", indent)+trimmed)
		} else {
			result = append(result, "")
		}

		// Increase indent for opening braces
		if strings.HasSuffix(trimmed, "{") || strings.HasSuffix(trimmed, "(") {
			indent++
		}
	}

	return strings.Join(result, "\n")
}

func showDiff(file, original, formatted string) {
	fmt.Printf("--- %s (original)\n", file)
	fmt.Printf("+++ %s (formatted)\n", file)
	fmt.Println()
}

func lintFile(file string) int {
	content, err := os.ReadFile(file)
	if err != nil {
		logger.Error("Cannot read %s: %v", file, err)
		return 1
	}

	code := string(content)
	issues := 0

	// Check for common issues
	if strings.Contains(code, "  ") && !strings.Contains(code, "    ") {
		logger.Warning("%s: Use 4 spaces for indentation", file)
		issues++
	}

	if strings.Count(code, "\t") > 0 {
		logger.Warning("%s: Use spaces instead of tabs", file)
		issues++
	}

	// Check for trailing whitespace
	lines := strings.Split(code, "\n")
	for i, line := range lines {
		if strings.HasSuffix(line, " ") || strings.HasSuffix(line, "\t") {
			logger.Warning("%s:%d: Trailing whitespace", file, i+1)
			issues++
		}
	}

	// Check for missing newlines at end of file
	if len(code) > 0 && !strings.HasSuffix(code, "\n") {
		logger.Warning("%s: Missing newline at end of file", file)
		issues++
	}

	return issues
}

func init() {
	fmtCmd.Flags().BoolVarP(&fmtWrite, "write", "w", true, "Write formatted output to files")
	fmtCmd.Flags().BoolVarP(&fmtList, "list", "l", false, "List files that need formatting")
	fmtCmd.Flags().BoolVarP(&fmtDiff, "diff", "d", false, "Show diff")

	rootCmd.AddCommand(fmtCmd)
	rootCmd.AddCommand(lintCmd)
}
