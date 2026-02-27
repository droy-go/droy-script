package installer

import (
	"archive/tar"
	"compress/gzip"
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"

	"github.com/go-git/go-git/v5"
)

// Installer handles package installation
type Installer struct {
	ModulesPath string
	CachePath   string
}

// New creates a new installer
func New(modulesPath string) *Installer {
	homeDir, _ := os.UserHomeDir()
	cachePath := filepath.Join(homeDir, ".droy", "cache")
	
	return &Installer{
		ModulesPath: modulesPath,
		CachePath:   cachePath,
	}
}

// Install installs a package
func (i *Installer) Install(name, version string) error {
	// Create modules directory if needed
	if err := os.MkdirAll(i.ModulesPath, 0755); err != nil {
		return fmt.Errorf("failed to create modules directory: %w", err)
	}

	// Determine package source
	if strings.HasPrefix(name, "github.com/") {
		return i.installFromGitHub(name, version)
	}

	return i.installFromRegistry(name, version)
}

// Uninstall removes a package
func (i *Installer) Uninstall(name string) error {
	pkgPath := filepath.Join(i.ModulesPath, name)
	if err := os.RemoveAll(pkgPath); err != nil {
		return fmt.Errorf("failed to remove package: %w", err)
	}
	return nil
}

func (i *Installer) installFromGitHub(repo, version string) error {
	// Parse repository URL
	parts := strings.Split(repo, "/")
	if len(parts) < 3 {
		return fmt.Errorf("invalid GitHub repository format")
	}

	owner := parts[1]
	repoName := parts[2]
	
	// Target directory
	targetDir := filepath.Join(i.ModulesPath, repoName)
	
	// Remove existing installation
	os.RemoveAll(targetDir)

	// Clone repository
	cloneURL := fmt.Sprintf("https://github.com/%s/%s.git", owner, repoName)
	
	cloneOptions := &git.CloneOptions{
		URL:      cloneURL,
		Progress: os.Stdout,
	}

	// Checkout specific version if not latest
	if version != "latest" && version != "*" {
		cloneOptions.ReferenceName = "refs/tags/" + version
		cloneOptions.SingleBranch = true
	}

	_, err := git.PlainClone(targetDir, false, cloneOptions)
	if err != nil {
		return fmt.Errorf("failed to clone repository: %w", err)
	}

	return nil
}

func (i *Installer) installFromRegistry(name, version string) error {
	// For now, assume registry returns a tarball URL
	// In a real implementation, this would query the registry API
	
	tarballURL := fmt.Sprintf("https://registry.droy-lang.org/%s/-/"+name+"-%s.tgz", name, version)
	
	// Download tarball
	tarballPath := filepath.Join(i.CachePath, fmt.Sprintf("%s-%s.tgz", name, version))
	
	if err := os.MkdirAll(i.CachePath, 0755); err != nil {
		return fmt.Errorf("failed to create cache directory: %w", err)
	}

	// Check cache first
	if _, err := os.Stat(tarballPath); os.IsNotExist(err) {
		if err := downloadFile(tarballURL, tarballPath); err != nil {
			return fmt.Errorf("failed to download package: %w", err)
		}
	}

	// Extract tarball
	targetDir := filepath.Join(i.ModulesPath, name)
	if err := extractTarball(tarballPath, targetDir); err != nil {
		return fmt.Errorf("failed to extract package: %w", err)
	}

	return nil
}

func downloadFile(url, path string) error {
	resp, err := http.Get(url)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("download failed: %s", resp.Status)
	}

	out, err := os.Create(path)
	if err != nil {
		return err
	}
	defer out.Close()

	_, err = io.Copy(out, resp.Body)
	return err
}

func extractTarball(tarballPath, targetDir string) error {
	file, err := os.Open(tarballPath)
	if err != nil {
		return err
	}
	defer file.Close()

	gzReader, err := gzip.NewReader(file)
	if err != nil {
		return err
	}
	defer gzReader.Close()

	tarReader := tar.NewReader(gzReader)

	if err := os.MkdirAll(targetDir, 0755); err != nil {
		return err
	}

	for {
		header, err := tarReader.Next()
		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}

		target := filepath.Join(targetDir, header.Name)

		switch header.Typeflag {
		case tar.TypeDir:
			if err := os.MkdirAll(target, 0755); err != nil {
				return err
			}
		case tar.TypeReg:
			if err := os.MkdirAll(filepath.Dir(target), 0755); err != nil {
				return err
			}

			outFile, err := os.Create(target)
			if err != nil {
				return err
			}

			if _, err := io.Copy(outFile, tarReader); err != nil {
				outFile.Close()
				return err
			}
			outFile.Close()
		}
	}

	return nil
}
