package cmd

import (
	"os"

	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var (
	version = "1.0.0"
	rootCmd = &cobra.Command{
		Use:   "droy-pm",
		Short: color.CyanString("Droy Package Manager - Manage packages for Droy language"),
		Long: color.CyanString(`
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║   ██████╗ ██████╗  ██████╗ ██╗   ██╗    ██████╗ ███╗   ███╗  ║
║   ██╔══██╗██╔══██╗██╔═══██╗╚██╗ ██╔╝    ██╔══██╗████╗ ████║  ║
║   ██║  ██║██████╔╝██║   ██║ ╚████╔╝     ██████╔╝██╔████╔██║  ║
║   ██║  ██║██╔══██╗██║   ██║  ╚██╔╝      ██╔═══╝ ██║╚██╔╝██║  ║
║   ██████╔╝██║  ██║╚██████╔╝   ██║       ██║     ██║ ╚═╝ ██║  ║
║   ╚═════╝ ╚═╝  ╚═╝ ╚═════╝    ╚═╝       ╚═╝     ╚═╝     ╚═╝  ║
║                                                              ║
║          Package Manager for Droy Programming Language       ║
║                        Version `) + color.YellowString(version) + color.CyanString(`                       ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝

`),
	}
)

func Execute() {
	rootCmd.CompletionOptions.DisableDefaultCmd = true
	if err := rootCmd.Execute(); err != nil {
		color.Red("Error: %v", err)
		os.Exit(1)
	}
}

func init() {
	// Project management
	rootCmd.AddCommand(newCmd)
	rootCmd.AddCommand(initCmd)

	// Package management
	rootCmd.AddCommand(installCmd)
	rootCmd.AddCommand(uninstallCmd)
	rootCmd.AddCommand(updateCmd)
	rootCmd.AddCommand(searchCmd)
	rootCmd.AddCommand(listCmd)
	rootCmd.AddCommand(publishCmd)
	rootCmd.AddCommand(cleanCmd)

	// Development
	rootCmd.AddCommand(runCmd)
	rootCmd.AddCommand(buildCmd)
	rootCmd.AddCommand(testCmd)
	rootCmd.AddCommand(fmtCmd)
	rootCmd.AddCommand(lintCmd)

	// Information
	rootCmd.AddCommand(infoCmd)
	rootCmd.AddCommand(depsCmd)
	rootCmd.AddCommand(versionCmd)
}
