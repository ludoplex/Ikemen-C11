/**
 * Configuration handling for Ikemen C11
 * 
 * Handles loading and parsing of INI configuration files
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_PATH_LEN 512
#define MAX_CMD_FLAGS 64

typedef struct {
    char system_script[MAX_PATH_LEN];
    int game_width;
    int game_height;
    bool fullscreen;
    int master_volume;
    
    // Command line flags
    char cmd_flags[MAX_CMD_FLAGS][2][MAX_PATH_LEN]; // key-value pairs
    int num_flags;
} config_t;

/**
 * Load configuration from INI file
 */
int load_config(const char *path, config_t *config);

/**
 * Process command line arguments
 */
int process_command_line(int argc, char *argv[], config_t *config);

/**
 * Cleanup configuration resources
 */
void config_cleanup(config_t *config);

/**
 * Get command line flag value
 */
const char* config_get_flag(const config_t *config, const char *flag);

#endif /* CONFIG_H */
