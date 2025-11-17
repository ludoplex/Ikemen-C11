# Ikemen C11 Refactor Roadmap

## Goal

This document outlines the plan for refactoring the Ikemen GO engine from Go to C11. The goal is to create a high-performance, cross-platform fighting game engine using modern C11 standards while maintaining compatibility with M.U.G.E.N resources.

## Core Principles

### 1. Non-Destructive Development
- **All C11 work MUST reside under the `c11/` directory**
- **Existing Go sources in `src/` MUST NOT be modified or deleted**
- The C11 refactor runs in parallel with the Go engine until feature parity is achieved
- Both engines will coexist during the transition period

### 2. Code Organization
```
c11/
├── src/           # C11 implementation sources
├── include/       # Public header files
├── tests/         # Unit and integration tests
├── cmake/         # CMake modules and build scripts
├── docs/          # C11-specific documentation
└── build/         # Out-of-tree build directory (gitignored)
```

## Development Phases

### Phase 0: Scaffolding (Current)
- ✓ Set up directory structure
- ✓ Create minimal build system with CMake
- ✓ Add basic test infrastructure
- ✓ Configure CI/CD pipeline
- ✓ Document development guidelines

### Phase 1: Core Runtime & Foundation
- Memory management system (arena allocators, object pools)
- Platform abstraction layer (file I/O, threading, timing)
- Math library (vectors, matrices, transforms)
- Data structures (hash maps, dynamic arrays, strings)
- Logging and error handling
- Configuration parsing (INI format)

### Phase 2: Resource Loading
- Image loading (PNG, PCX formats)
- Font rendering system
- Sound loading (WAV, OGG, MP3)
- File system abstraction
- Archive handling (ZIP support)

### Phase 3: Rendering Backend
- OpenGL 3.2+ renderer
- Vulkan renderer (optional, future)
- Shader management
- Texture management and atlasing
- Sprite rendering pipeline
- Animation system

### Phase 4: Audio Backend
- OpenAL integration
- FFmpeg audio decoding
- Module music support (libxmp)
- Sound effect mixing
- Music playback

### Phase 5: Game Logic
- Character state machine
- Move/command interpreter
- Hit detection and collision
- Stage management
- Camera system
- Particle effects

### Phase 6: Scripting & Bytecode
- Port Lua-based scripting system
- CNS/ST bytecode compiler
- Expression evaluator
- Trigger system

### Phase 7: Network & Rollback
- Network protocol implementation
- GGPO-style rollback netcode
- Input prediction and synchronization
- Replay system

### Phase 8: UI & Menus
- Menu system
- Lifebars and HUD
- Character select screen
- Options and configuration UI

### Phase 9: Testing & Polish
- Performance profiling and optimization
- Memory leak detection
- Cross-platform testing
- Documentation completion
- Migration tools

## Coding Guidelines

### C11 Standard Compliance
- Strictly adhere to ISO C11 (ISO/IEC 9899:2011)
- Use `_Static_assert` for compile-time checks
- Leverage `_Alignas`, `_Alignof` for memory alignment
- Use `stdatomic.h` for lock-free operations where appropriate
- Prefer `stdbool.h` for boolean types

### Compiler Settings
- Enable all warnings: `-Wall -Wextra -Wpedantic`
- Treat warnings as errors: `-Werror`
- Use sanitizers during development:
  - AddressSanitizer (`-fsanitize=address`)
  - UndefinedBehaviorSanitizer (`-fsanitize=undefined`)
  - LeakSanitizer (part of ASan)

### Code Style
- **Indentation**: 4 spaces (no tabs)
- **Line length**: Maximum 100 characters
- **Naming conventions**:
  - Types: `PascalCase` (e.g., `RenderContext`, `CharacterState`)
  - Functions: `snake_case` (e.g., `render_sprite`, `load_character`)
  - Macros: `UPPER_SNAKE_CASE` (e.g., `MAX_BUFFER_SIZE`)
  - Private functions: prefix with `_` (e.g., `_internal_helper`)
- **Header guards**: Use `#pragma once` or include guards with project prefix
- **Comments**: Doxygen-style for API documentation

### Error Handling
- Use explicit error codes (enums)
- Never use `assert()` for runtime errors (only preconditions)
- Prefer returning error codes over global error states
- Log errors with context information

### Memory Management
- Minimize heap allocations in hot paths
- Use arena allocators for frame-local memory
- Clear ownership semantics (document who owns/frees memory)
- Zero-initialize all structures
- Avoid manual memory management where possible (use RAII-style patterns)

### Static Analysis
- Run `clang-tidy` with strict checks
- Use `cppcheck` for additional validation
- Consider Infer or Coverity for deeper analysis
- All code must pass static analysis before merge

### Testing Requirements
- Unit tests for all public APIs
- Integration tests for subsystems
- Minimum 80% code coverage target
- Performance benchmarks for critical paths
- Memory leak tests (Valgrind, ASan)

### Build System
- CMake 3.20+ required
- Support out-of-tree builds
- Provide presets for common configurations
- Cross-compilation support (Windows, Linux, macOS)
- Optional features via CMake options

### Documentation
- Doxygen comments for all public APIs
- README in each major subsystem directory
- Architecture decision records (ADRs) for significant choices
- Keep documentation in sync with code

## Dependencies

### Required
- C11-compliant compiler (GCC 5+, Clang 3.1+, MSVC 2015+)
- CMake 3.20+
- OpenGL 3.2+ support

### Optional
- Vulkan SDK (for Vulkan renderer)
- FFmpeg 4.0+ (for video/audio)
- OpenAL Soft (for audio)
- libxmp (for module music)
- Lua 5.4+ (for scripting)

## Platform Support

### Primary Targets
- Linux (x86_64, ARM64)
- Windows (x86_64)
- macOS (ARM64, x86_64)

### Minimum Requirements
- Windows 7 SP1 or later
- Linux kernel 3.10+ with glibc 2.17+
- macOS 10.13+ (High Sierra)

## Migration Strategy

1. **Parallel Development**: C11 and Go engines run side-by-side
2. **Feature Parity**: Achieve 100% compatibility with Go version
3. **Testing Period**: Extended beta testing with community
4. **Gradual Transition**: Move users to C11 version over time
5. **Go Deprecation**: Archive Go version once C11 is stable

## Success Criteria

- ✓ Complete feature parity with Ikemen GO
- ✓ Better or equivalent performance
- ✓ Stable on all target platforms
- ✓ Active community testing and feedback
- ✓ Comprehensive documentation
- ✓ Zero critical bugs in bug tracker

## Resources

- [C11 Standard (draft)](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
- [CMake Documentation](https://cmake.org/documentation/)
- [M.U.G.E.N Documentation](http://www.elecbyte.com/mugendocs/)
- [Ikemen GO Wiki](https://github.com/ikemen-engine/Ikemen-GO/wiki)

## Contributing

See `COPILOT_C11_GUIDELINES.md` for AI assistant guidelines.
See `CONTRIBUTING.md` for general contribution guidelines.

All C11 contributions must:
1. Follow the coding guidelines above
2. Include tests
3. Pass CI checks
4. Be reviewed by maintainers
5. Not modify existing Go sources

---

Last Updated: 2025-11-17
