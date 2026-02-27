# Governance

This document outlines the governance structure for the Droy Language project.

## 📋 Overview

Droy Language is an open-source project that welcomes contributions from everyone. This governance model ensures the project remains healthy, sustainable, and community-driven.

## 🏛️ Project Roles

### 1. Project Lead (BDFL)

**Current**: [@droy-go](https://github.com/droy-go)

**Responsibilities**:
- Set project direction and vision
- Make final decisions on contentious issues
- Approve major architectural changes
- Manage releases
- Ensure project sustainability

**Authority**:
- Can override any decision in exceptional circumstances
- Has veto power on significant changes
- Appoints and removes maintainers

### 2. Maintainers

**Current**: [@droy-go](https://github.com/droy-go)

**Responsibilities**:
- Review and merge pull requests
- Triage issues
- Enforce code of conduct
- Guide contributors
- Maintain documentation

**Requirements**:
- Significant contributions to the project
- Understanding of the codebase
- Good communication skills
- Commitment to the project's values

### 3. Contributors

Anyone who contributes to the project, including:
- Code contributions
- Documentation improvements
- Bug reports
- Feature suggestions
- Community support

## 📊 Decision Making

### Consensus-Based

Most decisions are made through consensus:
1. Discussion in issues/PRs
2. Seeking input from community
3. Addressing concerns
4. Reaching agreement

### Lazy Consensus

For minor changes:
1. Propose the change
2. Wait for objections (typically 3-7 days)
3. If no objections, proceed

### Maintainer Decision

For significant changes:
1. Discussion and consensus-building
2. Maintainer review
3. Final approval by at least one maintainer

### Project Lead Decision

For major decisions:
- Project direction changes
- Breaking changes
- License changes
- Major architectural decisions

## 🔄 Contribution Process

### 1. Proposal

- Open an issue to discuss the change
- For significant changes, use the feature request template

### 2. Discussion

- Community discussion
- Feedback collection
- Refinement of proposal

### 3. Implementation

- Fork and branch
- Implement changes
- Add tests
- Update documentation

### 4. Review

- Open pull request
- Code review by maintainers
- Address feedback
- CI checks must pass

### 5. Merge

- Approved by at least one maintainer
- All checks pass
- No outstanding concerns

## 📅 Release Process

### Version Numbering

We follow [Semantic Versioning](https://semver.org/):
- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes (backward compatible)

### Release Cycle

- **Patch releases**: As needed for bug fixes
- **Minor releases**: Every 1-2 months
- **Major releases**: As needed, with advance notice

### Release Steps

1. Update CHANGELOG.md
2. Update version in CMakeLists.txt
3. Create release branch
4. Run full test suite
5. Create GitHub release
6. Tag the release
7. Announce to community

## 🗳️ Voting

### When to Vote

Voting is used for:
- Adding/removing maintainers
- Significant policy changes
- Disputed decisions

### Voting Process

1. Proposal posted with clear options
2. Discussion period (7 days)
3. Voting period (7 days)
4. Simple majority wins
5. Project Lead has tie-breaking vote

### Who Can Vote

- Maintainers: Full vote
- Contributors with 5+ merged PRs: Advisory vote

## 🚪 Joining the Team

### Becoming a Contributor

1. Start contributing (code, docs, issues)
2. Follow the code of conduct
3. Help others in the community

### Becoming a Maintainer

Requirements:
- 10+ merged PRs
- 3+ months of active participation
- Good understanding of the codebase
- Nominated by existing maintainer
- Approved by Project Lead

### Maintainer Responsibilities

- Review PRs within 7 days
- Respond to issues within 7 days
- Participate in discussions
- Uphold code of conduct
- Attend meetings (if scheduled)

## 📜 Code of Conduct

All participants must follow our [Code of Conduct](CODE_OF_CONDUCT.md).

Violations may result in:
1. Warning
2. Temporary ban
3. Permanent ban

## 🔄 Changes to Governance

Changes to this document require:
1. Proposal issue
2. Discussion period (14 days)
3. Approval by Project Lead
4. Community notification

## 📞 Contact

For governance-related questions:
- Open a [discussion](https://github.com/droy-go/droy-lang/discussions)
- Email: [droy.go@example.com](mailto:droy.go@example.com)

## 📚 Related Documents

- [Code of Conduct](CODE_OF_CONDUCT.md)
- [Contributing Guide](CONTRIBUTING.md)
- [Security Policy](SECURITY.md)

---

**Last Updated**: February 2026

This governance document is a living document and may be updated as the project evolves.
