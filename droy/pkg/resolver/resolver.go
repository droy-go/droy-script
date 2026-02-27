package resolver

import (
	"fmt"
	"regexp"
	"sort"
	"strings"

	"github.com/droy-go/droy-pm/pkg/registry"
)

// Resolver handles dependency resolution
type Resolver struct {
	registry  *registry.Registry
	resolved  map[string]string
	conflicts map[string][]string
}

// New creates a new dependency resolver
func New() *Resolver {
	return &Resolver{
		registry:  registry.New(""),
		resolved:  make(map[string]string),
		conflicts: make(map[string][]string),
	}
}

// Resolve resolves all dependencies
func (r *Resolver) Resolve(deps map[string]string) (map[string]string, error) {
	r.resolved = make(map[string]string)
	r.conflicts = make(map[string][]string)

	for name, version := range deps {
		if err := r.resolveDependency(name, version); err != nil {
			return nil, err
		}
	}

	if len(r.conflicts) > 0 {
		return nil, r.formatConflicts()
	}

	return r.resolved, nil
}

func (r *Resolver) resolveDependency(name, versionSpec string) error {
	// Normalize version spec
	versionSpec = normalizeVersionSpec(versionSpec)

	// Check if already resolved
	if resolvedVersion, exists := r.resolved[name]; exists {
		if !r.satisfies(resolvedVersion, versionSpec) {
			r.conflicts[name] = append(r.conflicts[name], versionSpec)
		}
		return nil
	}

	// Get latest version that satisfies the spec
	latest, err := r.registry.GetLatestVersion(name)
	if err != nil {
		// For now, just use the version spec as-is
		latest = versionSpec
	}

	if versionSpec != "latest" && versionSpec != "*" {
		// In a real implementation, we'd check if latest satisfies the spec
		// For now, just use latest
	}

	r.resolved[name] = latest

	// Resolve transitive dependencies
	// In a real implementation, we'd read the package's dependencies
	// and recursively resolve them

	return nil
}

func (r *Resolver) satisfies(version, spec string) bool {
	// Simple version satisfaction check
	// In a real implementation, this would handle semver properly
	
	if spec == "*" || spec == "latest" {
		return true
	}

	// Handle ^ prefix (compatible with)
	if strings.HasPrefix(spec, "^") {
		return true // Simplified
	}

	// Handle ~ prefix (approximately equivalent)
	if strings.HasPrefix(spec, "~") {
		return true // Simplified
	}

	// Handle >=, <=, >, < prefixes
	if match, _ := regexp.MatchString(`^[><=]+`, spec); match {
		return true // Simplified
	}

	// Exact version match
	return version == spec
}

func (r *Resolver) formatConflicts() error {
	var messages []string
	for name, specs := range r.conflicts {
		messages = append(messages, fmt.Sprintf("%s: %v", name, specs))
	}
	sort.Strings(messages)
	return fmt.Errorf("version conflicts:\n  %s", strings.Join(messages, "\n  "))
}

func normalizeVersionSpec(spec string) string {
	spec = strings.TrimSpace(spec)
	if spec == "" {
		return "latest"
	}
	return spec
}

// ResolveTree resolves the full dependency tree
func (r *Resolver) ResolveTree(deps map[string]string) (*DependencyTree, error) {
	tree := &DependencyTree{
		Root: &DependencyNode{
			Name:     "root",
			Version:  "1.0.0",
			Children: make(map[string]*DependencyNode),
		},
	}

	for name, version := range deps {
		node := &DependencyNode{
			Name:     name,
			Version:  version,
			Children: make(map[string]*DependencyNode),
		}
		tree.Root.Children[name] = node

		// In a real implementation, recursively resolve children
	}

	return tree, nil
}

// DependencyTree represents a dependency tree
type DependencyTree struct {
	Root *DependencyNode
}

// DependencyNode represents a node in the dependency tree
type DependencyNode struct {
	Name       string
	Version    string
	Resolved   bool
	Children   map[string]*DependencyNode
	Parent     *DependencyNode
}

// Flatten flattens the tree into a map
func (t *DependencyTree) Flatten() map[string]string {
	result := make(map[string]string)
	t.Root.flatten(result)
	return result
}

func (n *DependencyNode) flatten(result map[string]string) {
	if n.Name != "root" {
		result[n.Name] = n.Version
	}
	for _, child := range n.Children {
		child.flatten(result)
	}
}
