# Ikemen C11 Engine

A C11 implementation of the Ikemen fighting game engine, designed for full feature parity with the Go reference implementation and compatibility with MUGEN resources.

## Overview

**Current Status: Phase 1 - Foundation (v0.1.0)**

The Go reference implementation (`src/main.go`, `src/system.go`) is a complete 2D fighting game engine that includes:
- Graphics window and rendering system (OpenGL 2.1/3.2, Vulkan 1.3)
- Lua scripting engine for game logic and flow control
- Complete MUGEN asset support (SFF sprites, AIR animations, CNS states, CMD commands)
- Audio system with BGM and sound effect playback
- Input handling with keyboard, joystick, and gamepad support
- Network play with rollback netcode
- Full match system with multiple game modes (Arcade, Versus, Training, etc.)
- Character, stage, and screenpack loading
- Save system for replays, config, and statistics

This C11 port aims to replicate **all** of that functionality using only C11 standard and portable libraries. The implementation proceeds in phases (see `docs/ROADMAP.md`), with each phase building working, tested functionality.

**Phase 1 provides:**
- Engine version identification
- Asset directory structure validation
- Foundation architecture for future components
- Build system and test infrastructure

**Future phases will add:** Configuration parsing, MUGEN file format support, rendering, physics, audio, input, and complete game logic.

## Quick Start

### Build Requirements
- C compiler with C11 support (GCC 4.9+, Clang 3.1+, MSVC 2015+)
- CMake 3.10 or later
- Standard build tools (make, ninja, or Visual Studio)

### Building

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run tests
ctest --output-on-failure

# Run asset compatibility check
./bin/ikemen_c11 ../..
```

### Cross-Platform Notes

**Linux:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

**macOS:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

**Windows (Visual Studio):**
```bash
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Project Structure

```
c11/
├── CMakeLists.txt       # Build configuration
├── README.md            # This file
├── include/             # Public API headers
│   └── ikemen_engine.h  # Main engine API
├── src/                 # Implementation sources
│   ├── engine.c         # Core engine logic
│   └── main.c           # Main executable
├── tests/               # Test programs
│   ├── test_version.c           # Version function tests
│   └── test_asset_compat.c      # Asset compatibility tests
└── docs/                # Documentation
    └── ROADMAP.md       # Development roadmap
```

## Current Status

**Version:** 0.1.0 (Phase 1 - Foundation)

**What's Implemented:**
- ✅ Version query functions
- ✅ Asset directory structure validation (data/, external/, font/)
- ✅ CMake build system with strict C11 enforcement
- ✅ Comprehensive test suite integrated with CTest
- ✅ CI/CD across Linux, macOS, Windows

**What's NOT Yet Implemented:**
The Go engine (`src/main.go`) does much more at startup that the C11 port will eventually replicate:
- ❌ Configuration file parsing (config.ini, system.def)
- ❌ Window creation and graphics initialization
- ❌ MUGEN file format loading (SFF, AIR, CNS, CMD, DEF)
- ❌ Lua scripting integration
- ❌ Rendering subsystem (OpenGL/Vulkan)
- ❌ Audio system (BGM, sound effects)
- ❌ Input system (keyboard, joystick, gamepad)
- ❌ Character and stage loading
- ❌ Game loop and match management
- ❌ Network play

**Planned:**
See [docs/ROADMAP.md](docs/ROADMAP.md) for the complete multi-phase development plan.

## API Overview

### Version Functions

```c
#include "ikemen_engine.h"

int major = ikm_engine_major_version();
int minor = ikm_engine_minor_version();
int patch = ikm_engine_patch_version();
const char* version = ikm_engine_version_string();  // "0.1.0"
```

### Asset Compatibility Check

```c
#include "ikemen_engine.h"

// Check if asset directories exist and are accessible
int result = ikm_check_asset_compatibility(".");
if (result == IKM_SUCCESS) {
    printf("Asset structure compatible\n");
} else {
    printf("Error: %s\n", ikm_get_last_error());
}
```

## Testing

The test suite covers:
- Unit tests for individual functions
- Integration tests for component interactions
- Asset compatibility validation

Run all tests:
```bash
cd build
ctest --output-on-failure
```

Run specific test:
```bash
./bin/test_version
./bin/test_asset_compat
```

## Development

### Code Standards
- **C Standard:** C11 (ISO/IEC 9899:2011)
- **Compiler Extensions:** None (strictly standard-compliant)
- **Warnings:** Compiled with `-Wall -Wextra -Wpedantic -Werror`
- **Style:** Follow existing code conventions

### Adding New Features

1. **Design:** Review [docs/ROADMAP.md](docs/ROADMAP.md) for planned architecture
2. **Implement:** Add source files to `src/`, headers to `include/`
3. **Test:** Create test file in `tests/` and integrate with CTest
4. **Document:** Update headers with API docs, add implementation comments
5. **Build:** Update `CMakeLists.txt` with new targets

### Example: Adding a Module

```cmake
# In CMakeLists.txt

# Add source to library
add_library(ikemen_engine STATIC
    src/engine.c
    src/newmodule.c  # New module
)

# Add test
add_executable(test_newmodule tests/test_newmodule.c)
target_link_libraries(test_newmodule PRIVATE ikemen_engine)
add_test(NAME NewModule COMMAND test_newmodule)
```

## Asset Compatibility

The C11 engine expects the same directory structure as the Go implementation:

```
repository_root/
├── data/         # Common data files (required)
├── external/     # External resources (required)
├── font/         # Font files (required)
├── chars/        # Character definitions (optional, from screenpack)
├── stages/       # Stage definitions (optional, from screenpack)
└── sound/        # Sound files (optional, from screenpack)
```

Asset search order matches the Go engine:
1. Explicit path (absolute or relative)
2. Relative to definition file
3. Motif directory (screenpack)
4. `data/` directory
5. `external/` directory
6. `font/` directory

## Contributing

### Before Starting
1. Read [../C11-REFORM-GUIDE.md](../C11-REFORM-GUIDE.md) for project principles
2. Review [docs/ROADMAP.md](docs/ROADMAP.md) for planned work
3. Check existing issues for tasks

### Contribution Workflow
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Write/update tests
5. Ensure CI passes
6. Submit a pull request

### Code Review Criteria
- ✅ Compiles without warnings
- ✅ All tests pass
- ✅ Code follows C11 standard
- ✅ Public APIs are documented
- ✅ No modifications to Go sources or shared assets
- ✅ Asset compatibility maintained

## Frequently Asked Questions

**Q: Why C11 instead of newer C standards?**  
A: C11 provides a good balance of modern features and wide compiler support. It's supported by GCC 4.9+, Clang 3.1+, and MSVC 2015+.

**Q: Will this replace the Go engine?**  
A: No. The Go engine remains the primary, reference implementation. C11 is an alternative for those who prefer a pure C implementation.

**Q: Can I use both engines with the same assets?**  
A: Yes! That's a core design goal. Both engines should work identically with the same MUGEN resources.

**Q: How can I help?**  
A: Check [docs/ROADMAP.md](docs/ROADMAP.md) for planned features. Pick something that interests you, implement it, test it, and submit a PR.

**Q: What about performance?**  
A: Performance benchmarking will begin once core functionality is implemented. The goal is to match or exceed Go engine performance.

## Resources

- **Main Documentation:** [../C11-REFORM-GUIDE.md](../C11-REFORM-GUIDE.md)
- **Development Roadmap:** [docs/ROADMAP.md](docs/ROADMAP.md)
- **AI/Copilot Guidelines:** [../COPILOT_C11_POLICY.md](../COPILOT_C11_POLICY.md)
- **Go Engine Reference:** [../src/](../src/)
- **MUGEN Specification:** Community documentation (external)

## License

Same as the main Ikemen GO project. See [../LICENCE.txt](../LICENCE.txt).

## Contact

For questions or discussion:
- Open a GitHub issue tagged with `c11-refactor`
- Reference this directory in discussions
- Propose changes via pull request

---

**Current Version:** 0.1.0  
**Last Updated:** 2025-11-17  
**Status:** Phase 1 Complete - Foundation established
