package cmd

import (
	"os"
	"path/filepath"

	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/spf13/cobra"
)

var cleanCmd = &cobra.Command{
	Use:   "clean",
	Short: "Clean cached packages",
	Long:  `Remove cached packages and temporary files.`,
	Run: func(cmd *cobra.Command, args []string) {
		logger.Info("Cleaning up...")

		// Remove droy_modules
		if err := os.RemoveAll("droy_modules"); err != nil {
			logger.Warning("Failed to remove droy_modules: %v", err)
		} else {
			logger.Info("Removed droy_modules/")
		}

		// Remove lock file
		if err := os.Remove("droy.lock"); err != nil {
			logger.Warning("Failed to remove droy.lock: %v", err)
		} else {
			logger.Info("Removed droy.lock")
		}

		// Clean cache
		homeDir, err := os.UserHomeDir()
		if err == nil {
			cachePath := filepath.Join(homeDir, ".droy", "cache")
			if err := os.RemoveAll(cachePath); err != nil {
				logger.Warning("Failed to clean cache: %v", err)
			} else {
				logger.Info("Cleaned cache")
			}
		}

		logger.Success("Cleanup complete")
	},
}
