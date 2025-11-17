/*
 * Ikemen C11 Engine - Public API Header
 * Complete implementation of Ikemen-GO functionality in C11
 */

#ifndef IKEMEN_ENGINE_H
#define IKEMEN_ENGINE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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
bool ikm_ini_get_bool(ikm_ini_file_t* ini, const char* section, const char* key, bool default_value);
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

/* === Image System (image.go) === */

/* PalFX - Palette Effects */
typedef struct ikm_palfx ikm_palfx_t;

ikm_palfx_t* ikm_palfx_create(void);
void ikm_palfx_destroy(ikm_palfx_t* pf);
void ikm_palfx_clear(ikm_palfx_t* pf, bool allow_neg);

/* Sprite */
typedef struct ikm_sprite ikm_sprite_t;

ikm_sprite_t* ikm_sprite_create(void);
void ikm_sprite_destroy(ikm_sprite_t* s);
bool ikm_sprite_is_blank(const ikm_sprite_t* s);
void ikm_sprite_get_size(const ikm_sprite_t* s, uint16_t* w, uint16_t* h);
void ikm_sprite_get_offset(const ikm_sprite_t* s, int16_t* x, int16_t* y);
void ikm_sprite_get_group_number(const ikm_sprite_t* s, int16_t* g, int16_t* n);

/* SFF - Sprite File Format */
typedef struct ikm_sff ikm_sff_t;

ikm_sff_t* ikm_sff_create(void);
void ikm_sff_destroy(ikm_sff_t* sff);
ikm_sff_t* ikm_sff_load(const char* filename);
ikm_sprite_t* ikm_sff_get_sprite(ikm_sff_t* sff, int16_t group, int16_t number);

/* Palette Management */
typedef struct ikm_palette_list ikm_palette_list_t;

ikm_palette_list_t* ikm_palette_list_create(void);
void ikm_palette_list_destroy(ikm_palette_list_t* pl);
int ikm_palette_list_add(ikm_palette_list_t* pl, const uint32_t* pal, size_t size);

/* === Animation System (anim.go) === */

/* Animation Frame */
typedef struct ikm_anim_frame ikm_anim_frame_t;

ikm_anim_frame_t* ikm_anim_frame_create(void);
void ikm_anim_frame_destroy(ikm_anim_frame_t* af);
ikm_anim_frame_t* ikm_anim_frame_parse(const char* line);

/* Animation */
typedef struct ikm_animation ikm_animation_t;

ikm_animation_t* ikm_animation_create(ikm_sff_t* sff, ikm_palette_list_t* pal);
void ikm_animation_destroy(ikm_animation_t* anim);
int ikm_animation_add_frame(ikm_animation_t* anim, ikm_anim_frame_t* frame);
void ikm_animation_reset(ikm_animation_t* anim);
void ikm_animation_update(ikm_animation_t* anim);
ikm_sprite_t* ikm_animation_get_sprite(const ikm_animation_t* anim);
const ikm_anim_frame_t* ikm_animation_get_frame(const ikm_animation_t* anim);
bool ikm_animation_has_looped(const ikm_animation_t* anim);

/* AIR - Animation File Format */
typedef struct ikm_air_file ikm_air_file_t;

ikm_air_file_t* ikm_air_create(ikm_sff_t* sff, ikm_palette_list_t* pal);
void ikm_air_destroy(ikm_air_file_t* air);
ikm_air_file_t* ikm_air_load(const char* filename, ikm_sff_t* sff, ikm_palette_list_t* pal);
ikm_animation_t* ikm_air_get_animation(ikm_air_file_t* air, int32_t action_no);

#endif /* IKEMEN_ENGINE_H */
