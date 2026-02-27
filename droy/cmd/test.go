package cmd

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var (
	testVerbose bool
	testPattern string
	testCoverage bool
	testWatch   bool
)

var testCmd = &cobra.Command{
	Use:     "test [pattern]",
	Aliases: []string{"t"},
	Short:   "Run tests",
	Long: `Run Droy tests in the current project.
If a pattern is specified, only tests matching the pattern will run.`,
	Example: `  droy-pm test              # Run all tests
  droy-pm test math         # Run tests matching "math"
  droy-pm test -v           # Verbose output
  droy-pm test --coverage   # With coverage report`,
	Run: func(cmd *cobra.Command, args []string) {
		pattern := ""
		if len(args) > 0 {
			pattern = args[0]
		}

		logger.Info("Running tests...")

		// Find test files
		testFiles, err := findTestFiles(pattern)
		if err != nil {
			logger.Error("Failed to find test files: %v", err)
			return
		}

		if len(testFiles) == 0 {
			logger.Warning("No test files found")
			if pattern != "" {
				logger.Info("No tests match pattern: %s", pattern)
			}
			return
		}

		logger.Info("Found %d test file(s)", len(testFiles))

		// Run tests
		passed := 0
		failed := 0

		for _, file := range testFiles {
			if testVerbose {
				logger.Info("Running: %s", file)
			}

			result := runTestFile(file)
			if result {
				passed++
			} else {
				failed++
			}
		}

		// Print summary
		fmt.Println()
		color.Cyan("Test Results:")
		color.Green("  Passed: %d", passed)
		if failed > 0 {
			color.Red("  Failed: %d", failed)
		}
		fmt.Printf("  Total:  %d\n", passed+failed)

		if failed > 0 {
			os.Exit(1)
		}
	},
}

var benchCmd = &cobra.Command{
	Use:   "bench [pattern]",
	Short: "Run benchmarks",
	Long:  `Run Droy benchmarks in the current project.`,
	Run: func(cmd *cobra.Command, args []string) {
		pattern := ""
		if len(args) > 0 {
			pattern = args[0]
		}

		logger.Info("Running benchmarks...")

		// Find benchmark files
		benchFiles, err := findBenchmarkFiles(pattern)
		if err != nil {
			logger.Error("Failed to find benchmark files: %v", err)
			return
		}

		if len(benchFiles) == 0 {
			logger.Warning("No benchmark files found")
			return
		}

		logger.Info("Found %d benchmark file(s)", len(benchFiles))

		for _, file := range benchFiles {
			logger.Info("Benchmarking: %s", file)
			runBenchmarkFile(file)
		}
	},
}

func findTestFiles(pattern string) ([]string, error) {
	var files []string

	// Common test directories
	dirs := []string{"tests", "test", "spec", "."}

	for _, dir := range dirs {
		if _, err := os.Stat(dir); os.IsNotExist(err) {
			continue
		}

		err := filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}

			if info.IsDir() {
				return nil
			}

			// Check if it's a test file
			if strings.HasSuffix(path, "_test.droy") || strings.HasSuffix(path, ".test.droy") {
				if pattern == "" || strings.Contains(path, pattern) {
					files = append(files, path)
				}
			}

			return nil
		})

		if err != nil {
			return nil, err
		}
	}

	return files, nil
}

func findBenchmarkFiles(pattern string) ([]string, error) {
	var files []string

	dirs := []string{"bench", "benchmarks", "tests", "."}

	for _, dir := range dirs {
		if _, err := os.Stat(dir); os.IsNotExist(err) {
			continue
		}

		err := filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}

			if info.IsDir() {
				return nil
			}

			if strings.HasSuffix(path, "_bench.droy") || strings.HasSuffix(path, ".bench.droy") {
				if pattern == "" || strings.Contains(path, pattern) {
					files = append(files, path)
				}
			}

			return nil
		})

		if err != nil {
			return nil, err
		}
	}

	return files, nil
}

func runTestFile(file string) bool {
	// Find the Droy interpreter
	droyPath := findDroyInterpreter()
	if droyPath == "" {
		logger.Error("Droy interpreter not found")
		return false
	}

	// Run the test file
	execCmd := exec.Command(droyPath, file)
	execCmd.Stdin = os.Stdin

	if testVerbose {
		execCmd.Stdout = os.Stdout
		execCmd.Stderr = os.Stderr
	}

	output, err := execCmd.CombinedOutput()
	
	if err != nil {
		if testVerbose {
			logger.Error("Test failed: %s", file)
			fmt.Println(string(output))
		} else {
			logger.Error("✗ %s", file)
		}
		return false
	}

	if testVerbose {
		logger.Success("✓ %s", file)
		fmt.Println(string(output))
	} else {
		logger.Success("✓ %s", file)
	}

	return true
}

func runBenchmarkFile(file string) {
	droyPath := findDroyInterpreter()
	if droyPath == "" {
		logger.Error("Droy interpreter not found")
		return
	}

	execCmd := exec.Command(droyPath, file)
	execCmd.Stdout = os.Stdout
	execCmd.Stderr = os.Stderr

	if err := execCmd.Run(); err != nil {
		logger.Error("Benchmark failed: %v", err)
	}
}

func init() {
	testCmd.Flags().BoolVarP(&testVerbose, "verbose", "v", false, "Verbose output")
	testCmd.Flags().StringVarP(&testPattern, "pattern", "p", "", "Test pattern")
	testCmd.Flags().BoolVar(&testCoverage, "coverage", false, "Generate coverage report")
	testCmd.Flags().BoolVarP(&testWatch, "watch", "w", false, "Watch for changes")

	rootCmd.AddCommand(testCmd)
	rootCmd.AddCommand(benchCmd)
}
