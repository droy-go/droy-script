package cmd

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/spf13/cobra"
)

var (
	buildOutput   string
	buildTarget   string
	buildOptimize bool
	buildVerbose  bool
)

var buildCmd = &cobra.Command{
	Use:   "build [file.droy]",
	Short: "Build a Droy program",
	Long: `Compile a Droy program to an executable or LLVM IR.
If no file is specified, builds the main file from droy.toml.`,
	Example: `  droy-pm build                    # Build default main file
  droy-pm build app.droy           # Build specific file
  droy-pm build -o output          # Specify output name
  droy-pm build --target=llvm      # Build to LLVM IR`,
	Run: func(cmd *cobra.Command, args []string) {
		var targetFile string
		
		if len(args) > 0 {
			targetFile = args[0]
		} else {
			// Try to get from droy.toml
			pkg, err := config.ReadPackageConfig("droy.toml")
			if err == nil && pkg.Main != "" {
				targetFile = pkg.Main
			} else {
				// Default to src/main.droy
				targetFile = "src/main.droy"
			}
		}

		// Check if file exists
		if _, err := os.Stat(targetFile); os.IsNotExist(err) {
			logger.Error("File not found: %s", targetFile)
			return
		}

		// Determine output name
		if buildOutput == "" {
			base := filepath.Base(targetFile)
			buildOutput = strings.TrimSuffix(base, filepath.Ext(base))
		}

		logger.Info("Building %s...", targetFile)

		// Find the Droy compiler
		droyPath := findDroyCompiler()
		if droyPath == "" {
			logger.Error("Droy compiler not found")
			logger.Info("Make sure Droy is installed and in your PATH")
			return
		}

		// Build command arguments
		var execArgs []string
		
		if buildTarget == "llvm" {
			// Build to LLVM IR
			execArgs = append(execArgs, "-c", "-o", buildOutput+".ll", targetFile)
		} else {
			// Build to executable
			execArgs = append(execArgs, targetFile)
			if buildOutput != "" {
				execArgs = append(execArgs, "-o", buildOutput)
			}
		}

		if buildVerbose {
			logger.Info("Command: %s %s", droyPath, strings.Join(execArgs, " "))
		}

		// Execute build
		execCmd := exec.Command(droyPath, execArgs...)
		execCmd.Stdin = os.Stdin
		execCmd.Stdout = os.Stdout
		execCmd.Stderr = os.Stderr

		if err := execCmd.Run(); err != nil {
			logger.Error("Build failed: %v", err)
			return
		}

		// Check if output was created
		if buildTarget == "llvm" {
			buildOutput += ".ll"
		}
		
		if _, err := os.Stat(buildOutput); os.IsNotExist(err) {
			logger.Warning("Build completed but output file not found")
			return
		}

		// Get file info
		if info, err := os.Stat(buildOutput); err == nil {
			size := formatFileSize(info.Size())
			logger.Success("Built %s (%s)", buildOutput, size)
		} else {
			logger.Success("Built %s", buildOutput)
		}
	},
}

var cleanBuildCmd = &cobra.Command{
	Use:   "clean-build",
	Short: "Clean build artifacts",
	Long:  `Remove all build artifacts and compiled files.`,
	Run: func(cmd *cobra.Command, args []string) {
		logger.Info("Cleaning build artifacts...")

		patterns := []string{
			"*.ll",
			"*.s",
			"*.o",
			"*.out",
			"*.exe",
			"dist/",
			"build/",
		}

		removed := 0
		for _, pattern := range patterns {
			matches, err := filepath.Glob(pattern)
			if err != nil {
				continue
			}
			for _, match := range matches {
				if err := os.RemoveAll(match); err == nil {
					removed++
					if buildVerbose {
						logger.Info("Removed: %s", match)
					}
				}
			}
		}

		logger.Success("Cleaned %d build artifacts", removed)
	},
}

func findDroyCompiler() string {
	// Check PATH
	if path, err := exec.LookPath("droy"); err == nil {
		return path
	}

	// Check common locations
	locations := []string{
		"./bin/droy",
		"./droy",
		"../bin/droy",
		"/usr/local/bin/droy",
		"/usr/bin/droy",
		"./droy-lang/bin/droy",
	}

	for _, loc := range locations {
		if _, err := os.Stat(loc); err == nil {
			absPath, _ := filepath.Abs(loc)
			return absPath
		}
	}

	// Check DROY_HOME environment variable
	if droyHome := os.Getenv("DROY_HOME"); droyHome != "" {
		path := filepath.Join(droyHome, "bin", "droy")
		if _, err := os.Stat(path); err == nil {
			return path
		}
	}

	return ""
}

func formatFileSize(size int64) string {
	const (
		KB = 1024
		MB = 1024 * KB
		GB = 1024 * MB
	)

	switch {
	case size >= GB:
		return fmt.Sprintf("%.2f GB", float64(size)/GB)
	case size >= MB:
		return fmt.Sprintf("%.2f MB", float64(size)/MB)
	case size >= KB:
		return fmt.Sprintf("%.2f KB", float64(size)/KB)
	default:
		return fmt.Sprintf("%d B", size)
	}
}

func init() {
	buildCmd.Flags().StringVarP(&buildOutput, "output", "o", "", "Output file name")
	buildCmd.Flags().StringVarP(&buildTarget, "target", "t", "", "Build target (native, llvm)")
	buildCmd.Flags().BoolVarP(&buildOptimize, "optimize", "O", false, "Enable optimizations")
	buildCmd.Flags().BoolVarP(&buildVerbose, "verbose", "v", false, "Verbose output")

	rootCmd.AddCommand(buildCmd)
	rootCmd.AddCommand(cleanBuildCmd)
}
