package cmd

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var (
	newTemplate string
	newSkipGit  bool
)

var newCmd = &cobra.Command{
	Use:   "new <project-name>",
	Short: "Create a new Droy project",
	Long: `Create a new Droy project with the specified name.
This initializes a new project directory with a basic structure.`,
	Example: `  droy-pm new my-app           # Create new project
  droy-pm new my-app --template=cli  # Use CLI template
  droy-pm new my-app --template=lib  # Use library template`,
	Args: cobra.ExactArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		projectName := args[0]
		
		logger.Info("Creating new Droy project: %s", projectName)

		// Create project directory
		if err := os.MkdirAll(projectName, 0755); err != nil {
			logger.Error("Failed to create directory: %v", err)
			return
		}

		// Create project structure based on template
		switch newTemplate {
		case "cli":
			createCliTemplate(projectName)
		case "lib", "library":
			createLibTemplate(projectName)
		case "web":
			createWebTemplate(projectName)
		case "api":
			createApiTemplate(projectName)
		default:
			createDefaultTemplate(projectName)
		}

		// Initialize git
		if !newSkipGit {
			initGit(projectName)
		}

		logger.Success("Project '%s' created successfully!", projectName)
		fmt.Println()
		color.Cyan("Next steps:")
		fmt.Printf("  cd %s\n", projectName)
		fmt.Println("  droy-pm install    # Install dependencies")
		fmt.Println("  droy-pm run        # Run the project")
	},
}

func createDefaultTemplate(name string) {
	// Create directories
	os.MkdirAll(filepath.Join(name, "src"), 0755)
	os.MkdirAll(filepath.Join(name, "tests"), 0755)

	// droy.toml
	droyToml := fmt.Sprintf(`# %s - Droy Project
name = "%s"
version = "1.0.0"
description = "A Droy project"
author = ""
license = "MIT"
droy_version = ">=1.0.0"
main = "src/main.droy"

[scripts]
build = "droy-pm build"
test = "droy-pm test"
run = "droy-pm run"
start = "droy-pm run"

[dependencies]
`, name, name)
	os.WriteFile(filepath.Join(name, "droy.toml"), []byte(droyToml), 0644)

	// src/main.droy
	mainDroy := fmt.Sprintf(`// %s - Main Entry Point
pkg "%s"

~s @si = "Hello, World!"
em @si

f greet(name) {
    ret "Hello, " + name + "!"
}

em greet("Droy")
`, name, name)
	os.WriteFile(filepath.Join(name, "src", "main.droy"), []byte(mainDroy), 0644)

	// tests/main_test.droy
	testDroy := `// Tests for the project
pkg "test"

f test_greet() {
    ~s result = greet("Test")
    fe (result == "Hello, Test!") {
        em "✓ test_greet passed"
    } else {
        em "✗ test_greet failed"
    }
}

test_greet()
`
	os.WriteFile(filepath.Join(name, "tests", "main_test.droy"), []byte(testDroy), 0644)

	// README.md
	readme := fmt.Sprintf(`# %s

A Droy project.

## Installation

\`\`\`bash
droy-pm install
\`\`\`

## Usage

\`\`\`bash
droy-pm run
\`\`\`

## Scripts

- \`droy-pm run\` - Run the project
- \`droy-pm build\` - Build the project
- \`droy-pm test\` - Run tests

## License

MIT
`, name)
	os.WriteFile(filepath.Join(name, "README.md"), []byte(readme), 0644)

	// .gitignore
	gitignore := `# Droy
.droy/
droy_modules/
droy.lock
*.log

# Build
dist/
build/
*.ll
*.s
*.o
*.out
*.exe

# IDE
.idea/
.vscode/
*.swp
*.swo
*~

# OS
.DS_Store
Thumbs.db
`
	os.WriteFile(filepath.Join(name, ".gitignore"), []byte(gitignore), 0644)
}

func createCliTemplate(name string) {
	createDefaultTemplate(name)

	// Update main.droy for CLI
	mainDroy := fmt.Sprintf(`// %s - CLI Application
pkg "%s"

~s @si = "CLI Tool"
~s version = "1.0.0"

f showHelp() {
    em @si + " v" + version
    em ""
    em "Usage:"
    em "  droy run src/main.droy [command]"
    em ""
    em "Commands:"
    em "  hello    Print hello message"
    em "  version  Show version"
    em "  help     Show this help"
}

f main(args) {
    fe (args == "hello") {
        em "Hello from CLI!"
    } else fe (args == "version") {
        em version
    } else {
        showHelp()
    }
}

// Run with default
main("")
`, name, name)
	os.WriteFile(filepath.Join(name, "src", "main.droy"), []byte(mainDroy), 0644)

	// Update droy.toml
	droyToml := fmt.Sprintf(`# %s - CLI Application
name = "%s"
version = "1.0.0"
description = "A CLI tool built with Droy"
author = ""
license = "MIT"
droy_version = ">=1.0.0"
main = "src/main.droy"

[bin]
%s = "src/main.droy"

[scripts]
build = "droy-pm build -o %s"
test = "droy-pm test"
run = "droy-pm run"

[dependencies]
droy-cli = "^1.0.0"
`, name, name, name, name)
	os.WriteFile(filepath.Join(name, "droy.toml"), []byte(droyToml), 0644)
}

func createLibTemplate(name string) {
	createDefaultTemplate(name)

	// Update main.droy for library
	mainDroy := fmt.Sprintf(`// %s - Library
pkg "%s"

// Export functions for use by other packages

f add(a, b) {
    ret a + b
}

f subtract(a, b) {
    ret a - b
}

f multiply(a, b) {
    ret a * b
}

f divide(a, b) {
    ret a / b
}
`, name, name)
	os.WriteFile(filepath.Join(name, "src", "main.droy"), []byte(mainDroy), 0644)

	// Update droy.toml
	droyToml := fmt.Sprintf(`# %s - Library
name = "%s"
version = "1.0.0"
description = "A library for Droy"
author = ""
license = "MIT"
droy_version = ">=1.0.0"
main = "src/main.droy"

[scripts]
build = "droy-pm build"
test = "droy-pm test"

[dependencies]
`, name, name)
	os.WriteFile(filepath.Join(name, "droy.toml"), []byte(droyToml), 0644)
}

func createWebTemplate(name string) {
	createDefaultTemplate(name)

	// Add public directory
	os.MkdirAll(filepath.Join(name, "public"), 0755)

	// Create index.html
	html := `<!DOCTYPE html>
<html>
<head>
    <title>Droy Web App</title>
</head>
<body>
    <h1>Hello from Droy!</h1>
    <div id="app"></div>
</body>
</html>
`
	os.WriteFile(filepath.Join(name, "public", "index.html"), []byte(html), 0644)

	// Update main.droy
	mainDroy := `// Web Application
pkg "web-app"

~s @si = "Web Server Starting..."
em @si

// Web server logic would go here
`
	os.WriteFile(filepath.Join(name, "src", "main.droy"), []byte(mainDroy), 0644)

	// Update droy.toml
	droyToml := fmt.Sprintf(`# %s - Web Application
name = "%s"
version = "1.0.0"
description = "A web application built with Droy"
author = ""
license = "MIT"
droy_version = ">=1.0.0"
main = "src/main.droy"

[scripts]
build = "droy-pm build"
test = "droy-pm test"
run = "droy-pm run"
serve = "droy-pm run && python3 -m http.server 8080"

[dependencies]
droy-http = "^1.0.0"
`, name, name)
	os.WriteFile(filepath.Join(name, "droy.toml"), []byte(droyToml), 0644)
}

func createApiTemplate(name string) {
	createDefaultTemplate(name)

	// Update main.droy for API
	mainDroy := fmt.Sprintf(`// %s - API Server
pkg "%s"

~s @si = "API Server"
em @si + " starting..."

// API routes would be defined here
// Example:
// route "/users" {
//     get { ret getUsers() }
//     post { ret createUser() }
// }
`, name, name)
	os.WriteFile(filepath.Join(name, "src", "main.droy"), []byte(mainDroy), 0644)

	// Update droy.toml
	droyToml := fmt.Sprintf(`# %s - API Server
name = "%s"
version = "1.0.0"
description = "An API server built with Droy"
author = ""
license = "MIT"
droy_version = ">=1.0.0"
main = "src/main.droy"

[scripts]
build = "droy-pm build"
test = "droy-pm test"
run = "droy-pm run"
dev = "droy-pm run --watch"

[dependencies]
droy-http = "^1.0.0"
droy-json = "^1.0.0"
`, name, name)
	os.WriteFile(filepath.Join(name, "droy.toml"), []byte(droyToml), 0644)
}

func initGit(projectPath string) {
	// Check if git is available
	if _, err := exec.LookPath("git"); err != nil {
		logger.Warning("Git not found, skipping initialization")
		return
	}

	// Initialize git repository
	cmd := exec.Command("git", "init")
	cmd.Dir = projectPath
	if err := cmd.Run(); err != nil {
		logger.Warning("Failed to initialize git: %v", err)
		return
	}

	// Initial commit
	cmd = exec.Command("git", "add", ".")
	cmd.Dir = projectPath
	cmd.Run()

	cmd = exec.Command("git", "commit", "-m", "Initial commit")
	cmd.Dir = projectPath
	cmd.Run()

	logger.Info("Initialized git repository")
}

func init() {
	newCmd.Flags().StringVarP(&newTemplate, "template", "t", "", "Project template (default, cli, lib, web, api)")
	newCmd.Flags().BoolVar(&newSkipGit, "skip-git", false, "Skip git initialization")

	rootCmd.AddCommand(newCmd)
}
