package cmd

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var depsCmd = &cobra.Command{
	Use:   "deps",
	Short: "Show dependency information",
	Long:  `Display detailed information about package dependencies.`,
	Run: func(cmd *cobra.Command, args []string) {
		pkg, err := config.ReadPackageConfig("droy.toml")
		if err != nil {
			logger.Error("Failed to read droy.toml: %v", err)
			return
		}

		color.Cyan("📦 Dependency Information for %s@%s\n", pkg.Name, pkg.Version)

		// Count dependencies
		depCount := len(pkg.Dependencies)
		devDepCount := len(pkg.DevDependencies)

		fmt.Printf("Total Dependencies: %d\n", depCount+devDepCount)
		fmt.Printf("  Production: %d\n", depCount)
		fmt.Printf("  Development: %d\n\n", devDepCount)

		// Check for missing dependencies
		if _, err := os.Stat("droy_modules"); os.IsNotExist(err) {
			logger.Warning("Dependencies not installed. Run 'droy-pm install'")
			return
		}

		// Check installed vs required
		missing := []string{}
		outdated := []string{}

		for name, requiredVersion := range pkg.Dependencies {
			pkgPath := filepath.Join("droy_modules", name, "droy.toml")
			if _, err := os.Stat(pkgPath); os.IsNotExist(err) {
				missing = append(missing, name)
			} else if installedPkg, err := config.ReadPackageConfig(pkgPath); err == nil {
				if installedPkg.Version != requiredVersion {
					outdated = append(outdated, fmt.Sprintf("%s (%s -> %s)", 
						name, installedPkg.Version, requiredVersion))
				}
			}
		}

		if len(missing) > 0 {
			color.Red("\n⚠ Missing Dependencies:\n")
			for _, name := range missing {
				fmt.Printf("  - %s\n", name)
			}
		}

		if len(outdated) > 0 {
			color.Yellow("\n⚡ Outdated Dependencies:\n")
			for _, info := range outdated {
				fmt.Printf("  - %s\n", info)
			}
		}

		if len(missing) == 0 && len(outdated) == 0 {
			color.Green("\n✓ All dependencies are installed and up to date!")
		}
	},
}
