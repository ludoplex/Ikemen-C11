# Ikemen-GO Control Flow Graph Analysis

This document provides a detailed control flow graph (CFG) extracted from the Go source code to inform the C11 implementation.

## Main Function Flow (src/main.go:53-138)

```
main()
  ├─> [1] Executable Path & macOS Bundle Detection
  │     - Get executable path via os.Executable()
  │     - If macOS app bundle: chdir to bundle root (../../../)
  │
  ├─> [2] Create Save Directories
  │     - os.Mkdir("save", 0755)
  │     - os.Mkdir("save/replays", 0755)
  │     - os.Mkdir("save/logs", 0755)
  │
  ├─> [3] Process Command Line
  │     - processCommandLine()
  │     - Parses: -config, -windowed, -p1, -p2, -stage, -r, -log, etc.
  │     - Stores in sys.cmdFlags map[string]string
  │
  ├─> [4] Initialize Stats File
  │     - Read "save/stats.json"
  │     - If error: Create with "{}"
  │
  ├─> [5] Load Configuration
  │     - Default: "save/config.ini"
  │     - Override with -config flag
  │     - loadConfig(cfgPath) -> Config struct
  │     - Store in sys.cfg
  │     - On error: panic with chk()
  │
  ├─> [6] Verify Main Lua Script
  │     - Open sys.cfg.Config.System (default: "external/script/main.lua")
  │     - On error: ShowErrorDialog() + panic
  │     - Close file (verification only)
  │
  ├─> [7] Initialize Engine
  │     - sys.init(sys.gameWidth, sys.gameHeight)
  │     - Returns *lua.LState
  │     - defer sys.shutdown()
  │
  └─> [8] Execute Main Script
        - sys.luaLState.DoFile(sys.cfg.Config.System)
        - On error:
            - Create "Ikemen.log"
            - Write version, build time, error
            - Check if lua.ApiError with "<game end>": Normal exit
            - Else: ShowErrorDialog() + panic
```

## sys.init() Function Flow (src/system.go:344-510)

```
sys.init(w int32, h int32) *lua.LState
  ├─> [1] Set Game Size
  │     - s.setGameSize(w, h)
  │     - Sets gameWidth, gameHeight, widthScale, heightScale
  │
  ├─> [2] Initialize Character Global Info
  │     - For i in cgi[]: Initialize palInfo map
  │
  ├─> [3] Load Localcoords
  │     - s.loadLocalcoords()
  │     - Parses data/system.base.def for coordinate systems
  │
  ├─> [4] Set Motif Scale
  │     - s.setMotifScale()
  │     - Calculate scaling for screenpack
  │
  ├─> [5] Create Window
  │     - s.newWindow(scrrect[2], scrrect[3])
  │     - Platform-specific (GLFW wrapper)
  │     - Store in s.window
  │     - On error: chk() panic
  │
  ├─> [6] Update Gamepad Mappings
  │     - input.UpdateGamepadMappings(cfg.GamepadMappings)
  │
  ├─> [7] Correct Joystick Mappings (macOS)
  │     - If runtime.GOOS == "darwin":
  │         - Match joysticks by GUID
  │         - Swap mismatched configs
  │
  ├─> [8] Load External Shaders
  │     - If cfg.Video.ExternalShaders:
  │         - For each shader location:
  │             - If RenderMode != "Vulkan 1.3":
  │                 - Read .vert and .frag files
  │             - Else:
  │                 - Read .vert.spv and .frag.spv files
  │         - Store in s.externalShaders[2][]byte
  │         - On error: chk() panic
  │
  ├─> [9] Initialize Rendering Backend
  │     - Switch cfg.Video.RenderMode:
  │         case "Vulkan 1.3":
  │           gfx = &Renderer_VK{}
  │           gfxFont = &FontRenderer_VK{}
  │         case "OpenGL 2.1":
  │           gfx = &Renderer_GL21{}
  │           gfxFont = &FontRenderer_GL21{}
  │         default ("OpenGL 3.2"):
  │           gfx = &Renderer_GL32{}
  │           gfxFont = &FontRenderer_GL32{}
  │     - gfx.Init()
  │     - gfxFont.Init()
  │
  ├─> [10] Initialize Audio System
  │     - sampleRate := beep.SampleRate(frequency)
  │     - bufferSize := sampleRate.N(time.Second / 60)
  │     - speaker.Init(sampleRate, bufferSize)
  │     - speaker.Play(s.soundMixer)
  │     - On error: Set nomusic = true (continue without audio)
  │
  ├─> [11] Create Debug Font
  │     - s.debugFont = NewTextSprite()
  │     - Used for on-screen debug text
  │
  ├─> [12] Setup Input System
  │     - input.Init(s.window)
  │     - Configure keyboard/joystick bindings
  │
  ├─> [13] Initialize Lua State
  │     - L := lua.NewState()
  │     - Register 100+ engine functions:
  │         - File I/O functions
  │         - Character management
  │         - Stage functions
  │         - System queries
  │         - Rendering functions
  │         - Audio functions
  │     - Return L
  │
  └─> Return: *lua.LState ready for DoFile()
```

## Lua-Driven Game Loop (external/script/main.lua)

```
main.lua execution
  ├─> Load Dependencies
  │     - require("global")
  │     - require("menu")
  │     - require("motif")
  │     - require("start")
  │     - require("options")
  │
  ├─> Initialize Systems
  │     - loadMotif() -- Parse screenpack
  │     - setLanguage()
  │     - loadStart() -- Character select screen
  │
  └─> Main Event Loop
        │
        while true do
          │
          ├─> [Menu State]
          │     - Display menu
          │     - Process input
          │     - commandGetState()
          │     - Update UI animations
          │     - If start pressed: goto Fight
          │
          ├─> [Select State]
          │     - Character selection
          │     - Stage selection
          │     - Team configuration
          │     - Load character defs
          │     - If done: goto Fight
          │
          ├─> [Fight State]
          │     - Load stage
          │     - Load characters
          │     - Initialize round
          │     │
          │     └─> Fight Loop:
          │           while not match_end do
          │             - sys.update() -- C function
          │             - sys.draw()   -- C function
          │             - If round_end:
          │                 - Update stats
          │                 - Check match end
          │                 - Reset for next round
          │           end
          │
          ├─> [Results State]
          │     - Display results
          │     - Update save/stats.json
          │     - Return to menu
          │
          └─> Check escape: If pressed, exit loop
        end
```

## sys.update() Control Flow (src/system.go:912-941)

```
sys.update() bool
  ├─> [1] Event Update
  │     - s.eventUpdate()
  │         - Poll GLFW events
  │         - Check window close
  │         - Process keyboard/mouse
  │     - If quit requested: return false
  │
  ├─> [2] Main Thread Tasks
  │     - s.runMainThreadTask()
  │         - Execute queued thread-safe tasks
  │
  ├─> [3] Frame Timing
  │     - s.await(FPS)
  │         - Calculate delta time
  │         - Sleep if ahead of schedule
  │         - Return skip flag if behind
  │     - If skip: return true (drop frame)
  │
  ├─> [4] Update Input
  │     - input.Update()
  │         - Read keyboard state
  │         - Read joystick state
  │         - Process input buffer
  │
  ├─> [5] Tick Sound
  │     - s.tickSound()
  │         - Update sound channels
  │         - Process BGM
  │
  ├─> [6] Tick Frame Logic
  │     - If paused:
  │         - Check frame step
  │         - Skip if not stepping
  │     - s.tickFrame()
  │         - Increment tick counter
  │         - s.charUpdate() -- Update all characters
  │         - s.globalCollision() -- Check collisions
  │         - s.explodUpdate() -- Update effects
  │         - s.globalTick() -- Update game state
  │
  └─> return true (continue game loop)
```

## sys.draw() Control Flow (src/system.go:2348-2481)

```
sys.draw(x float32, y float32, scl float32)
  ├─> [1] Setup Drawing
  │     - Calculate camera position
  │     - Set viewport
  │
  ├─> [2] Clear Screen
  │     - gfx.Clear(r, g, b)
  │
  ├─> [3] Draw Stage Background
  │     - If s.stage != nil:
  │         - s.stage.draw(x, y, scl)
  │           - Draw background layers
  │           - Apply parallax
  │
  ├─> [4] Draw Shadows
  │     - s.shadows.draw()
  │     - Pre-render shadows below characters
  │
  ├─> [5] Draw Sprites (Layered)
  │     - s.spritesLayerN1.draw() -- Layer -1 (back)
  │     - s.spritesLayer0.draw()  -- Layer 0 (main)
  │     - s.spritesLayer1.draw()  -- Layer 1 (front)
  │     - s.spritesLayerU.draw()  -- Layer U (UI)
  │
  ├─> [6] Draw Reflections
  │     - s.reflections.draw()
  │
  ├─> [7] Draw Stage Foreground
  │     - If s.stage != nil:
  │         - s.stage.drawForeground()
  │
  ├─> [8] Draw Projectiles
  │     - For each player:
  │         - For each proj in s.projs[playerNo]:
  │             - proj.draw()
  │
  ├─> [9] Draw Explods
  │     - For each player:
  │         - For each explod in s.explods[playerNo]:
  │             - explod.draw()
  │
  ├─> [10] Draw Lifebar/HUD
  │     - If not s.lifebarHide:
  │         - s.lifebar.draw()
  │           - Health bars
  │           - Power bars
  │           - Time
  │           - Round indicators
  │
  ├─> [11] Draw Debug Info
  │     - If s.debugDisplay:
  │         - s.drawDebugText()
  │         - Draw FPS counter
  │         - Draw character state info
  │     - If s.clsnDisplay:
  │         - Draw collision boxes
  │
  └─> [12] Swap Buffers
        - s.window.SwapBuffers()
        - Present frame to screen
```

## Key Data Structures

### System Struct (200+ fields)
```go
type System struct {
    // Core state
    randseed                int32
    gameWidth, gameHeight   int32
    window                  *Window
    gameEnd, frameSkip      bool
    
    // Graphics
    scrrect                 [4]int32
    widthScale, heightScale float32
    brightness              float32
    
    // Configuration
    cfg                     Config
    
    // Game state
    round                   int32
    match                   int32
    matchTime               int32
    curRoundTime            int32
    winTeam                 int
    
    // Characters
    chars                   [MaxPlayerNo][]*Char
    charList                CharList
    cgi                     [MaxPlayerNo]CharGlobalInfo
    
    // Stage
    stage                   *Stage
    stageList               map[int32]*Stage
    
    // Audio
    soundMixer              *beep.Mixer
    bgm                     Bgm
    soundChannels           *SoundChannels
    
    // Lua
    luaLState               *lua.LState
    luaStringVars           map[string]string
    luaNumVars              map[string]float32
    
    // Input
    keyConfig               []KeyConfig
    joystickConfig          []KeyConfig
    aiInput                 [MaxPlayerNo]AiInput
    
    // Rendering
    spritesLayerN1          DrawList
    spritesLayer0           DrawList
    spritesLayer1           DrawList
    spritesLayerU           DrawList
    shadows                 ShadowList
    reflections             ReflectionList
    
    // Effects
    projs                   [MaxPlayerNo][]*Projectile
    explods                 [MaxPlayerNo][]*Explod
    allPalFX                *PalFX
    bgPalFX                 *PalFX
    
    // UI
    lifebar                 Lifebar
    debugFont               *TextSprite
    
    // Timing
    tickCount               int
    tickCountF              float32
    nextAddTime             float32
    
    // ... 150+ more fields
}
```

### Config Struct
```go
type Config struct {
    // Files
    Common struct {
        Air     map[string][]string
        Cmd     map[string][]string
        Const   map[string][]string
        States  map[string][]string
    }
    
    // Video settings
    Video struct {
        Width         int
        Height        int
        Fullscreen    bool
        RenderMode    string
        MSAA          int32
        ExternalShaders []string
    }
    
    // Audio settings
    Sound struct {
        MasterVolume  int
        BGMVolume     int
        WavVolume     int
        Freq          int
    }
    
    // System
    Config struct {
        System string // Path to main.lua
        Motif  string
    }
    
    // Gameplay
    Options struct {
        Difficulty    int
        Life          float32
        Time          int32
        GameSpeed     int
    }
    
    // Input
    Keys []*KeyConfig
    Joystick []*KeyConfig
}
```

## Critical Control Flow Patterns

### 1. Error Handling Pattern
```
Go Pattern:
  result, err := operation()
  if err != nil {
      chk(err)  // -> panic()
  }

C11 Equivalent:
  int result = operation();
  if (result != IKM_SUCCESS) {
      ikm_log(IKM_LOG_FATAL, "Error: %s", ikm_get_last_error());
      cleanup();
      return EXIT_FAILURE;
  }
```

### 2. Deferred Cleanup Pattern
```
Go Pattern:
  resource := acquire()
  defer release(resource)
  // ... use resource ...

C11 Equivalent:
  Resource* resource = acquire();
  if (!resource) return error;
  // ... use resource ...
  release(resource);  // Manual at function exit
```

### 3. Lua-C Interaction Pattern
```
Go Pattern:
  L.SetGlobal("functionName", L.NewFunction(cFunction))
  L.DoFile("script.lua")
  // Lua calls cFunction

C11 Equivalent:
  lua_pushcfunction(L, cFunction);
  lua_setglobal(L, "functionName");
  luaL_dofile(L, "script.lua");
  // Lua calls cFunction
```

## Implementation Priority (Based on CFG)

### Phase 1: ✓ Completed
- Directory creation
- INI configuration loading
- Error handling infrastructure
- Logging system

### Phase 2: System Structure (CRITICAL)
- Implement System struct equivalent
- Window abstraction layer
- Renderer interface
- Input abstraction

### Phase 3: Lua Integration (CORE REQUIREMENT)
- Embed Lua 5.1+ interpreter
- Register C functions matching Go's lua bindings
- Execute main.lua script
- **Without Lua, the engine cannot run**

### Phase 4: Asset Loading
- SFF sprite file parser
- AIR animation file parser
- CNS state definition parser
- DEF screenpack parser

### Phase 5: Game Loop
- Event system
- Frame timing (60 FPS)
- update() function
- draw() function

### Phase 6: Character System
- Character struct
- State machine (CNS)
- Physics engine
- Collision detection

### Phase 7: Complete Engine
- Audio system (BGM, SFX)
- Network play
- Replay system
- All game modes

## Key Observations for C11

1. **Lua is Not Optional**: The entire game flow is driven by Lua scripts. C11 must embed a Lua interpreter.

2. **Single Global State**: Go uses a global `sys` variable. C11 should use a similar singleton pattern.

3. **Lazy Initialization**: Many systems init on first use. C11 should init everything in sys_init().

4. **Dynamic Arrays Everywhere**: Go slices are used extensively. C11 needs dynamic array implementations.

5. **Interface-Based Rendering**: Renderer is abstracted. C11 should use function pointers for backend-agnostic rendering.

6. **Frame-Based Logic**: Everything updates on 60 FPS ticks. C11 needs precise frame timing.

7. **Asset Path Fallbacks**: Files searched in multiple locations. C11 needs path resolution logic.

8. **No Multithreading**: Main logic is single-threaded (audio runs separate). C11 should match this.

9. **Error Recovery**: Go panics on fatal errors. C11 should log and exit cleanly.

10. **Configuration-Driven**: Everything configurable via INI files. C11 parser is critical.
