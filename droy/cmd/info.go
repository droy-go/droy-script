package cmd

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/pkg/registry"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var infoCmd = &cobra.Command{
	Use:   "info [package]",
	Short: "Show package information",
	Long: `Display detailed information about a package.
If no package is specified, shows info about the current project.`,
	Example: `  droy-pm info              # Show current project info
  droy-pm info http         # Show package info from registry`,
	Run: func(cmd *cobra.Command, args []string) {
		if len(args) == 0 {
			showProjectInfo()
		} else {
			showPackageInfo(args[0])
		}
	},
}

func showProjectInfo() {
	// Read droy.toml
	pkg, err := config.ReadPackageConfig("droy.toml")
	if err != nil {
		logger.Error("Failed to read droy.toml: %v", err)
		logger.Info("Make sure you're in a Droy project directory")
		return
	}

	// Print project info
	fmt.Println()
	color.Cyan("📦 %s", pkg.Name)
	fmt.Println()

	color.White("Version:     %s", pkg.Version)
	if pkg.Description != "" {
		color.White("Description: %s", pkg.Description)
	}
	if pkg.Author != "" {
		color.White("Author:      %s", pkg.Author)
	}
	if pkg.License != "" {
		color.White("License:     %s", pkg.License)
	}
	if pkg.Repository != "" {
		color.White("Repository:  %s", pkg.Repository)
	}
	if pkg.Homepage != "" {
		color.White("Homepage:    %s", pkg.Homepage)
	}
	if pkg.DroyVersion != "" {
		color.White("Droy:        %s", pkg.DroyVersion)
	}
	if pkg.Main != "" {
		color.White("Main:        %s", pkg.Main)
	}

	// Dependencies
	if len(pkg.Dependencies) > 0 {
		fmt.Println()
		color.Green("Dependencies (%d):", len(pkg.Dependencies))
		for name, version := range pkg.Dependencies {
			fmt.Printf("  %s %s\n", color.CyanString(name), color.WhiteString(version))
		}
	}

	if len(pkg.DevDependencies) > 0 {
		fmt.Println()
		color.Yellow("Dev Dependencies (%d):", len(pkg.DevDependencies))
		for name, version := range pkg.DevDependencies {
			fmt.Printf("  %s %s\n", color.CyanString(name), color.WhiteString(version))
		}
	}

	// Scripts
	if len(pkg.Scripts) > 0 {
		fmt.Println()
		color.Blue("Scripts:")
		for name, script := range pkg.Scripts {
			fmt.Printf("  %s: %s\n", color.CyanString(name), color.WhiteString(script))
		}
	}

	// Check installed packages
	if _, err := os.Stat("droy_modules"); !os.IsNotExist(err) {
		entries, _ := os.ReadDir("droy_modules")
		if len(entries) > 0 {
			fmt.Println()
			color.Magenta("Installed Packages (%d):", len(entries))
			for _, entry := range entries {
				if entry.IsDir() {
					// Try to get version
					pkgPath := filepath.Join("droy_modules", entry.Name(), "droy.toml")
					if subPkg, err := config.ReadPackageConfig(pkgPath); err == nil {
						fmt.Printf("  %s@%s\n", color.CyanString(subPkg.Name), color.WhiteString(subPkg.Version))
					} else {
						fmt.Printf("  %s\n", color.CyanString(entry.Name()))
					}
				}
			}
		}
	}

	// Project stats
	fmt.Println()
	color.Blue("Project Stats:")

	// Count source files
	srcFiles := 0
	linesOfCode := 0
	if _, err := os.Stat("src"); !os.IsNotExist(err) {
		filepath.Walk("src", func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}
			if !info.IsDir() && filepath.Ext(path) == ".droy" {
				srcFiles++
				if content, err := os.ReadFile(path); err == nil {
					linesOfCode += len(content)
				}
			}
			return nil
		})
	}

	fmt.Printf("  Source Files: %d\n", srcFiles)
	fmt.Printf("  Code Size:    %d bytes\n", linesOfCode)

	fmt.Println()
}

func showPackageInfo(name string) {
	logger.Info("Fetching info for %s...", name)

	// Check for aliases
	if aliased, exists := packageAliases[name]; exists {
		name = aliased
	}

	// Get from registry
	reg := registry.New("")
	info, err := reg.GetPackage(name)
	if err != nil {
		logger.Error("Failed to get package info: %v", err)
		return
	}

	fmt.Println()
	color.Cyan("📦 %s", info.Name)
	fmt.Println()

	color.White("Version:     %s", info.Version)
	if info.Description != "" {
		color.White("Description: %s", info.Description)
	}
	if info.Author != "" {
		color.White("Author:      %s", info.Author)
	}
	if info.License != "" {
		color.White("License:     %s", info.License)
	}
	if info.Repository != "" {
		color.White("Repository:  %s", info.Repository)
	}

	if len(info.Keywords) > 0 {
		fmt.Println()
		color.Green("Keywords: %v", info.Keywords)
	}

	if len(info.Versions) > 0 {
		fmt.Println()
		color.Yellow("Available Versions:")
		for i, v := range info.Versions {
			if i >= 10 {
				fmt.Printf("  ... and %d more\n", len(info.Versions)-10)
				break
			}
			fmt.Printf("  %s\n", v)
		}
	}

	fmt.Println()
	color.Blue("Install:")
	fmt.Printf("  droy-pm install %s\n", name)
	fmt.Println()
}

var docsCmd = &cobra.Command{
	Use:   "docs [package]",
	Short: "Open package documentation",
	Long:  `Open the documentation for a package in your browser.`,
	Run: func(cmd *cobra.Command, args []string) {
		var url string

		if len(args) == 0 {
			// Try to get from droy.toml
			pkg, err := config.ReadPackageConfig("droy.toml")
			if err == nil && pkg.Homepage != "" {
				url = pkg.Homepage
			} else {
				url = "https://docs.droy-lang.org"
			}
		} else {
			name := args[0]
			if aliased, exists := packageAliases[name]; exists {
				name = aliased
			}
			url = fmt.Sprintf("https://docs.droy-lang.org/%s", name)
		}

		logger.Info("Opening documentation: %s", url)

		// Try to open browser
		var cmd_str string
		var args_str []string

		switch os := os.Getenv("GOOS"); os {
		case "darwin":
			cmd_str = "open"
			args_str = []string{url}
		case "windows":
			cmd_str = "cmd"
			args_str = []string{"/c", "start", url}
		default:
			cmd_str = "xdg-open"
			args_str = []string{url}
		}

		if err := exec.Command(cmd_str, args_str...).Start(); err != nil {
			logger.Error("Failed to open browser: %v", err)
			logger.Info("Please visit: %s", url)
		}
	},
}

func init() {
	rootCmd.AddCommand(infoCmd)
	rootCmd.AddCommand(docsCmd)
}
