package cmd

import (
	"fmt"

	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var versionCmd = &cobra.Command{
	Use:   "version",
	Short: "Show version information",
	Long:  `Display the version of droy-pm and related information.`,
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println()
		color.Cyan(`
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║   ██████╗ ██████╗  ██████╗ ██╗   ██╗    ██████╗ ███╗   ███╗  ║
║   ██╔══██╗██╔══██╗██╔═══██╗╚██╗ ██╔╝    ██╔══██╗████╗ ████║  ║
║   ██║  ██║██████╔╝██║   ██║ ╚████╔╝     ██████╔╝██╔████╔██║  ║
║   ██║  ██║██╔══██╗██║   ██║  ╚██╔╝      ██╔═══╝ ██║╚██╔╝██║  ║
║   ██████╔╝██║  ██║╚██████╔╝   ██║       ██║     ██║ ╚═╝ ██║  ║
║   ╚═════╝ ╚═╝  ╚═╝ ╚═════╝    ╚═╝       ╚═╝     ╚═╝     ╚═╝  ║
║                                                              ║
║                    Package Manager                           ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝
`)
		fmt.Println()
		color.Yellow("  Version:    %s", version)
		color.Yellow("  Go Version: 1.21+")
		color.Yellow("  Platform:   Multi-platform")
		color.Yellow("  License:    MIT")
		fmt.Println()
		color.Cyan("  For Droy Programming Language")
		color.Cyan("  https://github.com/droy-go/droy-lang")
		fmt.Println()
	},
}
