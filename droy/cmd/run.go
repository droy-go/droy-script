package cmd

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var (
	runFile   string
	runDebug  bool
	runArgs   []string
)

var runCmd = &cobra.Command{
	Use:   "run [file.droy]",
	Short: "Run a Droy program",
	Long: `Execute a Droy program file.
If no file is specified, looks for the main file defined in droy.toml
or defaults to src/main.droy.`,
	Example: `  droy-pm run                    # Run default main file
  droy-pm run app.droy           # Run specific file
  droy-pm run app.droy -- arg1   # Run with arguments`,
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
			logger.Info("Make sure the file exists or specify a different file")
			return
		}

		logger.Info("Running %s...", targetFile)
		
		// Find the Droy interpreter
		droyPath := findDroyInterpreter()
		if droyPath == "" {
			logger.Error("Droy interpreter not found")
			logger.Info("Make sure Droy is installed and in your PATH")
			logger.Info("Install from: https://github.com/droy-go/droy-lang")
			return
		}

		// Build command
		execCmd := exec.Command(droyPath, targetFile)
		
		// Pass additional arguments
		if len(args) > 1 {
			execCmd.Args = append(execCmd.Args, args[1:]...)
		}

		// Set environment
		execCmd.Env = os.Environ()
		if runDebug {
			execCmd.Env = append(execCmd.Env, "DROY_DEBUG=1")
		}

		// Set working directory
		execCmd.Dir = "."

		// Connect stdio
		execCmd.Stdin = os.Stdin
		execCmd.Stdout = os.Stdout
		execCmd.Stderr = os.Stderr

		// Run the program
		if err := execCmd.Run(); err != nil {
			if exitErr, ok := err.(*exec.ExitError); ok {
				logger.Error("Program exited with code %d", exitErr.ExitCode())
			} else {
				logger.Error("Failed to run program: %v", err)
			}
			return
		}

		logger.Success("Program completed successfully")
	},
}

var scriptCmd = &cobra.Command{
	Use:   "script <name>",
	Short: "Run a script from droy.toml",
	Long: `Execute a script defined in the [scripts] section of droy.toml.`,
	Example: `  droy-pm script build    # Run the build script
  droy-pm script test     # Run the test script`,
	Args: cobra.ExactArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		scriptName := args[0]

		// Read droy.toml
		pkg, err := config.ReadPackageConfig("droy.toml")
		if err != nil {
			logger.Error("Failed to read droy.toml: %v", err)
			return
		}

		// Find script
		script, exists := pkg.Scripts[scriptName]
		if !exists {
			logger.Error("Script '%s' not found in droy.toml", scriptName)
			logger.Info("Available scripts:")
			for name := range pkg.Scripts {
				logger.Info("  - %s", name)
			}
			return
		}

		logger.Info("Running script '%s': %s", scriptName, script)

		// Execute script
		execCmd := exec.Command("sh", "-c", script)
		execCmd.Stdin = os.Stdin
		execCmd.Stdout = os.Stdout
		execCmd.Stderr = os.Stderr

		if err := execCmd.Run(); err != nil {
			logger.Error("Script failed: %v", err)
			return
		}

		logger.Success("Script completed")
	},
}

func findDroyInterpreter() string {
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

func init() {
	runCmd.Flags().BoolVarP(&runDebug, "debug", "d", false, "Enable debug mode")
	
	rootCmd.AddCommand(runCmd)
	rootCmd.AddCommand(scriptCmd)
}
