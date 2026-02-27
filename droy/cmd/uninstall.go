package cmd

import (
	"os"
	"path/filepath"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/spf13/cobra"
)

var uninstallCmd = &cobra.Command{
	Use:     "uninstall <package>",
	Aliases: []string{"remove", "rm"},
	Short:   "Uninstall a package",
	Long:    `Remove a package from droy_modules and update droy.toml.`,
	Args:    cobra.ExactArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		name := args[0]
		
		logger.Info("Uninstalling %s...", name)

		// Remove from droy_modules
		modulePath := filepath.Join("droy_modules", name)
		if err := os.RemoveAll(modulePath); err != nil {
			logger.Error("Failed to remove package directory: %v", err)
			return
		}

		// Update droy.toml
		pkg, err := config.ReadPackageConfig("droy.toml")
		if err == nil {
			delete(pkg.Dependencies, name)
			delete(pkg.DevDependencies, name)
			
			if err := config.WritePackageConfig(pkg, "droy.toml"); err != nil {
				logger.Warning("Failed to update droy.toml: %v", err)
			}
		}

		// Update lock file
		lock, err := config.ReadLockFile("droy.lock")
		if err == nil {
			delete(lock.Dependencies, name)
			
			if err := config.WriteLockFile(lock, "droy.lock"); err != nil {
				logger.Warning("Failed to update lock file: %v", err)
			}
		}

		logger.Success("Uninstalled %s", name)
	},
}
