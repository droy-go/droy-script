package cmd

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var initCmd = &cobra.Command{
	Use:   "init [path]",
	Short: "Initialize a new Droy package",
	Long:  `Initialize a new Droy package by creating a droy.toml configuration file.`,
	Args:  cobra.MaximumNArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		path := "."
		if len(args) > 0 {
			path = args[0]
		}

		logger.Info("Initializing new Droy package...")

		// Get package information interactively
		reader := bufio.NewReader(os.Stdin)

		fmt.Print(color.CyanString("Package name [" + filepath.Base(path) + "]: "))
		name, _ := reader.ReadString('\n')
		name = strings.TrimSpace(name)
		if name == "" {
			name = filepath.Base(path)
		}

		fmt.Print(color.CyanString("Version [1.0.0]: "))
		version, _ := reader.ReadString('\n')
		version = strings.TrimSpace(version)
		if version == "" {
			version = "1.0.0"
		}

		fmt.Print(color.CyanString("Description: "))
		description, _ := reader.ReadString('\n')
		description = strings.TrimSpace(description)

		fmt.Print(color.CyanString("Author: "))
		author, _ := reader.ReadString('\n')
		author = strings.TrimSpace(author)

		fmt.Print(color.CyanString("License [MIT]: "))
		license, _ := reader.ReadString('\n')
		license = strings.TrimSpace(license)
		if license == "" {
			license = "MIT"
		}

		fmt.Print(color.CyanString("Repository URL: "))
		repository, _ := reader.ReadString('\n')
		repository = strings.TrimSpace(repository)

		// Create package config
		pkg := &config.Package{
			Name:        name,
			Version:     version,
			Description: description,
			Author:      author,
			License:     license,
			Repository:  repository,
			DroyVersion: ">=1.0.0",
			Dependencies: make(map[string]string),
		}

		// Create directory if it doesn't exist
		if path != "." {
			if err := os.MkdirAll(path, 0755); err != nil {
				logger.Error("Failed to create directory: %v", err)
				return
			}
		}

		// Write configuration
		configPath := filepath.Join(path, "droy.toml")
		if err := config.WritePackageConfig(pkg, configPath); err != nil {
			logger.Error("Failed to write configuration: %v", err)
			return
		}

		// Create src directory
		srcPath := filepath.Join(path, "src")
		if err := os.MkdirAll(srcPath, 0755); err != nil {
			logger.Error("Failed to create src directory: %v", err)
			return
		}

		// Create example main.droy file
		mainFile := filepath.Join(srcPath, "main.droy")
		mainContent := fmt.Sprintf(`// %s - %s
// Author: %s
// Version: %s

pkg "%s"

~s @si = "Hello from %s!"
em @si
`, name, description, author, version, name, name)

		if err := os.WriteFile(mainFile, []byte(mainContent), 0644); err != nil {
			logger.Error("Failed to create main.droy: %v", err)
			return
		}

		// Create .gitignore
		gitignorePath := filepath.Join(path, ".gitignore")
		gitignoreContent := `# Droy Package Manager
droy_modules/
droy.lock
*.log
`
		if err := os.WriteFile(gitignorePath, []byte(gitignoreContent), 0644); err != nil {
			logger.Error("Failed to create .gitignore: %v", err)
			return
		}

		// Create README.md
		readmePath := filepath.Join(path, "README.md")
		readmeContent := fmt.Sprintf(`# %s

%s

## Installation

\`\`\`bash
droy-pm install %s
\`\`\`

## Usage

\`\`\`droy
pkg "%s"

// Your code here
\`\`\`

## License

%s
`, name, description, name, name, license)

		if err := os.WriteFile(readmePath, []byte(readmeContent), 0644); err != nil {
			logger.Error("Failed to create README.md: %v", err)
			return
		}

		logger.Success("Package '%s' initialized successfully!", name)
		logger.Info("Configuration: %s", configPath)
		logger.Info("Source files: %s/", srcPath)
	},
}
