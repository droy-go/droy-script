package cmd

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/pkg/installer"
	"github.com/droy-go/droy-pm/pkg/resolver"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/fatih/color"
	"github.com/spf13/cobra"
)

var (
	installGlobal bool
	installDev    bool
	installSave   bool
)

var installCmd = &cobra.Command{
	Use:     "install [package[@version]]",
	Aliases: []string{"i", "add"},
	Short:   "Install packages",
	Long: `Install packages from the Droy registry or GitHub.
If no package is specified, installs all dependencies from droy.toml.

Common Droy packages:
  http      - HTTP client library (droy-http)
  json      - JSON parsing (droy-json)
  cli       - CLI utilities (droy-cli)
  test      - Testing framework (droy-test)
  fs        - File system utilities (droy-fs)
  net       - Network utilities (droy-net)`,
	Example: `  droy-pm install                    # Install all dependencies
  droy-pm install http               # Install droy-http package
  droy-pm install json@2.0.0         # Install specific version
  droy-pm install mypackage          # Install from registry
  droy-pm install github.com/user/repo # Install from GitHub`,
	Run: func(cmd *cobra.Command, args []string) {
		if len(args) == 0 {
			// Install all dependencies from droy.toml
			installAllDependencies()
		} else {
			// Install specific package
			installPackage(args[0])
		}
	},
}

func installAllDependencies() {
	logger.Info("Reading package configuration...")

	pkg, err := config.ReadPackageConfig("droy.toml")
	if err != nil {
		logger.Error("Failed to read droy.toml: %v", err)
		logger.Info("Run 'droy-pm init' to create a new package")
		return
	}

	if len(pkg.Dependencies) == 0 && len(pkg.DevDependencies) == 0 {
		logger.Warning("No dependencies found in droy.toml")
		return
	}

	logger.Info("Installing dependencies for '%s'...", pkg.Name)

	// Resolve dependencies
	res := resolver.New()
	allDeps := make(map[string]string)

	for name, version := range pkg.Dependencies {
		allDeps[name] = version
	}

	if installDev {
		for name, version := range pkg.DevDependencies {
			allDeps[name] = version
		}
	}

	resolved, err := res.Resolve(allDeps)
	if err != nil {
		logger.Error("Failed to resolve dependencies: %v", err)
		return
	}

	// Install each dependency
	inst := installer.New("droy_modules")
	total := len(resolved)
	installed := 0

	for name, version := range resolved {
		logger.Progress(installed+1, total, "Installing %s@%s", name, version)
		
		if err := inst.Install(name, version); err != nil {
			logger.Error("Failed to install %s@%s: %v", name, version, err)
			continue
		}
		installed++
	}

	// Create lock file
	lock := &config.LockFile{
		Version:      pkg.Version,
		Dependencies: resolved,
	}
	
	if err := config.WriteLockFile(lock, "droy.lock"); err != nil {
		logger.Warning("Failed to create lock file: %v", err)
	}

	logger.Success("Installed %d/%d packages", installed, total)
}

func installPackage(pkgSpec string) {
	// Parse package specification
	name, version := parsePackageSpec(pkgSpec)
	
	logger.Info("Installing %s@%s...", name, version)

	// Install the package
	inst := installer.New("droy_modules")
	if err := inst.Install(name, version); err != nil {
		logger.Error("Failed to install %s: %v", name, err)
		return
	}

	// Update droy.toml if --save flag is set
	if installSave {
		pkg, err := config.ReadPackageConfig("droy.toml")
		if err != nil {
			logger.Warning("Could not read droy.toml, skipping save")
		} else {
			if installDev {
				if pkg.DevDependencies == nil {
					pkg.DevDependencies = make(map[string]string)
				}
				pkg.DevDependencies[name] = "^" + version
			} else {
				pkg.Dependencies[name] = "^" + version
			}
			
			if err := config.WritePackageConfig(pkg, "droy.toml"); err != nil {
				logger.Warning("Failed to update droy.toml: %v", err)
			} else {
				logger.Info("Added to droy.toml")
			}
		}
	}

	logger.Success("Installed %s@%s", name, version)
}

// Common package aliases for Droy
var packageAliases = map[string]string{
	"http":      "droy-http",
	"json":      "droy-json",
	"xml":       "droy-xml",
	"cli":       "droy-cli",
	"test":      "droy-test",
	"fs":        "droy-fs",
	"net":       "droy-net",
	"crypto":    "droy-crypto",
	"db":        "droy-db",
	"web":       "droy-web",
	"log":       "droy-log",
	"time":      "droy-time",
	"regex":     "droy-regex",
	"math":      "droy-math",
	"string":    "droy-string",
	"array":     "droy-array",
	"object":    "droy-object",
	"event":     "droy-event",
	"stream":    "droy-stream",
	"buffer":    "droy-buffer",
	"path":      "droy-path",
	"os":        "droy-os",
	"process":   "droy-process",
	"env":       "droy-env",
	"color":     "droy-color",
	"table":     "droy-table",
	"chart":     "droy-chart",
	"image":     "droy-image",
	"audio":     "droy-audio",
	"video":     "droy-video",
	"websocket": "droy-websocket",
	"graphql":   "droy-graphql",
	"rest":      "droy-rest",
	"auth":      "droy-auth",
	"jwt":       "droy-jwt",
	"oauth":     "droy-oauth",
	"email":     "droy-email",
	"sms":       "droy-sms",
	"cache":     "droy-cache",
	"queue":     "droy-queue",
	"scheduler": "droy-scheduler",
	"worker":    "droy-worker",
	"task":      "droy-task",
	"job":       "droy-job",
	"cron":      "droy-cron",
	"backup":    "droy-backup",
	"sync":      "droy-sync",
	"async":     "droy-async",
	"promise":   "droy-promise",
	"future":    "droy-future",
	"channel":   "droy-channel",
	"mutex":     "droy-mutex",
	"semaphore": "droy-semaphore",
	"pool":      "droy-pool",
	"worker-pool": "droy-worker-pool",
}

func parsePackageSpec(spec string) (name, version string) {
	version = "latest"
	
	// Check for version specifier
	if idx := strings.LastIndex(spec, "@"); idx > 0 {
		name = spec[:idx]
		version = spec[idx+1:]
	} else {
		name = spec
	}

	// Check for aliases
	if aliased, exists := packageAliases[name]; exists {
		name = aliased
		logger.Info("Using package: %s", name)
	}

	return
}

func init() {
	installCmd.Flags().BoolVarP(&installGlobal, "global", "g", false, "Install package globally")
	installCmd.Flags().BoolVarP(&installDev, "dev", "D", false, "Install as dev dependency")
	installCmd.Flags().BoolVarP(&installSave, "save", "S", true, "Save to droy.toml")
}
