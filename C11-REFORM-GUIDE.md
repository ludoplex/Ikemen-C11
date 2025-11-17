# C11 Reform Guide for Ikemen Engine

## Purpose

This document establishes the guiding principles and rules for the C11 refactor of the Ikemen engine. The C11 implementation is being developed alongside the existing Go implementation to provide an alternative, high-performance engine core while maintaining full compatibility with MUGEN resources.

## Core Principles

### 1. Non-Breaking Coexistence

**The C11 refactor must not break the Go implementation.**

- All C11 code lives exclusively under the `c11/` directory
- No modifications to existing Go source files (`src/*.go`) except where absolutely necessary for additive CI integration
- No removal or reorganization of Go build pipelines
- No changes to existing assets (`data/`, `external/`, `font/`, etc.)
- Both implementations can be built and run independently

### 2. Go Engine as Canonical Reference

**The Go implementation is the authoritative reference for behavior.**

- When specifications are ambiguous, follow Go engine's interpretation
- C11 behavior should match Go behavior for equivalent inputs
- Compatibility tests should validate C11 against Go processing of same assets
- Bug fixes discovered during C11 development should benefit both implementations

### 3. Asset Compatibility

**Both engines must work with identical MUGEN resources.**

- Maintain compatibility with existing MUGEN stages, characters, and screenpacks
- Use the same asset directory structure and search paths
- Parse the same file formats (SFF, AIR, CNS, CMD, DEF, etc.)
- Do not require asset modifications to work with C11
- Both engines should be interchangeable from the user's perspective

### 4. Real, Working Code

**All committed C11 code must be functional, not placeholder.**

- No example-only files or stub implementations
- All code must compile with strict warnings enabled
- All features must include working tests
- CI must validate C11 builds and tests on every commit
- No conversational documentation; only technical documentation and code comments

## Directory Structure

```
Ikemen-C11/
├── c11/                    # All C11 implementation files
│   ├── CMakeLists.txt      # C11 build configuration
│   ├── include/            # Public C11 API headers
│   │   └── ikemen_engine.h
│   ├── src/                # C11 source implementation
│   │   ├── engine.c
│   │   └── main.c
│   ├── tests/              # C11 test programs
│   │   ├── test_version.c
│   │   └── test_asset_compat.c
│   └── docs/               # C11-specific documentation
│       └── ROADMAP.md
├── src/                    # Go implementation (DO NOT MODIFY)
├── data/                   # Shared assets (DO NOT MODIFY)
├── external/               # Shared assets (DO NOT MODIFY)
├── font/                   # Shared assets (DO NOT MODIFY)
├── .github/workflows/      # CI configurations
│   └── c11-ci.yml          # C11 build/test workflow (NEW)
└── C11-REFORM-GUIDE.md     # This file
```

## Development Rules

### For C11 Development

1. **All new C11 code goes under `c11/`**
   - Never create C files outside this directory for the refactor
   - Organize code logically: headers in `include/`, sources in `src/`, tests in `tests/`

2. **Enforce C11 standard compliance**
   - Use `-std=c11` with no compiler extensions
   - Enable strict warnings: `-Wall -Wextra -Wpedantic -Werror` (or `/W4 /WX` on MSVC)
   - Use only C11 standard library and POSIX APIs
   - Avoid platform-specific code where possible; isolate when necessary

3. **Write real tests**
   - Every module needs unit tests
   - Integration tests for component interactions
   - Compatibility tests comparing C11 vs. Go behavior on same assets
   - Wire all tests into CTest

4. **Document in code**
   - Public APIs fully documented in headers
   - Implementation comments explain design decisions
   - Reference Go source files when implementing equivalent functionality
   - Note MUGEN compatibility considerations

5. **Build system**
   - Use CMake for cross-platform builds
   - Support out-of-tree builds (`c11/build/`)
   - Define clear targets: library, executable, tests
   - CI must build on Linux, Windows, macOS

### For Go Development (Existing Engine)

1. **Continue normal development**
   - The C11 refactor does not block Go engine improvements
   - Maintain backward compatibility with MUGEN resources
   - No need to coordinate every change with C11 team

2. **When touching shared assets**
   - Ensure changes don't break C11's assumptions
   - If asset structure changes, update C11 compatibility checks
   - Communicate major asset-related changes

3. **CI coordination**
   - New workflows should be additive (e.g., `c11-ci.yml`)
   - Do not modify existing Go CI unless integrating C11 compatibility checks
   - Both engines' CI must pass

## Asset Path Resolution

Both engines follow the same search order for resources:

1. **Explicit paths**: If absolute or explicitly relative, use as-is
2. **Relative to definition file**: Check relative to the .def file's directory
3. **Motif directory**: Screenpack-specific resources
4. **`data/` directory**: Common game data
5. **`external/` directory**: External resources (scripts, shaders, etc.)
6. **`font/` directory**: Font files
7. **`video/` directory**: Video files

The C11 implementation must replicate this search order to maintain compatibility.

## Testing Strategy

### Unit Tests
- Test individual C11 modules in isolation
- Mock dependencies when needed
- Fast, repeatable, no external dependencies

### Integration Tests
- Test interactions between C11 components
- May use temporary test assets
- Validate against known-good results

### Compatibility Tests
- Run both engines on the same MUGEN assets
- Compare outputs (parsed data, rendered frames, etc.)
- Identify and fix behavioral differences
- These tests are critical for validating the refactor

### CI/CD Requirements
- C11 must build successfully on all target platforms
- All CTest tests must pass
- Asset compatibility checks must pass
- Optional: cross-engine comparison tests

## Communication and Coordination

### Issues and Pull Requests
- Tag C11-related issues with `c11-refactor` label
- PR descriptions should reference affected components
- Note any changes that might impact Go engine

### Documentation
- Keep `c11/docs/ROADMAP.md` updated with progress
- Document architectural decisions in `c11/docs/`
- Update this guide if principles change

### Community
- Encourage discussion of C11 vs. Go trade-offs
- Share insights from both implementations
- Collaborate on bug fixes and optimizations

## Performance and Optimization

### Goals
- Match or exceed Go engine performance
- Efficient memory usage
- Fast asset loading
- Smooth frame rates (60+ FPS)

### Benchmarking
- Create benchmark suite comparing C11 vs. Go
- Measure: load times, frame times, memory usage
- Identify optimization opportunities
- Document performance characteristics

### Optimization Guidelines
- Profile before optimizing
- Prefer clarity over premature optimization
- Use benchmarks to validate improvements
- Document performance-critical sections

## Compatibility Matrix

### MUGEN Feature Support
Track which MUGEN features are supported:

| Feature | Go Engine | C11 Engine | Notes |
|---------|-----------|------------|-------|
| SFF v1.0 | ✓ | Planned | Sprite files |
| SFF v2.0 | ✓ | Planned | Sprite files |
| AIR | ✓ | Planned | Animation |
| CNS | ✓ | Planned | Character states |
| CMD | ✓ | Planned | Commands |
| DEF | ✓ | Planned | Definition files |
| Stages | ✓ | Planned | Stage backgrounds |
| Sound | ✓ | Planned | Audio playback |
| Network | ✓ | Planned | Online play |

### Platform Support

| Platform | Go Engine | C11 Engine | CI |
|----------|-----------|------------|-----|
| Linux x86-64 | ✓ | ✓ | ✓ |
| Linux ARM64 | ✓ | Planned | Planned |
| Windows x64 | ✓ | Planned | Planned |
| macOS ARM | ✓ | Planned | Planned |
| macOS Intel | ✓ | Planned | Planned |

## Contribution Guidelines

### Getting Started with C11
1. Read this guide and `c11/docs/ROADMAP.md`
2. Build the C11 engine: `cd c11/build && cmake .. && make`
3. Run tests: `ctest`
4. Choose a component from the roadmap
5. Implement, test, document, submit PR

### Code Review Requirements
- Code must compile with zero warnings
- All tests must pass
- Documentation must be updated
- CI must pass on all platforms
- Code review approval required

### Quality Standards
- Follow existing code style
- Use clear, descriptive names
- Add comments for complex logic
- Write self-documenting code where possible
- Prefer standard library over custom implementations

## Maintenance

### Keeping C11 in Sync with Go
- Periodically review Go engine changes
- Adapt C11 to match Go behavior updates
- Share bug fixes between implementations
- Coordinate major architectural changes

### Version Numbering
- C11 engine has independent version numbers
- Major version increment: Breaking API changes
- Minor version increment: New features
- Patch version increment: Bug fixes

### Deprecation Policy
- Announce deprecations in advance
- Provide migration path
- Maintain backward compatibility when possible
- Document deprecated features

## Frequently Asked Questions

### Why C11 instead of C++, Rust, etc.?
C11 provides a stable, widely-supported standard with excellent tooling and minimal runtime overhead. It's a pragmatic choice for a performance-critical game engine that must run on diverse platforms.

### Will the Go engine be deprecated?
No. The Go engine remains the primary, reference implementation. C11 is an alternative for users who prefer a pure C implementation or need specific performance characteristics.

### Can I use Go and C11 engines interchangeably?
Yes, that's the goal. Both should work with the same MUGEN assets and behave identically from a user perspective.

### How can I help?
Check `c11/docs/ROADMAP.md` for planned work. Pick a component, implement it, test it, and submit a PR. Join the discussion on GitHub issues.

### What if C11 and Go engines diverge?
We should fix that. Report behavioral differences as bugs. The goal is parity.

## References

- **MUGEN Specification**: Community-maintained documentation of MUGEN file formats and behavior
- **Go Engine Source**: `src/*.go` - The authoritative reference implementation
- **C11 Roadmap**: `c11/docs/ROADMAP.md` - Detailed refactor plan
- **Building Guide**: `BUILDING.md` - Build instructions for both engines

## Conclusion

The C11 refactor is a collaborative, incremental effort to provide an alternative implementation of Ikemen while preserving full compatibility with MUGEN resources and the Go reference engine. By following these principles and rules, we ensure both engines coexist harmoniously and benefit from each other's development.

All contributors to the C11 refactor should familiarize themselves with this guide and the detailed roadmap. Questions, suggestions, and improvements to this guide are welcome via GitHub issues and pull requests.

---

**Version**: 1.0  
**Last Updated**: 2025-11-17  
**Maintainers**: Ikemen community
