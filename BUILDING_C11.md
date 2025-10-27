# Ikemen C11 Build Instructions

This document describes how to build the C11 version of Ikemen.

## Overview

The Ikemen C11 project is a reimplementation of Ikemen GO in C11 for better performance and portability.

## Prerequisites

### All Platforms
- CMake 3.10 or higher
- C compiler with C11 support (GCC, Clang, or MSVC)

### Future Dependencies (To be added)
- Lua 5.1+ development libraries
- GLFW3
- OpenGL
- FFmpeg development libraries
- libxmp

## Building

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install -y build-essential cmake

# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make

# The executable will be at: ./Ikemen_C11
```

### macOS

```bash
# Install dependencies via Homebrew
brew install cmake

# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make

# The executable will be at: ./Ikemen_C11
```

### Windows (MSYS2/MinGW)

```bash
# Install dependencies
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc

# Create build directory
mkdir build
cd build

# Configure and build
cmake -G "MinGW Makefiles" ..
mingw32-make

# The executable will be at: ./Ikemen_C11.exe
```

### Windows (Visual Studio)

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# The executable will be in: ./Release/Ikemen_C11.exe
```

## Current Status

This is an initial proof-of-concept implementation. The following components are currently implemented:

- [x] Basic project structure
- [x] Configuration file parsing
- [x] Command line argument processing
- [x] Directory initialization
- [ ] Window management (TODO)
- [ ] OpenGL rendering (TODO)
- [ ] Lua script integration (TODO)
- [ ] Audio system (TODO)
- [ ] Input handling (TODO)
- [ ] Game logic (TODO)

## Running

After building, ensure you have the required data files in place:
- `data/` directory
- `font/` directory
- `external/` directory

Then run:
```bash
./Ikemen_C11        # Linux/macOS
./Ikemen_C11.exe    # Windows
```

## Development Notes

The C11 implementation follows these principles:
1. Use standard C11 features for portability
2. Minimize external dependencies
3. Clear separation of concerns (modules)
4. Platform-specific code isolated in separate files
5. Comprehensive error handling

## Migration from Go

The original Go codebase is being progressively migrated to C11. Key changes:

- Go packages → C modules (header/source pairs)
- Go interfaces → Function pointers and structs
- Go goroutines → Manual threading (when needed)
- Go channels → Message queues/mutexes
- Go's defer → Explicit cleanup functions
- Go's garbage collection → Manual memory management

## License

Ikemen C11 is licensed under the MIT License, same as the original Ikemen GO.
