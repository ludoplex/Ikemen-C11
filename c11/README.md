# Ikemen C11 Engine

**Status: Experimental / In Development**

This directory contains the scaffolding for a long-term refactoring effort to migrate the Ikemen GO engine from Go to C11. This is an incremental, gradual process that will not affect the existing Go implementation.

## Overview

The C11 implementation aims to provide:
- A clean, modern C11 codebase following best practices
- Improved performance characteristics
- Better integration with low-level graphics and audio APIs
- Maintained compatibility with M.U.G.E.N resources

**Important**: This is currently a minimal skeleton with basic structure only. The full engine functionality will be ported module by module over time.

## Directory Structure

```
c11/
├── src/              # C11 source files
├── include/          # Public header files
├── tests/            # Unit tests
├── cmake/            # CMake modules and utilities
├── CMakeLists.txt    # Top-level build configuration
└── README.md         # This file
```

## Building

### Prerequisites

- **CMake** 3.15 or higher
- **C Compiler** with C11 support:
  - GCC 4.9+ (recommended)
  - Clang 3.1+
  - MSVC 2015+ (on Windows)
- **Build tools**: make, ninja, or similar

### Quick Start (Linux/macOS)

```bash
# Configure the build
cmake -S c11 -B c11/build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build c11/build

# Run the skeleton executable
./c11/build/bin/ikemen_c11

# Run tests
ctest --test-dir c11/build --output-on-failure
```

### Windows (Visual Studio)

```bash
# Configure for Visual Studio
cmake -S c11 -B c11/build -G "Visual Studio 17 2022"

# Build
cmake --build c11/build --config Release

# Run
.\c11\build\bin\Release\ikemen_c11.exe

# Run tests
ctest --test-dir c11\build -C Release --output-on-failure
```

### Windows (MSYS2/MinGW)

```bash
# From MSYS2 MINGW64 shell
cmake -S c11 -B c11/build -G "MSYS Makefiles"
cmake --build c11/build
./c11/build/bin/ikemen_c11.exe
ctest --test-dir c11/build --output-on-failure
```

## Testing

The project uses CTest for running tests:

```bash
# Run all tests
ctest --test-dir c11/build

# Run with verbose output
ctest --test-dir c11/build --verbose

# Run specific test
ctest --test-dir c11/build -R EngineCore
```

## Development Status

### Completed
- [x] Basic directory structure
- [x] CMake build system
- [x] Minimal engine core module
- [x] Basic unit test framework

### In Progress
- [ ] Graphics subsystem
- [ ] Audio subsystem
- [ ] Input handling
- [ ] Resource management
- [ ] Character system
- [ ] Stage system
- [ ] Fight logic

### Future Work
- [ ] Full M.U.G.E.N compatibility
- [ ] Network multiplayer
- [ ] Advanced rendering features
- [ ] Performance optimizations
- [ ] Cross-platform testing (Windows, macOS, Linux)

## Contributing

Please see the [COPILOT_REFACTORING_GUIDE.md](../COPILOT_REFACTORING_GUIDE.md) in the repository root for guidelines specific to the C11 refactoring effort.

General contribution guidelines can be found in [CONTRIBUTING.md](../CONTRIBUTING.md).

### Code Style

- Follow C11 standard strictly
- Use consistent naming conventions:
  - `snake_case` for functions and variables
  - `UPPER_CASE` for constants and macros
  - Prefix public API functions with `ikemen_`
- Add MIT license header to all new files
- Document public APIs with clear comments
- Write unit tests for new functionality

## License

The C11 implementation follows the same MIT License as the original Ikemen GO engine. See [LICENCE.txt](../LICENCE.txt) for details.
