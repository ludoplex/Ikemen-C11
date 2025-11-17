# Ikemen C11 Refactor Roadmap

## Overview

This document outlines the incremental plan for porting the Ikemen GO engine functionality to C11. The C11 implementation is designed to coexist with the existing Go implementation while maintaining full compatibility with MUGEN assets and resources.

## Design Principles

### 1. Asset Compatibility
The C11 engine must use the same asset directory structure as the Go reference implementation:
- `data/` - Common data files (common.cmd, common.const, system.base.def, etc.)
- `external/` - External resources (icons, mods, script, shaders)
- `font/` - Font files
- Screenpacks and character/stage definitions follow MUGEN conventions

### 2. Non-Breaking Coexistence
- All C11 code lives under `c11/` directory
- No modifications to existing Go sources or Go build pipelines
- C11 and Go implementations can be tested side-by-side for behavioral equivalence
- Shared assets remain in their current locations

### 3. Incremental Development
The refactor proceeds in phases, with each phase building working, tested functionality:

## Phase 1: Foundation (Current - v0.1.0)
**Status: Complete**

- [x] Basic directory structure (`c11/src`, `c11/include`, `c11/tests`, `c11/docs`)
- [x] Public API header (`ikemen_engine.h`)
- [x] Version query functions
- [x] Asset compatibility check (validates directory structure)
- [x] CMake build system with C11 enforcement
- [x] Unit and integration tests
- [x] CI/CD integration via GitHub Actions

### Key Deliverables
- Compiling C11 code with strict warnings
- Working executable that checks asset compatibility
- Automated testing via CTest
- Documentation and development guidelines

## Phase 2: Configuration and Resource Loading (Planned - v0.2.0)

### Goals
Implement configuration file parsing and basic resource loading compatible with MUGEN formats.

### Components
- **Configuration Parser**
  - Parse system.base.def format (INI-style with MUGEN extensions)
  - Handle [Info], [Files], [Music], etc. sections
  - Support common.const parsing
  - Validate against Go implementation's config behavior

- **File I/O Utilities**
  - Safe file reading with error handling
  - Path resolution matching Go implementation's search order:
    - Explicit paths
    - Relative to definition file
    - Motif directory fallback
    - `data/` directory fallback
  - Cross-platform path handling (Windows/Linux/macOS)

- **Testing**
  - Parse example system.base.def files
  - Compare parsed results against Go engine's interpretation
  - Validate error handling for malformed configs

### Asset Compatibility Checks
- Verify presence of key files (common.cmd, system.base.def)
- Check file format validity
- Report detailed compatibility issues

## Phase 3: Animation and Sprite System (Planned - v0.3.0)

### Goals
Implement MUGEN-compatible sprite (SFF) and animation (AIR) file support.

### Components
- **SFF Reader**
  - Parse SFF v1.0 and v2.0 formats
  - Extract sprite data and palettes
  - Memory-efficient sprite caching
  - Match Go implementation's SFF handling

- **AIR Parser**
  - Parse animation definition files
  - Build animation action database
  - Handle clsn (collision) boxes
  - Support element properties (flip, scale, blend modes)

- **Image Abstraction**
  - Define sprite data structures
  - Prepare for future rendering backend integration
  - Support for palette manipulation

- **Testing**
  - Load actual character SFF/AIR files from repository's test data
  - Validate sprite extraction against Go engine results
  - Test animation state machine construction

## Phase 4: Character State System (Planned - v0.4.0)

### Goals
Implement the character state machine and command processing (CNS/CMD).

### Components
- **CNS Parser**
  - State definition parsing
  - State controller parsing (VelSet, PosSet, ChangeState, etc.)
  - Expression compiler for trigger conditions
  - Match MUGEN state semantics

- **CMD Parser**
  - Command definition parsing
  - Input sequence matching
  - Command buffering

- **State Machine Engine**
  - State transitions
  - State controller execution
  - Trigger evaluation
  - Variable management (var, fvar)

- **Testing**
  - Load and parse character CNS files
  - Validate state machine construction
  - Test trigger evaluation against known cases
  - Compare with Go engine behavior

## Phase 5: Basic Rendering (Planned - v0.5.0)

### Goals
Implement minimal rendering to display sprites and animations.

### Components
- **Rendering Abstraction**
  - Define rendering interface (portable across OpenGL/Vulkan/etc.)
  - Sprite rendering primitives
  - Palette application
  - Alpha blending

- **Camera System**
  - Viewport management
  - Screen space to world space conversion
  - Camera following logic

- **Stage Background System**
  - DEF file parsing for stages
  - Background layer rendering
  - Parallax scrolling

- **Testing**
  - Render test sprites
  - Display animation sequences
  - Stage background visualization
  - Screenshot comparison with Go engine

## Phase 6: Physics and Collision (Planned - v0.6.0)

### Goals
Implement character physics, movement, and hit detection.

### Components
- **Physics System**
  - Position, velocity, acceleration
  - Gravity and friction
  - Ground collision
  - Platform physics (if applicable)

- **Collision Detection**
  - CLSN box evaluation
  - Hit/hurt box interactions
  - Push box collision
  - Projectile collision

- **Testing**
  - Unit tests for physics calculations
  - Collision detection accuracy tests
  - Compare with Go engine physics behavior

## Phase 7: Full Game Logic (Planned - v0.7.0+)

### Goals
Complete game functionality parity with Go engine.

### Components
- Sound system (module music, sound effects)
- Input handling and replay
- Network play and rollback
- UI and menu systems
- Training mode
- Tag team mechanics
- Full MUGEN feature compatibility

## Development Workflow

### Asset Path Resolution
The C11 engine follows the Go engine's search order for resources:

1. Check explicit absolute/relative paths first
2. Check relative to the definition file's directory
3. Fall back to motif directory (screenpack)
4. Fall back to `data/` directory
5. Fall back to `video/` for video files

### Testing Strategy
Each phase includes:
- **Unit Tests**: Test individual components in isolation
- **Integration Tests**: Test component interactions
- **Compatibility Tests**: Compare C11 behavior with Go engine using same assets
- **Asset Tests**: Validate against actual MUGEN resources from the repository

### Benchmarking
Once core functionality exists:
- Performance comparison with Go engine
- Memory usage profiling
- Load time measurements
- Frame rate analysis

## Cross-Platform Support

### Target Platforms
- Linux (x86-64, ARM64)
- Windows (x86-64)
- macOS (Apple Silicon, Intel)

### Platform-Specific Considerations
- Use POSIX APIs where possible
- Provide Windows alternatives when needed (e.g., path separators)
- Test on all target platforms via CI
- CMake handles platform detection and configuration

## Integration Points

### Go Engine as Reference
- The Go implementation (`src/*.go`) remains the authoritative reference
- C11 behavior should match Go behavior for equivalent inputs
- When in doubt, Go engine's interpretation of MUGEN specs is correct

### Shared Assets
- Both engines use same asset directories
- No duplication of game resources
- C11 tests can compare results against Go engine processing same assets

### CI/CD
- C11 builds must pass on all platforms
- Tests must complete successfully
- Asset compatibility checks run on every commit
- Optional: side-by-side behavior comparison tests

## Documentation

### Code Documentation
- All public APIs documented in headers
- Implementation notes in source comments
- Complex algorithms explained with references to MUGEN behavior

### Architecture Documentation
- Component diagrams in `c11/docs/architecture/`
- Data structure definitions
- File format specifications

### Maintenance
- Update this roadmap as phases complete
- Document decisions and rationale
- Track compatibility issues with MUGEN edge cases

## Contributing Guidelines

### C11 Development
- All C11 work under `c11/` directory
- Follow C11 standard strictly (no compiler extensions)
- Pass `-Wall -Wextra -Wpedantic -Werror`
- Write tests for all new functionality
- Update documentation for API changes

### Asset Compatibility
- Never modify existing MUGEN assets to work with C11
- C11 must adapt to assets, not vice versa
- Report any asset compatibility issues as bugs
- Test with diverse MUGEN content

### Coordination with Go Development
- C11 refactor does not block Go engine improvements
- Periodically sync C11 behavior with Go engine updates
- Share bug fixes and insights between implementations

## Success Criteria

### Phase Completion
Each phase is complete when:
- All planned components implemented and tested
- CTest passes 100%
- Code review approved
- Documentation updated
- CI builds and tests pass on all platforms

### Overall Project Success
The C11 refactor is successful when:
- Feature parity with Go engine achieved
- Performance meets or exceeds Go implementation
- Full MUGEN asset compatibility maintained
- Active community adoption and contribution
- Stable, maintainable codebase

## Timeline

This is an **incremental, community-driven effort**. No hard deadlines. Each phase proceeds as contributors are available.

**Current Focus**: Phase 1 foundation is complete. Next step is Phase 2 configuration parsing.

## Questions and Discussion

For questions about the C11 refactor:
- Open a GitHub issue tagged with `c11-refactor`
- Reference this roadmap in discussions
- Propose changes to roadmap via pull request

---

**Last Updated**: 2025-11-17  
**Current Version**: 0.1.0 (Phase 1 Complete)
