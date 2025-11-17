/*
 * Ikemen C11 Engine - Public API Header
 * Complete implementation of Ikemen-GO functionality in C11
 */

#ifndef IKEMEN_ENGINE_H
#define IKEMEN_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#define IKM_ENGINE_MAJOR 0
#define IKM_ENGINE_MINOR 1
#define IKM_ENGINE_PATCH 0
#define IKM_ENGINE_NAME "Ikemen-C11"

#define IKM_SUCCESS 0
#define IKM_ERROR_FILE_NOT_FOUND 1
#define IKM_ERROR_INVALID_PATH 2
#define IKM_ERROR_PERMISSION_DENIED 3
#define IKM_ERROR_IO 4
#define IKM_ERROR_PARSE 5
#define IKM_ERROR_MEMORY 6
#define IKM_ERROR_INVALID_CONFIG 7
#define IKM_ERROR_UNKNOWN 99

#define IKM_MAX_PATH 512
#define IKM_MAX_KEY_LENGTH 128
#define IKM_MAX_VALUE_LENGTH 512
#define IKM_MAX_SECTION_LENGTH 64

/* Version information */
int ikm_engine_major_version(void);
int ikm_engine_minor_version(void);
int ikm_engine_patch_version(void);
const char* ikm_engine_version_string(void);
const char* ikm_engine_name(void);

/* INI configuration structures */
typedef struct {
    char key[IKM_MAX_KEY_LENGTH];
    char value[IKM_MAX_VALUE_LENGTH];
} ikm_ini_entry_t;

typedef struct {
    char name[IKM_MAX_SECTION_LENGTH];
    ikm_ini_entry_t* entries;
    size_t entry_count;
    size_t entry_capacity;
} ikm_ini_section_t;

typedef struct {
    ikm_ini_section_t* sections;
    size_t section_count;
    size_t section_capacity;
    char filepath[IKM_MAX_PATH];
} ikm_ini_file_t;

/* INI file operations */
ikm_ini_file_t* ikm_ini_load(const char* filepath);
void ikm_ini_free(ikm_ini_file_t* ini);
const char* ikm_ini_get(ikm_ini_file_t* ini, const char* section, const char* key, const char* default_value);
int ikm_ini_get_int(ikm_ini_file_t* ini, const char* section, const char* key, int default_value);
double ikm_ini_get_double(ikm_ini_file_t* ini, const char* section, const char* key, double default_value);
int ikm_ini_get_bool(ikm_ini_file_t* ini, const char* section, const char* key, int default_value);
int ikm_ini_save(ikm_ini_file_t* ini, const char* filepath);

/* Engine configuration */
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

/* Configuration management */
int ikm_config_load(const char* filepath, ikm_config_t* config);
int ikm_config_save(const char* filepath, const ikm_config_t* config);
void ikm_config_set_defaults(ikm_config_t* config);

/* Engine initialization */
typedef struct ikm_engine ikm_engine_t;


ikm_engine_t* ikm_engine_create(void);
void ikm_engine_destroy(ikm_engine_t* engine);
int ikm_engine_initialize(ikm_engine_t* engine, const ikm_config_t* config);
int ikm_engine_shutdown(ikm_engine_t* engine);

/* Directory and file management */
int ikm_create_directories(const char* base_path);
int ikm_verify_installation(const char* base_path);
int ikm_scan_directory(const char* path, char*** files, size_t* count);
void ikm_free_file_list(char** files, size_t count);
int file_exists(const char* path);
int dir_exists(const char* path);

/* System information */
typedef struct {
    char data_dir[IKM_MAX_PATH];
    char external_dir[IKM_MAX_PATH];
    char font_dir[IKM_MAX_PATH];
    char save_dir[IKM_MAX_PATH];
    char system_def[IKM_MAX_PATH];
    int has_system_def;
    int has_common_files;
    size_t stage_count;
    size_t char_count;
} ikm_system_info_t;

int ikm_get_system_info(const char* base_path, ikm_system_info_t* info);

/* Error handling */
const char* ikm_get_last_error(void);
void ikm_set_error(const char* fmt, ...);
void ikm_clear_error(void);

/* Logging */
typedef enum {
    IKM_LOG_DEBUG,
    IKM_LOG_INFO,
    IKM_LOG_WARNING,
    IKM_LOG_ERROR,
    IKM_LOG_FATAL
} ikm_log_level_t;

void ikm_log(ikm_log_level_t level, const char* format, ...);
void ikm_log_set_level(ikm_log_level_t level);
void ikm_log_set_file(const char* filepath);


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
void ikm_set_error(const char* fmt, ...);


/* Lua integration */
typedef struct ikm_lua_state ikm_lua_state_t;

ikm_lua_state_t* ikm_lua_create(void);
void ikm_lua_destroy(ikm_lua_state_t* state);
int ikm_lua_dofile(ikm_lua_state_t* state, const char* filename);
int ikm_lua_dostring(ikm_lua_state_t* state, const char* code);
void* ikm_lua_get_state(ikm_lua_state_t* state);

/* Window management */
typedef struct ikm_window ikm_window_t;

ikm_window_t* ikm_window_create(const char* title, int width, int height, int fullscreen);
void ikm_window_destroy(ikm_window_t* window);
int ikm_window_should_close(ikm_window_t* window);
void ikm_window_poll_events(ikm_window_t* window);
void ikm_window_swap_buffers(ikm_window_t* window);
void ikm_window_get_size(ikm_window_t* window, int* width, int* height);
void ikm_window_set_title(ikm_window_t* window, const char* title);
void* ikm_window_get_handle(ikm_window_t* window);

/* Input system */
typedef struct ikm_input ikm_input_t;

ikm_input_t* ikm_input_create(ikm_window_t* window);
void ikm_input_destroy(ikm_input_t* input);
void ikm_input_update(ikm_input_t* input);
int ikm_input_key_pressed(ikm_input_t* input, int key);
int ikm_input_key_released(ikm_input_t* input, int key);
int ikm_input_key_down(ikm_input_t* input, int key);
void ikm_input_get_mouse_position(ikm_input_t* input, double* x, double* y);
int ikm_input_mouse_button_pressed(ikm_input_t* input, int button);
int ikm_input_command_state(ikm_input_t* input, const char* command);

/* Renderer system */
typedef struct ikm_renderer ikm_renderer_t;

ikm_renderer_t* ikm_renderer_create(void);
void ikm_renderer_destroy(ikm_renderer_t* renderer);
int ikm_renderer_initialize(ikm_renderer_t* renderer, int width, int height);
void ikm_renderer_begin_frame(ikm_renderer_t* renderer);
void ikm_renderer_end_frame(ikm_renderer_t* renderer);
void ikm_renderer_set_clear_color(ikm_renderer_t* renderer, float r, float g, float b, float a);
void ikm_renderer_set_viewport(ikm_renderer_t* renderer, int x, int y, int width, int height);
void ikm_renderer_draw_quad(ikm_renderer_t* renderer, float x, float y, float w, float h, 
                            float r, float g, float b, float a);
const char* ikm_renderer_get_name(ikm_renderer_t* renderer);

/* Engine subsystem accessors */
ikm_lua_state_t* ikm_engine_get_lua_state(ikm_engine_t* engine);
ikm_window_t* ikm_engine_get_window(ikm_engine_t* engine);
ikm_input_t* ikm_engine_get_input(ikm_engine_t* engine);
ikm_renderer_t* ikm_engine_get_renderer(ikm_engine_t* engine);

#endif /* IKEMEN_ENGINE_H */
