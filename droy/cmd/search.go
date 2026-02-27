package cmd

import (
	"github.com/droy-go/droy-pm/pkg/registry"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var searchRegistry string

var searchCmd = &cobra.Command{
	Use:   "search <query>",
	Short: "Search for packages",
	Long:  `Search for packages in the Droy registry.`,
	Args:  cobra.ExactArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		query := args[0]
		
		logger.Info("Searching for '%s'...", query)

		reg := registry.New(searchRegistry)
		results, err := reg.Search(query)
		if err != nil {
			logger.Error("Search failed: %v", err)
			return
		}

		if len(results) == 0 {
			logger.Warning("No packages found matching '%s'", query)
			return
		}

		logger.Success("Found %d packages:\n", len(results))
		
		for _, pkg := range results {
			color.Cyan("  %s", pkg.Name)
			color.White("    %s", pkg.Description)
			color.Yellow("    Version: %s | Author: %s | License: %s\n", 
				pkg.Version, pkg.Author, pkg.License)
		}
	},
}

func init() {
	searchCmd.Flags().StringVarP(&searchRegistry, "registry", "r", "https://registry.droy-lang.org", "Registry URL")
}
