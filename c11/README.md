# Ikemen C11 Engine

This directory contains the C11 refactor of the Ikemen fighting game engine.

## ⚠️ Important

**This is a work in progress.** The C11 engine is being developed in parallel with the existing Go engine. The Go engine in `../src/` remains the primary, production-ready version.

## Directory Structure

```
c11/
├── CMakeLists.txt      # Root CMake configuration
├── README.md           # This file
├── src/                # C11 implementation files
│   ├── engine.c        # Core engine implementation
│   ├── main.c          # Main executable entry point
│   └── CMakeLists.txt  # Source build configuration
├── include/            # Public header files
│   └── engine.h        # Core engine API
├── tests/              # Test files
│   ├── test_engine.c   # Engine unit tests
│   └── CMakeLists.txt  # Test build configuration
├── cmake/              # CMake modules
├── docs/               # Documentation
└── build/              # Build artifacts (not in git)
```

## Building

### Prerequisites

- CMake 3.20 or later
- C11-compliant compiler (GCC 5+, Clang 3.1+, MSVC 2015+)

### Linux/macOS

```bash
cd c11
mkdir -p build
cd build
cmake ..
cmake --build .
```

### Windows (with Visual Studio)

```cmd
cd c11
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### Windows (with MinGW/MSYS2)

```bash
cd c11
mkdir -p build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Running

After building, the executable will be in `build/bin/`:

```bash
./build/bin/ikemen_c11
```

## Testing

Run tests using CTest:

```bash
cd c11/build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./bin/test_engine
```

## Build Options

### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Release Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Disable Sanitizers (Debug mode only)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=OFF -DENABLE_UBSAN=OFF
```

## Development

### Code Style

- **Standard**: C11 (ISO/IEC 9899:2011)
- **Indentation**: 4 spaces
- **Line length**: Maximum 100 characters
- **Naming**:
  - Types: `PascalCase`
  - Functions: `snake_case`
  - Macros: `UPPER_SNAKE_CASE`

### Adding New Source Files

1. Create the `.c` file in `src/` and `.h` in `include/`
2. Update `src/CMakeLists.txt` to include the new source
3. Add corresponding tests in `tests/`
4. Update `tests/CMakeLists.txt` if adding new test executables

### Compiler Warnings

All warnings are treated as errors. Code must compile without warnings.

Enabled warnings include:
- `-Wall -Wextra -Wpedantic` (GCC/Clang)
- `-Werror` (treat warnings as errors)
- `-Wformat=2` (format string checking)
- `-Wstrict-prototypes -Wmissing-prototypes` (function declarations)

## Current Status

**Phase 0: Scaffolding** ✅

- [x] Directory structure
- [x] CMake build system
- [x] Minimal engine stub
- [x] Basic testing infrastructure
- [x] CI/CD pipeline

**Next Steps: Phase 1 - Core Runtime**

- [ ] Memory management system
- [ ] Platform abstraction layer
- [ ] Math library
- [ ] Data structures
- [ ] Configuration parser

## Documentation

- [C11 Roadmap](../C11-ROADMAP.md) - Development plan and guidelines
- [AI Guidelines](../COPILOT_C11_GUIDELINES.md) - For AI assistants
- [Project README](../README.md) - Main project documentation

## Contributing

All C11 contributions must:
1. Follow the coding guidelines in [C11-ROADMAP.md](../C11-ROADMAP.md)
2. Include unit tests
3. Pass all CI checks
4. Not modify existing Go sources in `../src/`

See [CONTRIBUTING.md](../CONTRIBUTING.md) for general guidelines.

## License

Same as the main project: MIT License. See [LICENSE.txt](../LICENCE.txt) for details.
