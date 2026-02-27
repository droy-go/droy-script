package cmd

import (
	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/pkg/installer"
	"github.com/droy-go/droy-pm/pkg/registry"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/spf13/cobra"
)

var updateCmd = &cobra.Command{
	Use:   "update [package]",
	Short: "Update packages",
	Long: `Update packages to their latest versions.
If no package is specified, updates all dependencies.`,
	Run: func(cmd *cobra.Command, args []string) {
		if len(args) == 0 {
			updateAll()
		} else {
			updatePackage(args[0])
		}
	},
}

func updateAll() {
	logger.Info("Updating all dependencies...")

	pkg, err := config.ReadPackageConfig("droy.toml")
	if err != nil {
		logger.Error("Failed to read droy.toml: %v", err)
		return
	}

	reg := registry.New("")
	inst := installer.New("droy_modules")
	updated := 0

	for name, currentVersion := range pkg.Dependencies {
		latest, err := reg.GetLatestVersion(name)
		if err != nil {
			logger.Warning("Could not check updates for %s: %v", name, err)
			continue
		}

		if latest != currentVersion {
			logger.Info("Updating %s: %s -> %s", name, currentVersion, latest)
			
			// Remove old version
			inst.Uninstall(name)
			
			// Install new version
			if err := inst.Install(name, latest); err != nil {
				logger.Error("Failed to update %s: %v", name, err)
				continue
			}

			pkg.Dependencies[name] = latest
			updated++
		}
	}

	// Update droy.toml
	if err := config.WritePackageConfig(pkg, "droy.toml"); err != nil {
		logger.Warning("Failed to update droy.toml: %v", err)
	}

	logger.Success("Updated %d packages", updated)
}

func updatePackage(name string) {
	logger.Info("Checking for updates to %s...", name)

	pkg, err := config.ReadPackageConfig("droy.toml")
	if err != nil {
		logger.Error("Failed to read droy.toml: %v", err)
		return
	}

	currentVersion, exists := pkg.Dependencies[name]
	if !exists {
		logger.Error("Package %s not found in dependencies", name)
		return
	}

	reg := registry.New("")
	latest, err := reg.GetLatestVersion(name)
	if err != nil {
		logger.Error("Could not check updates: %v", err)
		return
	}

	if latest == currentVersion {
		logger.Info("%s is already up to date (%s)", name, currentVersion)
		return
	}

	logger.Info("Updating %s: %s -> %s", name, currentVersion, latest)

	inst := installer.New("droy_modules")
	inst.Uninstall(name)
	
	if err := inst.Install(name, latest); err != nil {
		logger.Error("Failed to update %s: %v", name, err)
		return
	}

	pkg.Dependencies[name] = latest
	if err := config.WritePackageConfig(pkg, "droy.toml"); err != nil {
		logger.Warning("Failed to update droy.toml: %v", err)
	}

	logger.Success("Updated %s to %s", name, latest)
}
