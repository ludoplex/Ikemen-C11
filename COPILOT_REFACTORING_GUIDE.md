# Ikemen C11 Refactoring Policy Guide

**For Contributors, GitHub Copilot, and AI Assistants**

## Purpose

This document establishes clear policies for the gradual refactoring of the Ikemen GO engine from Go to C11. The goal is to enable a safe, incremental migration without disrupting the existing, working Go implementation.

## Core Principles

### 1. Preserve the Go Implementation

**Rule**: The existing Go source code in the `src/` directory and related Go files (`go.mod`, `go.sum`, `Makefile`, etc.) **MUST NOT** be modified, deleted, or moved as part of the C11 refactoring effort.

**Rationale**: The Go implementation is production-ready and actively used. It must remain functional throughout the refactoring process.

**Exceptions**: 
- Explicit, documented migration shims may be added later if approved by maintainers.
- Bug fixes unrelated to the C11 refactor may proceed normally in the Go codebase.
- Normal Go feature development continues independently.

### 2. All C11 Work Goes in `c11/` Directory

**Rule**: All C11 refactoring work, including source files, headers, tests, build scripts, and documentation, belongs exclusively under the `c11/` directory.

**What goes in `c11/`:**
- C11 source files (`.c`)
- C11 headers (`.h`)
- CMake build configuration
- C11 unit tests
- C11-specific documentation
- Helper scripts for C11 builds

**What does NOT go in `c11/`:**
- Go source code
- Modifications to existing engine data formats (preserve compatibility)
- Changes to repository-wide documentation (unless explicitly documenting C11 work)

### 3. Incremental Implementation

**Rule**: The C11 refactor proceeds module by module, feature by feature, with working builds at every step.

**Guidelines**:
- Each PR should add or complete a discrete, testable module
- New modules should include unit tests
- Maintain compilability at all times
- Document incomplete or stub functionality clearly with `TODO` comments

### 4. Compatibility and Coexistence

**Rule**: Both Go and C11 implementations must coexist peacefully in the same repository.

**Requirements**:
- The C11 build system must not interfere with Go builds
- CI workflows for C11 must run independently of Go CI
- The C11 executable must have a distinct name (e.g., `ikemen_c11`)
- Shared assets (data, fonts, etc.) should remain compatible with both implementations

## For GitHub Copilot and AI Assistants

When working on Ikemen-C11 repository tasks:

### ✅ DO:
- Add new C11 source files under `c11/src/`
- Add new C11 headers under `c11/include/`
- Create or modify CMake files within `c11/`
- Write tests in `c11/tests/`
- Update `c11/README.md` or create new docs in `c11/`
- Add or modify CI workflows in `.github/workflows/` specifically for C11 builds
- Reference the Go implementation for understanding engine behavior, but implement in C11

### ❌ DO NOT:
- Modify, delete, or move Go source files (`.go` files in `src/`)
- Change `go.mod`, `go.sum`, or the existing `Makefile` targeting Go builds
- Remove or alter existing Go CI workflows
- Suggest "replacing" Go code with C11 code
- Make destructive changes to working Go functionality
- Move shared assets or data files without careful consideration

### When Uncertain:
- Prefer adding new files in `c11/` over modifying existing files
- Ask for clarification if a task seems to require Go code changes
- Default to conservative, non-destructive changes
- Document any assumptions or limitations

## For Human Contributors

### Pull Request Guidelines

When submitting C11-related PRs:

1. **Title Format**: Use conventional commits format
   - Examples: `feat(c11): add graphics subsystem`, `refactor(c11): improve memory management`

2. **Description**: Clearly state:
   - Which module(s) are being added or modified
   - Current implementation status (complete, partial, stub)
   - Test coverage
   - Any known limitations or TODOs

3. **Checklist**:
   - [ ] All new C11 code is under `c11/`
   - [ ] No Go source files modified (unless approved)
   - [ ] Code compiles on Linux (at minimum)
   - [ ] Unit tests included and passing
   - [ ] Documentation updated (at least `c11/README.md`)
   - [ ] CI workflows pass

### Code Review Focus

Reviewers should check:
- No inadvertent Go code modifications
- Proper directory structure adherence
- Code quality and C11 standard compliance
- Test coverage
- Documentation completeness

## CI/CD Integration

### C11 Build Workflows

- C11 has dedicated GitHub Actions workflows (e.g., `.github/workflows/c11-build.yml`)
- These workflows build and test only the C11 code
- They run independently of Go build workflows
- Matrix includes at least Linux; Windows/macOS support to be added

### Go Build Workflows

- Existing Go workflows remain unchanged
- They continue to build and test the Go implementation
- C11 presence should not affect Go builds

## Migration Timeline

This is a long-term project without a fixed completion date. The priority is correctness and maintainability over speed.

**Phases** (tentative):
1. **Phase 1** (Current): Scaffolding and core infrastructure
2. **Phase 2**: Graphics and rendering
3. **Phase 3**: Audio and input
4. **Phase 4**: Character and stage systems
5. **Phase 5**: Fight logic and gameplay
6. **Phase 6**: Advanced features and optimization

Each phase may take months. Progress updates will be communicated via discussions and documentation.

## Questions and Discussions

For questions about the refactoring process:
- Open a discussion in the GitHub Discussions section
- Tag with `c11-refactor` label
- Check existing discussions first

For bugs specific to C11 implementation:
- Open an issue with `c11` label
- Provide build environment details
- Include minimal reproduction steps

## Summary

**Remember**: This is a **gradual, additive** refactoring. The Go implementation stays intact. All C11 work goes in `c11/`. When in doubt, ask or default to non-destructive changes.

---

*Last Updated: 2024-11-17*
