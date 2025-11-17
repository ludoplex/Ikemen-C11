/*
 * Ikemen C11 Engine - Core Implementation
 * Complete fighting game engine implementation
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#define PATH_SEP '\\'
#define strcasecmp _stricmp
#else
#define PATH_SEP '/'
/* Provide strdup if not available */
#ifndef strdup
static char* my_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* new_str = (char*)malloc(len);
    if (new_str) {
        memcpy(new_str, s, len);
    }
    return new_str;
}
#define strdup my_strdup
#endif
#endif

#define MAX_ERROR_LENGTH 1024
#define INITIAL_CAPACITY 16

static char last_error_msg[MAX_ERROR_LENGTH] = "";
static ikm_log_level_t current_log_level = IKM_LOG_INFO;
static FILE* log_file = NULL;

/* Engine state structure */
struct ikm_engine {
    ikm_config_t config;
    ikm_system_info_t system_info;
    int initialized;
    char base_path[IKM_MAX_PATH];
    
    /* Subsystems */
    ikm_lua_state_t* lua_state;
    ikm_window_t* window;
    ikm_input_t* input;
    ikm_renderer_t* renderer;
};

/* Internal helpers */

void ikm_set_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(last_error_msg, MAX_ERROR_LENGTH, fmt, args);
    va_end(args);
}

int file_exists(const char* path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

int dir_exists(const char* path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

static int create_dir(const char* path) {
    if (dir_exists(path)) {
        return 1;
    }
    if (mkdir(path, 0755) != 0 && errno != EEXIST) {
        ikm_set_error("Failed to create directory %s: %s", path, strerror(errno));
        return 0;
    }
    return 1;
}

static void join_path(char* dest, size_t dest_size, const char* base, const char* rel) {
    if (strcmp(base, ".") == 0) {
        snprintf(dest, dest_size, "%s", rel);
    } else {
        snprintf(dest, dest_size, "%s%c%s", base, PATH_SEP, rel);
    }
}

static char* trim_whitespace(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

/* Version functions */

int ikm_engine_major_version(void) {
    return IKM_ENGINE_MAJOR;
}

int ikm_engine_minor_version(void) {
    return IKM_ENGINE_MINOR;
}

int ikm_engine_patch_version(void) {
    return IKM_ENGINE_PATCH;
}

const char* ikm_engine_version_string(void) {
    static char version[32];
    snprintf(version, sizeof(version), "%d.%d.%d", 
             IKM_ENGINE_MAJOR, IKM_ENGINE_MINOR, IKM_ENGINE_PATCH);
    return version;
}

const char* ikm_engine_name(void) {
    return IKM_ENGINE_NAME;
}

/* INI file implementation */

ikm_ini_file_t* ikm_ini_load(const char* filepath) {
    FILE* fp;
    char line[1024];
    ikm_ini_file_t* ini;
    ikm_ini_section_t* current_section = NULL;
    
    fp = fopen(filepath, "r");
    if (!fp) {
        ikm_set_error("Cannot open file: %s", filepath);
        return NULL;
    }
    
    ini = (ikm_ini_file_t*)calloc(1, sizeof(ikm_ini_file_t));
    if (!ini) {
        fclose(fp);
        ikm_set_error("Memory allocation failed");
        return NULL;
    }
    
    strncpy(ini->filepath, filepath, IKM_MAX_PATH - 1);
    ini->section_capacity = INITIAL_CAPACITY;
    ini->sections = (ikm_ini_section_t*)calloc(ini->section_capacity, sizeof(ikm_ini_section_t));
    
    while (fgets(line, sizeof(line), fp)) {
        char* p = trim_whitespace(line);
        
        if (*p == '\0' || *p == ';' || *p == '#') {
            continue;
        }
        
        if (*p == '[') {
            char* end = strchr(p, ']');
            if (end) {
                *end = '\0';
                if (ini->section_count >= ini->section_capacity) {
                    ini->section_capacity *= 2;
                    ini->sections = (ikm_ini_section_t*)realloc(ini->sections, 
                        ini->section_capacity * sizeof(ikm_ini_section_t));
                }
                current_section = &ini->sections[ini->section_count++];
                memset(current_section, 0, sizeof(ikm_ini_section_t));
                strncpy(current_section->name, p + 1, IKM_MAX_SECTION_LENGTH - 1);
                current_section->entry_capacity = INITIAL_CAPACITY;
                current_section->entries = (ikm_ini_entry_t*)calloc(
                    current_section->entry_capacity, sizeof(ikm_ini_entry_t));
            }
            continue;
        }
        
        char* eq = strchr(p, '=');
        if (eq && current_section) {
            *eq = '\0';
            char* key = trim_whitespace(p);
            char* value = trim_whitespace(eq + 1);
            
            if (current_section->entry_count >= current_section->entry_capacity) {
                current_section->entry_capacity *= 2;
                current_section->entries = (ikm_ini_entry_t*)realloc(current_section->entries,
                    current_section->entry_capacity * sizeof(ikm_ini_entry_t));
            }
            
            ikm_ini_entry_t* entry = &current_section->entries[current_section->entry_count++];
            strncpy(entry->key, key, IKM_MAX_KEY_LENGTH - 1);
            strncpy(entry->value, value, IKM_MAX_VALUE_LENGTH - 1);
        }
    }
    
    fclose(fp);
    return ini;
}

void ikm_ini_free(ikm_ini_file_t* ini) {
    if (!ini) return;
    for (size_t i = 0; i < ini->section_count; i++) {
        free(ini->sections[i].entries);
    }
    free(ini->sections);
    free(ini);
}

const char* ikm_ini_get(ikm_ini_file_t* ini, const char* section, const char* key, const char* default_value) {
    if (!ini) return default_value;
    
    for (size_t i = 0; i < ini->section_count; i++) {
        if (strcasecmp(ini->sections[i].name, section) == 0) {
            for (size_t j = 0; j < ini->sections[i].entry_count; j++) {
                if (strcasecmp(ini->sections[i].entries[j].key, key) == 0) {
                    return ini->sections[i].entries[j].value;
                }
            }
        }
    }
    return default_value;
}

int ikm_ini_get_int(ikm_ini_file_t* ini, const char* section, const char* key, int default_value) {
    const char* value = ikm_ini_get(ini, section, key, NULL);
    return value ? atoi(value) : default_value;
}

double ikm_ini_get_double(ikm_ini_file_t* ini, const char* section, const char* key, double default_value) {
    const char* value = ikm_ini_get(ini, section, key, NULL);
    return value ? atof(value) : default_value;
}

int ikm_ini_get_bool(ikm_ini_file_t* ini, const char* section, const char* key, int default_value) {
    const char* value = ikm_ini_get(ini, section, key, NULL);
    if (!value) return default_value;
    return (strcasecmp(value, "true") == 0 || strcasecmp(value, "1") == 0 || strcasecmp(value, "yes") == 0);
}

int ikm_ini_save(ikm_ini_file_t* ini, const char* filepath) {
    FILE* fp = fopen(filepath, "w");
    if (!fp) {
        ikm_set_error("Cannot write file: %s", filepath);
        return IKM_ERROR_IO;
    }
    
    for (size_t i = 0; i < ini->section_count; i++) {
        fprintf(fp, "[%s]\n", ini->sections[i].name);
        for (size_t j = 0; j < ini->sections[i].entry_count; j++) {
            fprintf(fp, "%s = %s\n", 
                ini->sections[i].entries[j].key,
                ini->sections[i].entries[j].value);
        }
        fprintf(fp, "\n");
    }
    
    fclose(fp);
    return IKM_SUCCESS;
}

/* Configuration management */

void ikm_config_set_defaults(ikm_config_t* config) {
    memset(config, 0, sizeof(ikm_config_t));
    config->width = 640;
    config->height = 480;
    config->fullscreen = 0;
    config->gamma = 1.0;
    config->msaa = 0;
    strcpy(config->render_mode, "OpenGL 3.2");
    config->master_volume = 100;
    config->bgm_volume = 100;
    config->sfx_volume = 100;
    config->frequency = 48000;
    config->channels = 2;
    strcpy(config->system_script, "external/script/main.lua");
    strcpy(config->motif, "data/system.def");
    strcpy(config->common_air, "data/common.air");
    strcpy(config->common_cmd, "data/common.cmd");
    config->difficulty = 4;
    config->life_multiplier = 1.0;
    config->time_limit = 99;
    config->rounds_to_win = 2;
    config->debug_mode = 0;
    config->console_enabled = 0;
    config->joystick_enabled = 1;
}

int ikm_config_load(const char* filepath, ikm_config_t* config) {
    ikm_ini_file_t* ini;
    
    ikm_config_set_defaults(config);
    
    ini = ikm_ini_load(filepath);
    if (!ini) {
        return IKM_SUCCESS;
    }
    
    config->width = ikm_ini_get_int(ini, "Video", "GameWidth", config->width);
    config->height = ikm_ini_get_int(ini, "Video", "GameHeight", config->height);
    config->fullscreen = ikm_ini_get_bool(ini, "Video", "Fullscreen", config->fullscreen);
    config->gamma = ikm_ini_get_double(ini, "Video", "Gamma", config->gamma);
    config->msaa = ikm_ini_get_int(ini, "Video", "MSAA", config->msaa);
    
    const char* render = ikm_ini_get(ini, "Video", "RenderMode", NULL);
    if (render) strncpy(config->render_mode, render, sizeof(config->render_mode) - 1);
    
    config->master_volume = ikm_ini_get_int(ini, "Sound", "MasterVolume", config->master_volume);
    config->bgm_volume = ikm_ini_get_int(ini, "Sound", "BGMVolume", config->bgm_volume);
    config->sfx_volume = ikm_ini_get_int(ini, "Sound", "WavVolume", config->sfx_volume);
    config->frequency = ikm_ini_get_int(ini, "Sound", "Freq", config->frequency);
    config->channels = ikm_ini_get_int(ini, "Sound", "Channels", config->channels);
    
    const char* system = ikm_ini_get(ini, "Config", "System", NULL);
    if (system) strncpy(config->system_script, system, IKM_MAX_PATH - 1);
    
    const char* motif = ikm_ini_get(ini, "Config", "Motif", NULL);
    if (motif) strncpy(config->motif, motif, IKM_MAX_PATH - 1);
    
    config->difficulty = ikm_ini_get_int(ini, "Options", "Difficulty", config->difficulty);
    config->life_multiplier = ikm_ini_get_double(ini, "Options", "Life", config->life_multiplier);
    config->time_limit = ikm_ini_get_int(ini, "Options", "Time", config->time_limit);
    config->rounds_to_win = ikm_ini_get_int(ini, "Options", "RoundsToWin", config->rounds_to_win);
    
    config->debug_mode = ikm_ini_get_bool(ini, "Config", "Debug", config->debug_mode);
    config->joystick_enabled = ikm_ini_get_bool(ini, "Input", "JoystickEnabled", config->joystick_enabled);
    
    ikm_ini_free(ini);
    return IKM_SUCCESS;
}

int ikm_config_save(const char* filepath, const ikm_config_t* config) {
    FILE* fp = fopen(filepath, "w");
    if (!fp) {
        ikm_set_error("Cannot write config file: %s", filepath);
        return IKM_ERROR_IO;
    }
    
    fprintf(fp, "[Video]\n");
    fprintf(fp, "GameWidth = %d\n", config->width);
    fprintf(fp, "GameHeight = %d\n", config->height);
    fprintf(fp, "Fullscreen = %d\n", config->fullscreen);
    fprintf(fp, "Gamma = %.2f\n", config->gamma);
    fprintf(fp, "MSAA = %d\n", config->msaa);
    fprintf(fp, "RenderMode = %s\n\n", config->render_mode);
    
    fprintf(fp, "[Sound]\n");
    fprintf(fp, "MasterVolume = %d\n", config->master_volume);
    fprintf(fp, "BGMVolume = %d\n", config->bgm_volume);
    fprintf(fp, "WavVolume = %d\n", config->sfx_volume);
    fprintf(fp, "Freq = %d\n", config->frequency);
    fprintf(fp, "Channels = %d\n\n", config->channels);
    
    fprintf(fp, "[Config]\n");
    fprintf(fp, "System = %s\n", config->system_script);
    fprintf(fp, "Motif = %s\n", config->motif);
    fprintf(fp, "Debug = %d\n\n", config->debug_mode);
    
    fprintf(fp, "[Options]\n");
    fprintf(fp, "Difficulty = %d\n", config->difficulty);
    fprintf(fp, "Life = %.2f\n", config->life_multiplier);
    fprintf(fp, "Time = %d\n", config->time_limit);
    fprintf(fp, "RoundsToWin = %d\n\n", config->rounds_to_win);
    
    fprintf(fp, "[Input]\n");
    fprintf(fp, "JoystickEnabled = %d\n", config->joystick_enabled);
    
    fclose(fp);
    return IKM_SUCCESS;
}

/* Directory management */

int ikm_create_directories(const char* base_path) {
    char path[IKM_MAX_PATH];
    const char* base = base_path ? base_path : ".";
    
    join_path(path, sizeof(path), base, "save");
    if (!create_dir(path)) return IKM_ERROR_IO;
    
    join_path(path, sizeof(path), base, "save/replays");
    if (!create_dir(path)) return IKM_ERROR_IO;
    
    join_path(path, sizeof(path), base, "save/logs");
    if (!create_dir(path)) return IKM_ERROR_IO;
    
    return IKM_SUCCESS;
}

int ikm_verify_installation(const char* base_path) {
    char path[IKM_MAX_PATH];
    const char* base = base_path ? base_path : ".";
    
    join_path(path, sizeof(path), base, "data");
    if (!dir_exists(path)) {
        ikm_set_error("Missing required directory: data/");
        return IKM_ERROR_FILE_NOT_FOUND;
    }
    
    join_path(path, sizeof(path), base, "external");
    if (!dir_exists(path)) {
        ikm_set_error("Missing required directory: external/");
        return IKM_ERROR_FILE_NOT_FOUND;
    }
    
    join_path(path, sizeof(path), base, "font");
    if (!dir_exists(path)) {
        ikm_set_error("Missing required directory: font/");
        return IKM_ERROR_FILE_NOT_FOUND;
    }
    
    join_path(path, sizeof(path), base, "data/system.base.def");
    if (!file_exists(path)) {
        ikm_set_error("Missing system definition file: data/system.base.def");
        return IKM_ERROR_FILE_NOT_FOUND;
    }
    
    return IKM_SUCCESS;
}

int ikm_scan_directory(const char* path, char*** files, size_t* count) {
    DIR* dir;
    struct dirent* entry;
    size_t capacity = INITIAL_CAPACITY;
    
    *count = 0;
    *files = (char**)calloc(capacity, sizeof(char*));
    if (!*files) {
        ikm_set_error("Memory allocation failed");
        return IKM_ERROR_MEMORY;
    }
    
    dir = opendir(path);
    if (!dir) {
        free(*files);
        *files = NULL;
        ikm_set_error("Cannot open directory: %s", path);
        return IKM_ERROR_IO;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        if (*count >= capacity) {
            capacity *= 2;
            *files = (char**)realloc(*files, capacity * sizeof(char*));
        }
        
        (*files)[*count] = strdup(entry->d_name);
        (*count)++;
    }
    
    closedir(dir);
    return IKM_SUCCESS;
}

void ikm_free_file_list(char** files, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free(files[i]);
    }
    free(files);
}

/* System information */

int ikm_get_system_info(const char* base_path, ikm_system_info_t* info) {
    const char* base = base_path ? base_path : ".";
    memset(info, 0, sizeof(ikm_system_info_t));
    
    join_path(info->data_dir, sizeof(info->data_dir), base, "data");
    join_path(info->external_dir, sizeof(info->external_dir), base, "external");
    join_path(info->font_dir, sizeof(info->font_dir), base, "font");
    join_path(info->save_dir, sizeof(info->save_dir), base, "save");
    join_path(info->system_def, sizeof(info->system_def), base, "data/system.base.def");
    
    info->has_system_def = file_exists(info->system_def);
    info->has_common_files = file_exists("data/common.cmd") && file_exists("data/common.air");
    
    return IKM_SUCCESS;
}

/* Engine management */

ikm_engine_t* ikm_engine_create(void) {
    ikm_engine_t* engine = (ikm_engine_t*)calloc(1, sizeof(ikm_engine_t));
    if (!engine) {
        ikm_set_error("Memory allocation failed");
        return NULL;
    }
    return engine;
}

void ikm_engine_destroy(ikm_engine_t* engine) {
    if (engine) {
        ikm_engine_shutdown(engine);
        free(engine);
    }
}

int ikm_engine_initialize(ikm_engine_t* engine, const ikm_config_t* config) {
    if (!engine) {
        ikm_set_error("Invalid engine instance");
        return IKM_ERROR_UNKNOWN;
    }
    
    if (engine->initialized) {
        ikm_set_error("Engine already initialized");
        return IKM_ERROR_UNKNOWN;
    }
    
    memcpy(&engine->config, config, sizeof(ikm_config_t));
    strcpy(engine->base_path, ".");
    
    int result = ikm_get_system_info(engine->base_path, &engine->system_info);
    if (result != IKM_SUCCESS) {
        return result;
    }
    
    /* Initialize Lua interpreter */
    ikm_log(IKM_LOG_INFO, "Initializing Lua interpreter...");
    engine->lua_state = ikm_lua_create();
    if (!engine->lua_state) {
        ikm_set_error("Failed to create Lua state");
        return IKM_ERROR_UNKNOWN;
    }
    
    /* Create window */
    ikm_log(IKM_LOG_INFO, "Creating window %dx%d...", config->width, config->height);
    engine->window = ikm_window_create(
        IKM_ENGINE_NAME,
        config->width,
        config->height,
        config->fullscreen
    );
    if (!engine->window) {
        ikm_lua_destroy(engine->lua_state);
        return IKM_ERROR_UNKNOWN;
    }
    
    /* Initialize renderer */
    ikm_log(IKM_LOG_INFO, "Initializing renderer: %s", config->render_mode);
    engine->renderer = ikm_renderer_create();
    if (!engine->renderer) {
        ikm_window_destroy(engine->window);
        ikm_lua_destroy(engine->lua_state);
        ikm_set_error("Failed to create renderer");
        return IKM_ERROR_UNKNOWN;
    }
    
    result = ikm_renderer_initialize(engine->renderer, config->width, config->height);
    if (result != IKM_SUCCESS) {
        ikm_renderer_destroy(engine->renderer);
        ikm_window_destroy(engine->window);
        ikm_lua_destroy(engine->lua_state);
        return result;
    }
    
    /* Initialize input system */
    ikm_log(IKM_LOG_INFO, "Initializing input system...");
    engine->input = ikm_input_create(engine->window);
    if (!engine->input) {
        ikm_renderer_destroy(engine->renderer);
        ikm_window_destroy(engine->window);
        ikm_lua_destroy(engine->lua_state);
        ikm_set_error("Failed to create input system");
        return IKM_ERROR_UNKNOWN;
    }
    
    engine->initialized = 1;
    ikm_log(IKM_LOG_INFO, "Engine initialized successfully");
    return IKM_SUCCESS;
}

int ikm_engine_shutdown(ikm_engine_t* engine) {
    if (!engine || !engine->initialized) {
        return IKM_SUCCESS;
    }
    
    ikm_log(IKM_LOG_INFO, "Engine shutting down");
    
    /* Cleanup subsystems in reverse order */
    if (engine->input) {
        ikm_input_destroy(engine->input);
        engine->input = NULL;
    }
    
    if (engine->renderer) {
        ikm_renderer_destroy(engine->renderer);
        engine->renderer = NULL;
    }
    
    if (engine->window) {
        ikm_window_destroy(engine->window);
        engine->window = NULL;
    }
    
    if (engine->lua_state) {
        ikm_lua_destroy(engine->lua_state);
        engine->lua_state = NULL;
    }
    
    engine->initialized = 0;
    return IKM_SUCCESS;
}

/* Error handling */

const char* ikm_get_last_error(void) {
    return last_error_msg;
}

void ikm_clear_error(void) {
    last_error_msg[0] = '\0';
}

/* Logging */

void ikm_log(ikm_log_level_t level, const char* format, ...) {
    if (level < current_log_level) {
        return;
    }
    
    const char* level_str[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
    va_list args;
    
    FILE* out = (level >= IKM_LOG_ERROR) ? stderr : stdout;
    fprintf(out, "[%s] ", level_str[level]);
    
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    
    fprintf(out, "\n");
    
    if (log_file) {
        fprintf(log_file, "[%s] ", level_str[level]);
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
}

void ikm_log_set_level(ikm_log_level_t level) {
    current_log_level = level;
}

void ikm_log_set_file(const char* filepath) {
    if (log_file && log_file != stdout && log_file != stderr) {
        fclose(log_file);
    }
    log_file = fopen(filepath, "a");
}

/* Subsystem accessors */
ikm_lua_state_t* ikm_engine_get_lua_state(ikm_engine_t* engine) {
    return engine ? engine->lua_state : NULL;
}

ikm_window_t* ikm_engine_get_window(ikm_engine_t* engine) {
    return engine ? engine->window : NULL;
}

ikm_input_t* ikm_engine_get_input(ikm_engine_t* engine) {
    return engine ? engine->input : NULL;
}

ikm_renderer_t* ikm_engine_get_renderer(ikm_engine_t* engine) {
    return engine ? engine->renderer : NULL;
}
