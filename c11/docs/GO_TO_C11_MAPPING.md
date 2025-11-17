# Ikemen C11 Engine - Go to C11 Mapping Reference

This document maps Go functions, variables, data structures, and control flow from the original Ikemen-GO implementation to their C11 equivalents.

## Quick Reference Table

| Go Component | Go File Location | C11 Equivalent | C11 File Location |
|--------------|------------------|----------------|-------------------|
| **Main Entry** | | | |
| `main()` | src/main.go:53 | `main()` | c11/src/main.c:72 |
| `Version` variable | src/main.go:15 | `IKM_ENGINE_VERSION` macro | c11/include/ikemen_engine.h:13 |
| `BuildTime` variable | src/main.go:16 | (not yet implemented) | - |
| **System Initialization** | | | |
| `sys.init()` | src/system.go:344 | `ikm_engine_initialize()` | c11/src/engine.c:487 |
| `sys.shutdown()` | src/system.go:511 | `ikm_engine_shutdown()` | c11/src/engine.c:504 |
| `System` struct | src/system.go:99-335 | `ikm_engine_t` struct | c11/src/engine.c:30 |
| **Configuration** | | | |
| `Config` struct | src/config.go:48-237 | `ikm_config_t` struct | c11/include/ikemen_engine.h:72 |
| `loadConfig()` | src/config.go:239 | `ikm_config_load()` | c11/src/engine.c:324 |
| INI parsing | gopkg.in/ini.v1 | `ikm_ini_load()` | c11/src/engine.c:147 |
| **Directory Management** | | | |
| `os.Mkdir("save")` | src/main.go:67 | `ikm_create_directories()` | c11/src/engine.c:395 |
| **Error Handling** | | | |
| `chk(err)` | src/main.go:23 | `ikm_log(IKM_LOG_FATAL)` + `return EXIT_FAILURE` | c11/src/main.c:* |
| `panic()` | Go runtime | `exit()` after logging | c11/src/main.c:* |
| **Logging** | | | |
| `createLog()` | src/main.go:42 | `ikm_log_set_file()` | c11/src/engine.c:555 |
| `log.Logger` | src/system.go:242 | `ikm_log()` | c11/src/engine.c:539 |

## Detailed Control Flow Mapping

### 1. Main Function Flow

#### Go Implementation (src/main.go:53-138)
```go
func main() {
    // [1] Get executable path
    exePath, err := os.Executable()
    if isRunningInsideAppBundle(exePath) {
        os.Chdir(path.Dir(exePath))
        os.Chdir("../../../")
    }
    
    // [2] Create directories
    os.Mkdir("save", os.ModeSticky|0755)
    os.Mkdir("save/replays", os.ModeSticky|0755)
    os.Mkdir("save/logs", os.ModeSticky|0755)
    
    // [3] Process command line
    processCommandLine()
    
    // [4] Initialize stats
    if _, err := os.ReadFile("save/stats.json"); err != nil {
        f, _ := os.Create("save/stats.json")
        f.Write([]byte("{}"))
        f.Close()
    }
    
    // [5] Load config
    cfgPath := "save/config.ini"
    if _, ok := sys.cmdFlags["-config"]; ok {
        cfgPath = sys.cmdFlags["-config"]
    }
    cfg, err := loadConfig(cfgPath)
    chk(err)
    sys.cfg = *cfg
    
    // [6] Verify Lua script
    ftemp, err1 := os.Open(sys.cfg.Config.System)
    if err1 != nil {
        ShowErrorDialog(err1.Error())
        panic(err2)
    }
    ftemp.Close()
    
    // [7] Initialize engine
    sys.luaLState = sys.init(sys.gameWidth, sys.gameHeight)
    defer sys.shutdown()
    
    // [8] Execute Lua
    err := sys.luaLState.DoFile(sys.cfg.Config.System)
    if err != nil {
        errorLog := createLog("Ikemen.log")
        defer closeLog(errorLog)
        fmt.Fprintf(errorLog, "Version: %s\n", Version)
        fmt.Fprintln(errorLog, err)
        ShowErrorDialog(fmt.Sprintf("%s\n\nError saved to Ikemen.log", err))
        panic(err)
    }
}
```

#### C11 Implementation (c11/src/main.c:72-253)
```c
int main(int argc, char** argv) {
    ikm_engine_t* engine = NULL;
    ikm_config_t config;
    ikm_system_info_t system_info;
    const char* config_path = "save/config.ini";
    int result;
    
    // [1] Executable path handling - NOT YET IMPLEMENTED
    // TODO: Detect macOS bundle and change directory
    
    // Parse command line (line 88-99)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-config") == 0 && i + 1 < argc) {
            config_path = argv[++i];
        }
    }
    
    // Setup logging (line 102-108)
    ikm_log_set_file(log_path);
    ikm_log_set_level(IKM_LOG_INFO);
    
    // [2] Create directories (line 112-121)
    result = ikm_create_directories(base_path);
    if (result != IKM_SUCCESS) {
        ikm_log(IKM_LOG_ERROR, "Failed: %s", ikm_get_last_error());
        return EXIT_FAILURE;
    }
    
    // [4] Initialize stats (line 124-138)
    if (!file_exists(stats_path)) {
        FILE* f = fopen(stats_path, "w");
        if (f) {
            fprintf(f, "{}");
            fclose(f);
        }
    }
    
    // [5] Load config (line 141-150)
    result = ikm_config_load(config_path, &config);
    if (result != IKM_SUCCESS) {
        ikm_log(IKM_LOG_ERROR, "Failed: %s", ikm_get_last_error());
    }
    
    // [6] Verify Lua script (line 166-172)
    if (!file_exists(config.system_script)) {
        ikm_log(IKM_LOG_FATAL, "Missing: %s", config.system_script);
        return EXIT_FAILURE;
    }
    
    // [7] Initialize engine (line 181-198)
    engine = ikm_engine_create();
    result = ikm_engine_initialize(engine, &config);
    if (result != IKM_SUCCESS) {
        ikm_log(IKM_LOG_FATAL, "Init failed: %s", ikm_get_last_error());
        ikm_engine_destroy(engine);
        return EXIT_FAILURE;
    }
    
    // [8] Execute Lua - NOT YET IMPLEMENTED
    // TODO: Integrate Lua interpreter and execute main script
    
    // Cleanup
    ikm_engine_shutdown(engine);
    ikm_engine_destroy(engine);
    
    return EXIT_SUCCESS;
}
```

### 2. System Initialization (sys.init)

#### Go Implementation (src/system.go:344-510)
```go
func (s *System) init(w, h int32) *lua.LState {
    // [1] Set game size
    s.setGameSize(w, h)
    
    // [2] Init character info
    for i := range sys.cgi {
        sys.cgi[i].palInfo = make(map[int]PalInfo)
    }
    
    // [3] Load localcoords
    s.loadLocalcoords()
    
    // [4] Set motif scale
    s.setMotifScale()
    
    // [5] Create window
    s.window, err = s.newWindow(int(s.scrrect[2]), int(s.scrrect[3]))
    chk(err)
    
    // [6] Update gamepad mappings
    input.UpdateGamepadMappings(sys.cfg.Config.GamepadMappings)
    
    // [7] Correct joystick mappings (macOS)
    if runtime.GOOS == "darwin" {
        // ... joystick GUID matching ...
    }
    
    // [8] Load external shaders
    if len(s.cfg.Video.ExternalShaders) > 0 {
        s.externalShaders = make([][][]byte, 2)
        for i, shaderLocation := range s.cfg.Video.ExternalShaders {
            if s.cfg.Video.RenderMode != "Vulkan 1.3" {
                s.externalShaders[0][i], _ = os.ReadFile(shaderLocation + ".vert")
                s.externalShaders[1][i], _ = os.ReadFile(shaderLocation + ".frag")
            } else {
                s.externalShaders[0][i], _ = os.ReadFile(shaderLocation + ".vert.spv")
                s.externalShaders[1][i], _ = os.ReadFile(shaderLocation + ".frag.spv")
            }
        }
    }
    
    // [9] Initialize rendering backend
    if s.cfg.Video.RenderMode == "Vulkan 1.3" {
        gfx = &Renderer_VK{}
        gfxFont = &FontRenderer_VK{}
    } else if s.cfg.Video.RenderMode == "OpenGL 2.1" {
        gfx = &Renderer_GL21{}
        gfxFont = &glfont.FontRenderer_GL21{}
    } else {
        gfx = &Renderer_GL32{}
        gfxFont = &glfont.FontRenderer_GL32{}
    }
    gfx.Init()
    gfxFont.Init()
    
    // [10] Initialize audio
    sampleRate := beep.SampleRate(sys.cfg.Sound.Freq)
    bufferSize := sampleRate.N(time.Second / 60)
    err := speaker.Init(sampleRate, bufferSize)
    if err != nil {
        s.nomusic = true
    } else {
        speaker.Play(s.soundMixer)
    }
    
    // [11] Create debug font
    s.debugFont = NewTextSprite()
    
    // [12] Setup input
    input.Init(s.window)
    
    // [13] Initialize Lua
    L := lua.NewState()
    // Register 100+ functions...
    return L
}
```

#### C11 Implementation (c11/src/engine.c:487-503)
```c
int ikm_engine_initialize(ikm_engine_t* engine, const ikm_config_t* config) {
    if (!engine) {
        set_error("Invalid engine instance");
        return IKM_ERROR_UNKNOWN;
    }
    
    if (engine->initialized) {
        set_error("Engine already initialized");
        return IKM_ERROR_UNKNOWN;
    }
    
    // Copy configuration
    memcpy(&engine->config, config, sizeof(ikm_config_t));
    strcpy(engine->base_path, ".");
    
    // Get system information
    int result = ikm_get_system_info(engine->base_path, &engine->system_info);
    if (result != IKM_SUCCESS) {
        return result;
    }
    
    // [1-4] Game size, localcoords, motif scale - NOT YET IMPLEMENTED
    // TODO: Implement coordinate system setup
    
    // [5] Window creation - NOT YET IMPLEMENTED
    // TODO: Create GLFW/SDL window wrapper
    
    // [6-7] Input system - NOT YET IMPLEMENTED
    // TODO: Initialize input handling
    
    // [8] Shader loading - NOT YET IMPLEMENTED
    // TODO: Load and compile shaders
    
    // [9] Rendering backend - NOT YET IMPLEMENTED
    // TODO: Initialize OpenGL/Vulkan renderer
    
    // [10] Audio system - NOT YET IMPLEMENTED
    // TODO: Initialize audio backend
    
    // [11] Debug font - NOT YET IMPLEMENTED
    // TODO: Create text rendering system
    
    // [12] Input setup - NOT YET IMPLEMENTED
    // TODO: Configure keyboard/joystick bindings
    
    // [13] Lua integration - NOT YET IMPLEMENTED
    // TODO: Create Lua state and register functions
    
    engine->initialized = 1;
    ikm_log(IKM_LOG_INFO, "Engine initialized successfully");
    return IKM_SUCCESS;
}
```

### 3. Data Structure Mapping

#### System Struct

**Go (src/system.go:99-335) - 200+ fields:**
```go
type System struct {
    randseed                int32
    scrrect                 [4]int32
    gameWidth, gameHeight   int32
    widthScale, heightScale float32
    window                  *Window
    gameEnd, frameSkip      bool
    brightness              float32
    cfg                     Config
    ffx                     map[string]*FightFx
    sel                     Select
    keyState                map[Key]bool
    chars                   [MaxPlayerNo][]*Char
    stage                   *Stage
    lifebar                 Lifebar
    soundMixer              *beep.Mixer
    bgm                     Bgm
    luaLState               *lua.LState
    // ... 180+ more fields
}
```

**C11 (c11/src/engine.c:30-36) - Currently minimal:**
```c
struct ikm_engine {
    ikm_config_t config;
    ikm_system_info_t system_info;
    int initialized;
    char base_path[IKM_MAX_PATH];
};

// TODO: Expand to include all Go System fields:
// - Window pointer
// - Character array [MaxPlayerNo]
// - Stage pointer
// - Lifebar
// - Sound mixer
// - Lua state
// - Input state
// - Game state variables
// - Timing variables
// - Rendering state
// - etc.
```

#### Config Struct

**Go (src/config.go:48-237):**
```go
type Config struct {
    Def     string
    IniFile *ini.File
    Common  struct {
        Air     map[string][]string
        Cmd     map[string][]string
        Const   map[string][]string
        States  map[string][]string
    }
    Options struct {
        Difficulty int
        Life       float32
        Time       int32
    }
    Video struct {
        Width      int
        Height     int
        Fullscreen bool
        RenderMode string
    }
    Sound struct {
        MasterVolume int
        BGMVolume    int
        WavVolume    int
    }
    // ... many more sections
}
```

**C11 (c11/include/ikemen_engine.h:72-100):**
```c
typedef struct {
    /* Video settings */
    int width;
    int height;
    int fullscreen;
    double gamma;
    int msaa;
    char render_mode[64];
    
    /* Audio settings */
    int master_volume;
    int bgm_volume;
    int sfx_volume;
    int frequency;
    int channels;
    
    /* System settings */
    char system_script[IKM_MAX_PATH];
    char motif[IKM_MAX_PATH];
    char common_air[IKM_MAX_PATH];
    char common_cmd[IKM_MAX_PATH];
    
    /* Gameplay settings */
    int difficulty;
    double life_multiplier;
    int time_limit;
    int rounds_to_win;
    
    /* Debug settings */
    int debug_mode;
    int console_enabled;
    
    /* Input settings */
    int joystick_enabled;
} ikm_config_t;

// TODO: Add Common struct for Air, Cmd, Const, States maps
// TODO: Add more detailed Option subsections
// TODO: Add Key bindings array
```

### 4. Function Mapping Reference

| Go Function | C11 Function | Status | Notes |
|-------------|--------------|--------|-------|
| **Configuration** | | | |
| `loadConfig()` | `ikm_config_load()` | ✓ Implemented | c11/src/engine.c:324 |
| `ini.Load()` | `ikm_ini_load()` | ✓ Implemented | c11/src/engine.c:147 |
| `ini.Section().Key()` | `ikm_ini_get()` | ✓ Implemented | c11/src/engine.c:195 |
| **System** | | | |
| `sys.init()` | `ikm_engine_initialize()` | ⚠ Partial | c11/src/engine.c:487 |
| `sys.shutdown()` | `ikm_engine_shutdown()` | ⚠ Partial | c11/src/engine.c:504 |
| `sys.setGameSize()` | (not implemented) | ✗ TODO | - |
| `sys.loadLocalcoords()` | (not implemented) | ✗ TODO | - |
| `sys.setMotifScale()` | (not implemented) | ✗ TODO | - |
| `sys.newWindow()` | (not implemented) | ✗ TODO | - |
| `sys.update()` | (not implemented) | ✗ TODO | - |
| `sys.draw()` | (not implemented) | ✗ TODO | - |
| **Directory** | | | |
| `os.Mkdir()` | `ikm_create_directories()` | ✓ Implemented | c11/src/engine.c:395 |
| `os.ReadFile()` | `file_exists()` | ✓ Implemented | c11/src/engine.c:68 |
| **Error Handling** | | | |
| `chk(err)` | `ikm_log() + exit()` | ✓ Implemented | Pattern in main.c |
| `panic()` | `exit(EXIT_FAILURE)` | ✓ Implemented | Pattern in main.c |
| **Logging** | | | |
| `createLog()` | `ikm_log_set_file()` | ✓ Implemented | c11/src/engine.c:555 |
| `fmt.Fprintf(log, ...)` | `ikm_log()` | ✓ Implemented | c11/src/engine.c:539 |
| **Utilities** | | | |
| `strings.TrimSpace()` | `trim_whitespace()` | ✓ Implemented | c11/src/engine.c:97 |
| `filepath.Join()` | `join_path()` | ✓ Implemented | c11/src/engine.c:89 |
| `os.Open()` | `fopen()` | ✓ Available | Standard C |
| `io.ReadAll()` | `fread()` | ✓ Available | Standard C |

### 5. Control Flow Patterns

#### Error Handling Pattern

**Go Pattern:**
```go
result, err := someOperation()
if err != nil {
    chk(err)  // Calls panic()
}
```

**C11 Pattern:**
```c
int result = someOperation();
if (result != IKM_SUCCESS) {
    ikm_log(IKM_LOG_ERROR, "Operation failed: %s", ikm_get_last_error());
    cleanup();
    return EXIT_FAILURE;
}
```

#### Deferred Cleanup Pattern

**Go Pattern:**
```go
file, err := os.Open("file.txt")
defer file.Close()
// Use file...
```

**C11 Pattern:**
```c
FILE* file = fopen("file.txt", "r");
if (!file) {
    return error;
}
// Use file...
fclose(file);  // Manual at end of scope
```

#### Map Access Pattern

**Go Pattern:**
```go
value, exists := myMap[key]
if exists {
    // Use value
}
```

**C11 Pattern:**
```c
const char* value = ikm_ini_get(ini, section, key, NULL);
if (value != NULL) {
    // Use value
}
```

### 6. Not Yet Implemented (Priority Order)

Based on CFG analysis, these are critical for engine functionality:

1. **Lua Integration** (CRITICAL)
   - Go: `lua.NewState()`, `L.DoFile()`, `L.SetGlobal()`
   - C11: Need to embed Lua 5.1+ and bind C functions
   - Files needed: c11/src/lua_bindings.c
   - Without this, the engine cannot run (Lua drives everything)

2. **Window Creation** (HIGH)
   - Go: `s.newWindow()` (uses GLFW)
   - C11: Need GLFW or SDL wrapper
   - Files needed: c11/src/window.c, c11/include/window.h

3. **Rendering Backend** (HIGH)
   - Go: `gfx.Init()`, `gfx.Clear()`, `gfx.Draw()`
   - C11: OpenGL/Vulkan abstraction
   - Files needed: c11/src/renderer_gl.c, c11/src/renderer_vk.c

4. **Input System** (HIGH)
   - Go: `input.Init()`, `input.Update()`
   - C11: Keyboard/joystick handling
   - Files needed: c11/src/input.c

5. **Asset Loaders** (MEDIUM)
   - Go: SFF, AIR, CNS, DEF parsers in various files
   - C11: Binary file parsers
   - Files needed: c11/src/sff.c, c11/src/air.c, c11/src/cns.c

6. **Game Loop** (MEDIUM)
   - Go: `sys.update()`, `sys.draw()`, `sys.await()`
   - C11: Frame timing and update logic
   - Files needed: c11/src/gameloop.c

7. **Character System** (MEDIUM)
   - Go: `Char` struct, state machine
   - C11: Character management
   - Files needed: c11/src/char.c

8. **Audio System** (LOW)
   - Go: `speaker.Init()`, `beep.Mixer`
   - C11: Audio playback (OpenAL/SDL_mixer)
   - Files needed: c11/src/audio.c

### 7. File Organization

```
c11/
├── include/
│   └── ikemen_engine.h          - Main API (maps to all Go packages)
├── src/
│   ├── engine.c                 - System init, config, INI parser
│   ├── main.c                   - Entry point (maps to src/main.go)
│   ├── [TODO] window.c          - Window management (maps to system.go window code)
│   ├── [TODO] renderer_gl.c     - OpenGL backend (maps to render_gl.go)
│   ├── [TODO] renderer_vk.c     - Vulkan backend (maps to render_vk.go)
│   ├── [TODO] input.c           - Input handling (maps to input.go)
│   ├── [TODO] lua_bindings.c    - Lua C bindings (maps to system.go Lua functions)
│   ├── [TODO] sff.c             - SFF sprite loader (maps to image.go)
│   ├── [TODO] air.c             - AIR animation parser (maps to anim.go)
│   ├── [TODO] cns.c             - CNS state parser (maps to compiler.go)
│   ├── [TODO] char.c            - Character system (maps to char.go)
│   ├── [TODO] stage.c           - Stage system (maps to stage.go)
│   ├── [TODO] audio.c           - Audio system (maps to sound.go)
│   └── [TODO] gameloop.c        - Game loop (maps to system.go update/draw)
└── docs/
    ├── ROADMAP.md               - Development phases
    ├── CONTROL_FLOW_GRAPH.md   - Detailed CFG from Go source
    └── GO_TO_C11_MAPPING.md    - This file
```

### 8. Variable Naming Conventions

| Go Convention | C11 Convention | Example |
|---------------|----------------|---------|
| camelCase | snake_case | `gameWidth` → `game_width` |
| PascalCase (exported) | snake_case with prefix | `System` → `ikm_system_t` |
| map[string]X | Dynamic array + search | `cmdFlags map[string]string` → `char** flag_names`, `char** flag_values` |
| []Type slice | Dynamic array | `chars []*Char` → `ikm_char_t** chars`, `size_t char_count` |
| defer cleanup | Manual at exit | `defer f.Close()` → `fclose(f);` before return |

### 9. Build System Comparison

**Go (Makefile + build/build.sh):**
```bash
go build -o Ikemen_GO ./src
```

**C11 (CMake):**
```bash
cd c11/build
cmake ..
cmake --build .
./bin/ikemen_c11
```

### 10. Testing Comparison

**Go:**
- Limited testing infrastructure in main repo
- Integration tests via Lua scripts

**C11:**
- CTest-based unit tests
- Tests in c11/tests/
- Run with: `ctest --output-on-failure`

## Summary

- **Implemented**: Configuration, INI parsing, directories, logging, error handling
- **Next Priority**: Lua integration (CRITICAL - engine won't run without it)
- **Then**: Window, rendering, input (HIGH - needed for graphics)
- **Finally**: Asset loaders, game loop, character system (MEDIUM)

See [CONTROL_FLOW_GRAPH.md](CONTROL_FLOW_GRAPH.md) for detailed flow analysis.
See [ROADMAP.md](ROADMAP.md) for implementation phases.
