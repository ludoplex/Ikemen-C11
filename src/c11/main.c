/**
 * Ikemen C11 - Main Entry Point
 * 
 * This is a C11 reimplementation of the Ikemen GO fighting game engine.
 * The original was written in Go, this version aims to provide better
 * performance and control through C11.
 *
 * License: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "system.h"
#include "lua_interface.h"

#define VERSION "development"
#define BUILD_TIME __DATE__ " " __TIME__

/**
 * Create a directory if it doesn't exist
 */
static int ensure_directory(const char *path) {
#ifdef _WIN32
    return mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

/**
 * Initialize save directories
 */
static void init_save_directories(void) {
    ensure_directory("save");
    ensure_directory("save/replays");
    ensure_directory("save/logs");
}

/**
 * Show error dialog and exit
 */
static void show_error_and_exit(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    // TODO: Implement platform-specific error dialog
    exit(EXIT_FAILURE);
}

/**
 * Initialize stats file
 */
static void init_stats_file(void) {
    FILE *f = fopen("save/stats.json", "r");
    if (f == NULL) {
        // Create empty stats file
        f = fopen("save/stats.json", "w");
        if (f == NULL) {
            show_error_and_exit("Failed to create stats.json");
        }
        fprintf(f, "{}");
        fclose(f);
    } else {
        fclose(f);
    }
}

/**
 * Check if running inside macOS app bundle
 */
static bool is_running_inside_app_bundle(const char *exe_path) {
    return strstr(exe_path, ".app/Contents/MacOS/") != NULL;
}

/**
 * Main entry point
 */
int main(int argc, char *argv[]) {
    printf("Ikemen C11 Engine\n");
    printf("Version: %s\n", VERSION);
    printf("Build Time: %s\n\n", BUILD_TIME);

    // Initialize save directories
    init_save_directories();
    
    // Initialize stats file
    init_stats_file();
    
    // Determine config file path
    const char *config_path = "save/config.ini";
    
    // Process command line arguments
    config_t config;
    if (process_command_line(argc, argv, &config) != 0) {
        return EXIT_FAILURE;
    }
    
    // Load configuration
    if (load_config(config_path, &config) != 0) {
        show_error_and_exit("Failed to load configuration");
        return EXIT_FAILURE;
    }
    
    // Check if main lua file exists
    FILE *lua_file = fopen(config.system_script, "r");
    if (lua_file == NULL) {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), 
                "Main lua file \"%s\" error: %s", 
                config.system_script, strerror(errno));
        show_error_and_exit(error_msg);
        return EXIT_FAILURE;
    }
    fclose(lua_file);
    
    // Initialize system
    system_t sys;
    if (system_init(&sys, config.game_width, config.game_height) != 0) {
        show_error_and_exit("Failed to initialize system");
        return EXIT_FAILURE;
    }
    
    // Initialize Lua state
    if (lua_init(&sys, config.system_script) != 0) {
        system_shutdown(&sys);
        show_error_and_exit("Failed to initialize Lua");
        return EXIT_FAILURE;
    }
    
    // Execute main Lua script
    if (lua_execute(&sys, config.system_script) != 0) {
        // Create error log
        FILE *error_log = fopen("Ikemen.log", "w");
        if (error_log != NULL) {
            fprintf(error_log, "Version: %s\nBuild Time: %s\n\nError log:\n", 
                   VERSION, BUILD_TIME);
            fprintf(error_log, "Lua execution failed\n");
            fclose(error_log);
        }
        
        lua_shutdown(&sys);
        system_shutdown(&sys);
        show_error_and_exit("Lua execution failed. Error saved to Ikemen.log");
        return EXIT_FAILURE;
    }
    
    // Cleanup
    lua_shutdown(&sys);
    system_shutdown(&sys);
    config_cleanup(&config);
    
    printf("Ikemen C11 shutting down normally.\n");
    return EXIT_SUCCESS;
}
