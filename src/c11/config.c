/**
 * Configuration implementation for Ikemen C11
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Initialize config with default values
 */
static void init_default_config(config_t *config) {
    strncpy(config->system_script, "data/system.def", MAX_PATH_LEN - 1);
    config->game_width = 640;
    config->game_height = 480;
    config->fullscreen = false;
    config->master_volume = 100;
    config->num_flags = 0;
}

/**
 * Parse a simple INI file
 * This is a basic implementation - a full implementation would use a library
 */
int load_config(const char *path, config_t *config) {
    init_default_config(config);
    
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        // Config file doesn't exist, use defaults
        return 0;
    }
    
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        // Skip comments and empty lines
        if (line[0] == ';' || line[0] == '#' || line[0] == '\n') {
            continue;
        }
        
        // Parse key=value
        char *equals = strchr(line, '=');
        if (equals != NULL) {
            *equals = '\0';
            char *key = line;
            char *value = equals + 1;
            
            // Trim newline from value
            char *newline = strchr(value, '\n');
            if (newline) *newline = '\0';
            
            // Process known config keys
            if (strcmp(key, "System") == 0) {
                strncpy(config->system_script, value, MAX_PATH_LEN - 1);
            } else if (strcmp(key, "GameWidth") == 0) {
                config->game_width = atoi(value);
            } else if (strcmp(key, "GameHeight") == 0) {
                config->game_height = atoi(value);
            } else if (strcmp(key, "Fullscreen") == 0) {
                config->fullscreen = (atoi(value) != 0);
            } else if (strcmp(key, "MasterVolume") == 0) {
                config->master_volume = atoi(value);
            }
        }
    }
    
    fclose(f);
    return 0;
}

/**
 * Add a command line flag
 */
static void add_flag(config_t *config, const char *key, const char *value) {
    if (config->num_flags < MAX_CMD_FLAGS) {
        strncpy(config->cmd_flags[config->num_flags][0], key, MAX_PATH_LEN - 1);
        strncpy(config->cmd_flags[config->num_flags][1], value, MAX_PATH_LEN - 1);
        config->num_flags++;
    }
}

/**
 * Process command line arguments
 */
int process_command_line(int argc, char *argv[], config_t *config) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // This is a flag
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0) {
                printf("Ikemen C11 Command line options\n\n");
                printf("Options (case sensitive):\n");
                printf("-h -?                   Help\n");
                printf("-config <path>          Use config file at <path>\n");
                printf("-r <path>               Loads motif <path>\n");
                printf("-windowed               Starts in windowed mode\n");
                printf("-width <num>            Sets game width\n");
                printf("-height <num>           Sets game height\n");
                printf("\nPress ENTER to exit\n");
                getchar();
                exit(0);
            }
            
            const char *key = argv[i];
            const char *value = "true";
            
            // Check if next arg is a value
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[i + 1];
                i++; // Skip the value in next iteration
            }
            
            add_flag(config, key, value);
            
            // Apply some flags immediately
            if (strcmp(key, "-width") == 0) {
                config->game_width = atoi(value);
            } else if (strcmp(key, "-height") == 0) {
                config->game_height = atoi(value);
            } else if (strcmp(key, "-windowed") == 0) {
                config->fullscreen = false;
            }
        }
    }
    
    return 0;
}

/**
 * Get command line flag value
 */
const char* config_get_flag(const config_t *config, const char *flag) {
    for (int i = 0; i < config->num_flags; i++) {
        if (strcmp(config->cmd_flags[i][0], flag) == 0) {
            return config->cmd_flags[i][1];
        }
    }
    return NULL;
}

/**
 * Cleanup configuration resources
 */
void config_cleanup(config_t *config) {
    // Nothing to cleanup in current implementation
    (void)config;
}
