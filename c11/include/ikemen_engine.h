/*
 * Ikemen C11 Engine - Public API Header
 * 
 * This is Phase 1 (Foundation) of the C11 port of Ikemen-GO, a fighting game
 * engine compatible with M.U.G.E.N resources.
 * 
 * The Go reference implementation (src/main.go, src/system.go) is a complete
 * fighting game engine with:
 * - Graphics rendering (OpenGL/Vulkan)
 * - Lua scripting for game logic
 * - Character/stage management with MUGEN file format support (SFF, AIR, CNS)
 * - Audio system (BGM, sound effects)
 * - Input handling and netplay
 * - Full match system with game modes
 * 
 * Current C11 Status (v0.1.0):
 * This initial implementation provides:
 * - Version identification
 * - Asset directory validation (data/, external/, font/)
 * - Foundation for future engine components
 * 
 * Next phases will add:
 * - Configuration file parsing (INI format)
 * - MUGEN file format support (SFF sprites, AIR animations, CNS states)
 * - Rendering subsystem
 * - Game loop and match management
 * - Full feature parity with Go implementation
 * 
 * See c11/docs/ROADMAP.md for the complete development plan.
 */

#ifndef IKEMEN_ENGINE_H
#define IKEMEN_ENGINE_H

/* Version information */
#define IKM_ENGINE_MAJOR 0
#define IKM_ENGINE_MINOR 1
#define IKM_ENGINE_PATCH 0

/* Return codes */
#define IKM_SUCCESS 0
#define IKM_ERROR_FILE_NOT_FOUND 1
#define IKM_ERROR_INVALID_PATH 2
#define IKM_ERROR_PERMISSION_DENIED 3
#define IKM_ERROR_UNKNOWN 99

/*
 * Get the major version number of the Ikemen C11 engine.
 * 
 * Returns: Major version number (currently 0 for development)
 */
int ikm_engine_major_version(void);

/*
 * Get the minor version number of the Ikemen C11 engine.
 * 
 * Returns: Minor version number
 */
int ikm_engine_minor_version(void);

/*
 * Get the patch version number of the Ikemen C11 engine.
 * 
 * Returns: Patch version number
 */
int ikm_engine_patch_version(void);

/*
 * Get the full version string of the Ikemen C11 engine.
 * 
 * Returns: Pointer to a static string containing the version (e.g., "0.1.0")
 *          Do not free this string.
 */
const char* ikm_engine_version_string(void);

/*
 * Check compatibility with the asset directory structure expected by the
 * Go reference implementation. This function verifies that essential
 * directories exist and are accessible.
 * 
 * The following directories are checked relative to base_path:
 * - data/          (Common data files: common.cmd, system.base.def, etc.)
 * - external/      (External resources: icons, mods, script, shaders)
 * - font/          (Font files)
 * 
 * Parameters:
 *   base_path - Path to the engine root directory (e.g., "." for current dir)
 *               If NULL, defaults to "."
 * 
 * Returns:
 *   IKM_SUCCESS              - All required directories found and accessible
 *   IKM_ERROR_INVALID_PATH   - base_path is invalid or inaccessible
 *   IKM_ERROR_FILE_NOT_FOUND - One or more required directories are missing
 *   IKM_ERROR_PERMISSION_DENIED - Permission denied accessing directories
 */
int ikm_check_asset_compatibility(const char* base_path);

/*
 * Get a human-readable error message for the last asset compatibility check.
 * This provides details about which specific directory or file caused a failure.
 * 
 * Returns: Pointer to a static string containing the error message.
 *          Returns empty string if no error or after successful check.
 *          Do not free this string.
 */
const char* ikm_get_last_error(void);

#endif /* IKEMEN_ENGINE_H */
