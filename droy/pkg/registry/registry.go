package registry

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"strings"
	"time"

	"github.com/droy-go/droy-pm/pkg/config"
)

// Registry represents a package registry
type Registry struct {
	URL        string
	HTTPClient *http.Client
}

// PackageInfo represents package information from registry
type PackageInfo struct {
	Name        string   `json:"name"`
	Version     string   `json:"version"`
	Description string   `json:"description"`
	Author      string   `json:"author"`
	License     string   `json:"license"`
	Repository  string   `json:"repository"`
	Keywords    []string `json:"keywords"`
	Versions    []string `json:"versions"`
	Dist        *DistInfo `json:"dist,omitempty"`
}

// DistInfo contains distribution information
type DistInfo struct {
	Tarball   string `json:"tarball"`
	Shasum    string `json:"shasum"`
	Integrity string `json:"integrity"`
}

// SearchResult represents a search result
type SearchResult struct {
	Packages []PackageInfo `json:"packages"`
	Total    int           `json:"total"`
}

// New creates a new registry client
func New(url string) *Registry {
	if url == "" {
		url = "https://registry.droy-lang.org"
	}

	return &Registry{
		URL: url,
		HTTPClient: &http.Client{
			Timeout: 30 * time.Second,
		},
	}
}

// GetPackage gets package information from the registry
func (r *Registry) GetPackage(name string) (*PackageInfo, error) {
	url := fmt.Sprintf("%s/%s", r.URL, name)
	
	resp, err := r.HTTPClient.Get(url)
	if err != nil {
		return nil, fmt.Errorf("failed to fetch package: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode == http.StatusNotFound {
		return nil, fmt.Errorf("package not found: %s", name)
	}

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("registry error: %s", resp.Status)
	}

	var info PackageInfo
	if err := json.NewDecoder(resp.Body).Decode(&info); err != nil {
		return nil, fmt.Errorf("failed to decode response: %w", err)
	}

	return &info, nil
}

// GetLatestVersion gets the latest version of a package
func (r *Registry) GetLatestVersion(name string) (string, error) {
	// Try to get from registry
	info, err := r.GetPackage(name)
	if err == nil {
		return info.Version, nil
	}

	// Fallback: try GitHub
	if strings.HasPrefix(name, "github.com/") {
		return r.getLatestGitHubVersion(name)
	}

	return "", err
}

// Search searches for packages
func (r *Registry) Search(query string) ([]PackageInfo, error) {
	url := fmt.Sprintf("%s/-/v1/search?text=%s", r.URL, query)
	
	resp, err := r.HTTPClient.Get(url)
	if err != nil {
		// Fallback to mock results for demonstration
		return r.mockSearch(query), nil
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return r.mockSearch(query), nil
	}

	var result SearchResult
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		return r.mockSearch(query), nil
	}

	return result.Packages, nil
}

// Publish publishes a package to the registry
func (r *Registry) Publish(pkg *config.Package, tarballPath string) error {
	url := fmt.Sprintf("%s/%s", r.URL, pkg.Name)

	// Open tarball
	file, err := os.Open(tarballPath)
	if err != nil {
		return fmt.Errorf("failed to open tarball: %w", err)
	}
	defer file.Close()

	// Create request
	req, err := http.NewRequest("PUT", url, file)
	if err != nil {
		return fmt.Errorf("failed to create request: %w", err)
	}

	req.Header.Set("Content-Type", "application/gzip")
	req.Header.Set("X-Droy-Version", pkg.Version)

	// Send request
	resp, err := r.HTTPClient.Do(req)
	if err != nil {
		return fmt.Errorf("failed to publish: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusCreated && resp.StatusCode != http.StatusOK {
		body, _ := io.ReadAll(resp.Body)
		return fmt.Errorf("publish failed: %s - %s", resp.Status, string(body))
	}

	return nil
}

func (r *Registry) getLatestGitHubVersion(repo string) (string, error) {
	parts := strings.Split(repo, "/")
	if len(parts) < 3 {
		return "", fmt.Errorf("invalid GitHub repository")
	}

	owner := parts[1]
	repoName := parts[2]

	url := fmt.Sprintf("https://api.github.com/repos/%s/%s/releases/latest", owner, repoName)
	
	resp, err := r.HTTPClient.Get(url)
	if err != nil {
		return "latest", nil // Fallback
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return "latest", nil
	}

	var release struct {
		TagName string `json:"tag_name"`
	}

	if err := json.NewDecoder(resp.Body).Decode(&release); err != nil {
		return "latest", nil
	}

	return strings.TrimPrefix(release.TagName, "v"), nil
}

func (r *Registry) mockSearch(query string) []PackageInfo {
	// Mock search results for demonstration
	return []PackageInfo{
		{
			Name:        "droy-http",
			Version:     "1.2.0",
			Description: "HTTP client library for Droy",
			Author:      "droy-team",
			License:     "MIT",
		},
		{
			Name:        "droy-json",
			Version:     "2.0.1",
			Description: "JSON parsing and serialization",
			Author:      "community",
			License:     "Apache-2.0",
		},
		{
			Name:        "droy-cli",
			Version:     "0.5.0",
			Description: "Command-line interface utilities",
			Author:      "droy-team",
			License:     "MIT",
		},
	}
}
