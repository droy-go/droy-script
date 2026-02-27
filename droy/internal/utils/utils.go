package utils

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

// FileExists checks if a file exists
func FileExists(path string) bool {
	_, err := os.Stat(path)
	return !os.IsNotExist(err)
}

// DirExists checks if a directory exists
func DirExists(path string) bool {
	info, err := os.Stat(path)
	if os.IsNotExist(err) {
		return false
	}
	return info.IsDir()
}

// EnsureDir ensures a directory exists
func EnsureDir(path string) error {
	return os.MkdirAll(path, 0755)
}

// CopyFile copies a file from src to dst
func CopyFile(src, dst string) error {
	sourceFile, err := os.Open(src)
	if err != nil {
		return err
	}
	defer sourceFile.Close()

	destFile, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer destFile.Close()

	_, err = io.Copy(destFile, sourceFile)
	return err
}

// CopyDir copies a directory recursively
func CopyDir(src, dst string) error {
	return filepath.Walk(src, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		relPath, err := filepath.Rel(src, path)
		if err != nil {
			return err
		}

		dstPath := filepath.Join(dst, relPath)

		if info.IsDir() {
			return os.MkdirAll(dstPath, info.Mode())
		}

		return CopyFile(path, dstPath)
	})
}

// RemoveDir removes a directory and all its contents
func RemoveDir(path string) error {
	return os.RemoveAll(path)
}

// CalculateSHA256 calculates the SHA256 hash of a file
func CalculateSHA256(path string) (string, error) {
	file, err := os.Open(path)
	if err != nil {
		return "", err
	}
	defer file.Close()

	hash := sha256.New()
	if _, err := io.Copy(hash, file); err != nil {
		return "", err
	}

	return hex.EncodeToString(hash.Sum(nil)), nil
}

// SanitizePackageName sanitizes a package name
func SanitizePackageName(name string) string {
	// Replace invalid characters
	name = strings.ReplaceAll(name, " ", "-")
	name = strings.ReplaceAll(name, "_", "-")
	name = strings.ToLower(name)
	
	// Remove any non-alphanumeric characters except hyphens
	var result strings.Builder
	for _, r := range name {
		if (r >= 'a' && r <= 'z') || (r >= '0' && r <= '9') || r == '-' || r == '/' || r == '.' || r == '@' {
			result.WriteRune(r)
		}
	}
	
	return result.String()
}

// ParseGitHubURL parses a GitHub URL
func ParseGitHubURL(url string) (owner, repo string, err error) {
	// Handle different GitHub URL formats
	url = strings.TrimPrefix(url, "https://github.com/")
	url = strings.TrimPrefix(url, "http://github.com/")
	url = strings.TrimPrefix(url, "github.com/")
	url = strings.TrimSuffix(url, ".git")

	parts := strings.Split(url, "/")
	if len(parts) < 2 {
		return "", "", fmt.Errorf("invalid GitHub URL")
	}

	return parts[0], parts[1], nil
}

// IsGitHubURL checks if a string is a GitHub URL
func IsGitHubURL(url string) bool {
	return strings.Contains(url, "github.com")
}

// GetCacheDir returns the cache directory
func GetCacheDir() string {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return ".cache"
	}
	return filepath.Join(homeDir, ".droy", "cache")
}

// GetGlobalDir returns the global packages directory
func GetGlobalDir() string {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return ".global"
	}
	return filepath.Join(homeDir, ".droy", "global")
}

// VersionCompare compares two version strings
// Returns -1 if v1 < v2, 0 if v1 == v2, 1 if v1 > v2
func VersionCompare(v1, v2 string) int {
	// Simple version comparison
	// In a real implementation, this would handle semver properly
	if v1 == v2 {
		return 0
	}
	
	// For now, just do string comparison
	if v1 < v2 {
		return -1
	}
	return 1
}
