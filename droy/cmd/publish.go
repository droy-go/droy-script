package cmd

import (
	"archive/tar"
	"compress/gzip"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/droy-go/droy-pm/pkg/config"
	"github.com/droy-go/droy-pm/pkg/registry"
	"github.com/droy-go/droy-pm/internal/logger"
	"github.com/spf13/cobra"
)

var (
	publishRegistry string
	publishDryRun   bool
)

var publishCmd = &cobra.Command{
	Use:   "publish",
	Short: "Publish package to registry",
	Long: `Publish your Droy package to the registry.
This will create a tarball and upload it to the specified registry.`,
	Run: func(cmd *cobra.Command, args []string) {
		logger.Info("Preparing package for publication...")

		// Read package config
		pkg, err := config.ReadPackageConfig("droy.toml")
		if err != nil {
			logger.Error("Failed to read droy.toml: %v", err)
			return
		}

		// Validate package
		if err := validatePackage(pkg); err != nil {
			logger.Error("Validation failed: %v", err)
			return
		}

		// Create tarball
		tarballPath, err := createTarball(pkg)
		if err != nil {
			logger.Error("Failed to create tarball: %v", err)
			return
		}

		if publishDryRun {
			logger.Info("Dry run - tarball created at: %s", tarballPath)
			logger.Success("Package '%s' v%s is ready for publication", pkg.Name, pkg.Version)
			return
		}

		// Publish to registry
		reg := registry.New(publishRegistry)
		if err := reg.Publish(pkg, tarballPath); err != nil {
			logger.Error("Failed to publish: %v", err)
			os.Remove(tarballPath)
			return
		}

		// Clean up tarball
		os.Remove(tarballPath)

		logger.Success("Published %s@%s to %s", pkg.Name, pkg.Version, publishRegistry)
	},
}

func validatePackage(pkg *config.Package) error {
	if pkg.Name == "" {
		return fmt.Errorf("package name is required")
	}
	if pkg.Version == "" {
		return fmt.Errorf("package version is required")
	}
	if !isValidVersion(pkg.Version) {
		return fmt.Errorf("invalid version format: %s", pkg.Version)
	}

	// Check if src directory exists
	if _, err := os.Stat("src"); os.IsNotExist(err) {
		return fmt.Errorf("src directory not found")
	}

	return nil
}

func isValidVersion(version string) bool {
	// Simple semver validation
	parts := strings.Split(version, ".")
	if len(parts) != 3 {
		return false
	}
	return true
}

func createTarball(pkg *config.Package) (string, error) {
	filename := fmt.Sprintf("%s-%s.tgz", pkg.Name, pkg.Version)
	path := filepath.Join(os.TempDir(), filename)

	file, err := os.Create(path)
	if err != nil {
		return "", err
	}
	defer file.Close()

	gzWriter := gzip.NewWriter(file)
	defer gzWriter.Close()

	tarWriter := tar.NewWriter(gzWriter)
	defer tarWriter.Close()

	// Add files to tarball
	files := []string{
		"droy.toml",
		"README.md",
		"LICENSE",
	}

	// Add src directory
	err = filepath.Walk("src", func(file string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		header, err := tar.FileInfoHeader(info, info.Name())
		if err != nil {
			return err
		}

		header.Name = filepath.ToSlash(file)

		if err := tarWriter.WriteHeader(header); err != nil {
			return err
		}

		if !info.IsDir() {
			data, err := os.Open(file)
			if err != nil {
				return err
			}
			defer data.Close()

			if _, err := io.Copy(tarWriter, data); err != nil {
				return err
			}
		}

		return nil
	})

	if err != nil {
		return "", err
	}

	// Add other files
	for _, file := range files {
		if _, err := os.Stat(file); os.IsNotExist(err) {
			continue
		}

		info, err := os.Stat(file)
		if err != nil {
			continue
		}

		header, err := tar.FileInfoHeader(info, info.Name())
		if err != nil {
			continue
		}

		header.Name = file
		header.ModTime = time.Now()

		if err := tarWriter.WriteHeader(header); err != nil {
			continue
		}

		data, err := os.Open(file)
		if err != nil {
			continue
		}

		if _, err := io.Copy(tarWriter, data); err != nil {
			data.Close()
			continue
		}
		data.Close()
	}

	return path, nil
}

func init() {
	publishCmd.Flags().StringVarP(&publishRegistry, "registry", "r", "https://registry.droy-lang.org", "Registry URL")
	publishCmd.Flags().BoolVarP(&publishDryRun, "dry-run", "d", false, "Prepare but don't publish")
}
