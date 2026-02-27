package cmd

import (
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var (
	listGlobal  bool
	listTree    bool
	listOutdated bool
)

var listCmd = &cobra.Command{
	Use:     "list",
	Aliases: []string{"ls"},
	Short:   "List installed packages",
	Long:    `List all installed packages and their versions.`,
	Run: func(cmd *cobra.Command, args []string) {
		if listGlobal {
			listGlobalPackages()
			return
		}

		// Read package config
		pkg, err := config.ReadPackageConfig("droy.toml")
		if err != nil {
			logger.Error("Failed to read droy.toml: %v", err)
			return
		}

		if listTree {
			printDependencyTree(pkg)
			return
		}

		printPackageList(pkg)
	},
}

func listGlobalPackages() {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		logger.Error("Failed to get home directory: %v", err)
		return
	}

	globalPath := filepath.Join(homeDir, ".droy", "global")
	if _, err := os.Stat(globalPath); os.IsNotExist(err) {
		logger.Info("No global packages installed")
		return
	}

	entries, err := os.ReadDir(globalPath)
	if err != nil {
		logger.Error("Failed to read global packages: %v", err)
		return
	}

	if len(entries) == 0 {
		logger.Info("No global packages installed")
		return
	}

	color.Cyan("Global packages:\n")
	for _, entry := range entries {
		if entry.IsDir() {
			fmt.Printf("  %s\n", entry.Name())
		}
	}
}

func printPackageList(pkg *config.Package) {
	color.Cyan("📦 %s@%s\n", pkg.Name, pkg.Version)
	
	if len(pkg.Dependencies) > 0 {
		color.Green("\nDependencies:\n")
		printDeps(pkg.Dependencies)
	}

	if len(pkg.DevDependencies) > 0 {
		color.Yellow("\nDev Dependencies:\n")
		printDeps(pkg.DevDependencies)
	}

	// Check installed packages
	if _, err := os.Stat("droy_modules"); !os.IsNotExist(err) {
		color.Blue("\nInstalled Packages:\n")
		printInstalledPackages()
	}
}

func printDeps(deps map[string]string) {
	// Sort keys
	var keys []string
	for k := range deps {
		keys = append(keys, k)
	}
	sort.Strings(keys)

	for _, name := range keys {
		version := deps[name]
		fmt.Printf("  %s %s\n", color.CyanString(name), color.WhiteString(version))
	}
}

func printInstalledPackages() {
	entries, err := os.ReadDir("droy_modules")
	if err != nil {
		return
	}

	for _, entry := range entries {
		if !entry.IsDir() {
			continue
		}

		// Try to read package info
		pkgPath := filepath.Join("droy_modules", entry.Name(), "droy.toml")
		if pkg, err := config.ReadPackageConfig(pkgPath); err == nil {
			fmt.Printf("  %s %s\n", 
				color.CyanString(pkg.Name), 
				color.WhiteString(pkg.Version))
		} else {
			fmt.Printf("  %s\n", color.CyanString(entry.Name()))
		}
	}
}

func printDependencyTree(pkg *config.Package) {
	color.Cyan("📦 %s@%s\n\n", pkg.Name, pkg.Version)
	
	if len(pkg.Dependencies) > 0 {
		color.Green("Dependency Tree:\n")
		printTree("droy_modules", "", true)
	}
}

func printTree(path, prefix string, isLast bool) {
	entries, err := os.ReadDir(path)
	if err != nil {
		return
	}

	var dirs []string
	for _, entry := range entries {
		if entry.IsDir() {
			dirs = append(dirs, entry.Name())
		}
	}

	for i, dir := range dirs {
		isLastItem := i == len(dirs)-1
		
		connector := "├── "
		if isLastItem {
			connector = "└── "
		}

		fmt.Printf("%s%s%s\n", prefix, connector, color.CyanString(dir))

		subPath := filepath.Join(path, dir, "droy_modules")
		if _, err := os.Stat(subPath); !os.IsNotExist(err) {
			subPrefix := prefix + "│   "
			if isLastItem {
				subPrefix = prefix + "    "
			}
			printTree(subPath, subPrefix, isLastItem)
		}
	}
}

func init() {
	listCmd.Flags().BoolVarP(&listGlobal, "global", "g", false, "List global packages")
	listCmd.Flags().BoolVarP(&listTree, "tree", "t", false, "Show dependency tree")
	listCmd.Flags().BoolVarP(&listOutdated, "outdated", "o", false, "Show outdated packages")
}
