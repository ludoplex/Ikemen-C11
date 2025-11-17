/*
 * Ikemen C11 Engine - Main Entry Point
 * 
 * Phase 1 (Foundation) entry point for the Ikemen-GO C11 port.
 * 
 * This is NOT yet a full game engine. It's the foundation layer that validates
 * the environment is ready for future development phases.
 * 
 * The Go engine (src/main.go) does much more at startup:
 * - Creates save/ directories for replays, logs, stats
 * - Parses command-line arguments (-windowed, -p1, -stage, etc.)
 * - Loads config.ini with game settings
 * - Verifies the main Lua script exists (system.def)
 * - Calls sys.init() which:
 *   * Creates graphics window (OpenGL/Vulkan)
 *   * Initializes input system and joysticks
 *   * Loads shaders
 *   * Sets up rendering subsystem
 *   * Initializes audio (speaker, mixer, BGM)
 * - Executes the Lua game loop (sys.luaLState.DoFile)
 * - Handles errors with logs and error dialogs
 * 
 * The C11 port will eventually replicate all of this. For now (Phase 1),
 * we just validate that the asset directories exist so future phases
 * can proceed with loading actual game data.
 * 
 * Planned phases (see c11/docs/ROADMAP.md):
 * - Phase 2: Config file parsing (INI format)
 * - Phase 3: Sprite/animation loading (SFF, AIR)
 * - Phase 4: Character state system (CNS, CMD)
 * - Phase 5: Basic rendering
 * - Phase 6: Physics and collision
 * - Phase 7: Full game logic and Lua integration
 * 
 * Usage: ikemen_c11 [base_path]
 *   base_path: Path to engine root (defaults to current directory)
 * 
 * Exit codes:
 *   0 - Success: Asset structure valid, ready for future development
 *   1 - Error: Missing required directories
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    const char* base_path = NULL;
    int result;
    
    /* Print engine information */
    printf("Ikemen C11 Engine (Phase 1 - Foundation)\n");
    printf("Version: %s\n", ikm_engine_version_string());
    printf("Build: C11 Standard (ISO/IEC 9899:2011)\n");
    printf("\n");
    printf("NOTE: This is an early foundation build.\n");
    printf("The full Ikemen-GO engine (src/main.go) includes:\n");
    printf("  - Graphics window and rendering (OpenGL/Vulkan)\n");
    printf("  - Lua scripting engine for game logic\n");
    printf("  - MUGEN asset loading (SFF sprites, AIR animations, CNS states)\n");
    printf("  - Audio system (BGM, sound effects)\n");
    printf("  - Input handling and netplay\n");
    printf("  - Complete match and game mode systems\n");
    printf("\n");
    printf("The C11 port will replicate all of this functionality incrementally.\n");
    printf("Current phase: Asset directory validation.\n");
    printf("See c11/docs/ROADMAP.md for the full development plan.\n");
    printf("\n");
    
    /* Parse command line arguments */
    if (argc > 1) {
        base_path = argv[1];
        printf("Using base path: %s\n", base_path);
    } else {
        printf("Using base path: . (current directory)\n");
    }
    printf("\n");
    
    /* Perform asset compatibility check */
    printf("Performing Phase 1 asset structure validation...\n");
    printf("Checking for required directories:\n");
    printf("  - data/      (Common data files: common.cmd, system.def, etc.)\n");
    printf("  - external/  (External resources: shaders, scripts, icons)\n");
    printf("  - font/      (Font files)\n");
    printf("\n");
    
    result = ikm_check_asset_compatibility(base_path);
    
    if (result == IKM_SUCCESS) {
        printf("✓ Asset structure validation PASSED\n");
        printf("\n");
        printf("All required directories exist and are accessible.\n");
        printf("The environment is ready for future C11 engine development.\n");
        printf("\n");
        printf("Details: %s\n", ikm_get_last_error());
        printf("\n");
        printf("Next development phase will add:\n");
        printf("  - Configuration file parsing (config.ini, system.def)\n");
        printf("  - See c11/docs/ROADMAP.md for details\n");
        return EXIT_SUCCESS;
    } else {
        printf("✗ Asset structure validation FAILED\n");
        printf("\n");
        
        /* Print specific error information */
        switch (result) {
            case IKM_ERROR_FILE_NOT_FOUND:
                printf("Error type: Missing required directory\n");
                break;
            case IKM_ERROR_INVALID_PATH:
                printf("Error type: Invalid path\n");
                break;
            case IKM_ERROR_PERMISSION_DENIED:
                printf("Error type: Permission denied\n");
                break;
            default:
                printf("Error type: Unknown error (code %d)\n", result);
                break;
        }
        
        printf("Details: %s\n", ikm_get_last_error());
        printf("\n");
        printf("The C11 engine requires the same asset directory structure\n");
        printf("as the Go reference implementation. These directories should\n");
        printf("contain the same data used by Ikemen-GO:\n");
        printf("  - data/: common.cmd, system.def, *.zss files\n");
        printf("  - external/: shaders, scripts, icons subdirectories\n");
        printf("  - font/: font definition files\n");
        printf("\n");
        printf("If running from the repository root, these directories should exist.\n");
        printf("If running from elsewhere, specify the correct base path as an argument.\n");
        
        return EXIT_FAILURE;
    }
}
