# Ikemen C11

A C11 reimplementation of the Ikemen GO fighting game engine.

## About

Ikemen C11 is a modern C11 port of Ikemen GO, originally written in Go. This version aims to provide:

- Better performance through native C code
- Smaller binary size
- Lower memory footprint
- More direct control over system resources
- Easier integration with existing C/C++ libraries

## Original Project

This is based on [Ikemen GO](https://github.com/ikemen-engine/Ikemen-GO), an open source fighting game engine that supports M.U.G.E.N resources.

## Status

**Current Phase: Initial Implementation**

This is currently a proof-of-concept demonstrating the refactoring approach from Go to C11. Core infrastructure is being established:

- ✅ Project structure and build system (CMake)
- ✅ Configuration file handling
- ✅ Command line argument parsing
- ✅ Basic initialization framework
- 🔄 Lua scripting interface (stub)
- ⏳ Window and rendering system
- ⏳ Audio system
- ⏳ Input handling
- ⏳ Game engine core
- ⏳ Character and stage loading
- ⏳ Fight system

## Building

See [BUILDING_C11.md](BUILDING_C11.md) for detailed build instructions.

Quick start (Linux):
```bash
mkdir build && cd build
cmake ..
make
./Ikemen_C11
```

## Architecture

The C11 implementation is organized into modules:

- `main.c` - Entry point and initialization
- `config.c/h` - Configuration and INI parsing
- `system.c/h` - System management, window, and main loop
- `lua_interface.c/h` - Lua scripting integration
- `render.c/h` - Graphics rendering (planned)
- `audio.c/h` - Audio system (planned)
- `input.c/h` - Input handling (planned)
- `game.c/h` - Game logic (planned)

## Differences from Go Version

| Aspect | Go Version | C11 Version |
|--------|-----------|-------------|
| Memory Management | Garbage collected | Manual |
| Concurrency | Goroutines & channels | Pthreads/manual |
| Error Handling | Multiple returns | Error codes |
| Dependencies | Go modules | pkg-config/CMake |
| Binary Size | ~20-40 MB | ~1-5 MB (target) |
| Startup Time | 100-200ms | <50ms (target) |

## Dependencies

### Core (Current)
- C11 compiler (GCC 4.9+, Clang 3.1+, MSVC 2015+)
- CMake 3.10+

### Planned
- Lua 5.1+
- GLFW3 or SDL2
- OpenGL 2.1+ / Vulkan
- FFmpeg
- libxmp

## License

MIT License - Same as original Ikemen GO.

See [LICENSE.txt](LICENSE.txt) for details.

## Contributing

This is an active refactoring project. Contributions are welcome, especially:

- Core engine module implementations
- Platform-specific optimizations
- Bug reports and testing
- Documentation improvements

## Original Go Code

The original Go source code is preserved in `src/*.go` files and will be progressively migrated.

## Credits

- Original Ikemen GO by Suehiro et al.
- C11 refactoring by the community
- M.U.G.E.N compatibility for the fighting game community
