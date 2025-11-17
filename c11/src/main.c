/*
 * Ikemen C11 Engine - Main Entry Point
 * Replicates src/main.go initialization and execution flow
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Forward declarations for external functions in engine.c */
extern int file_exists(const char* path);
extern int dir_exists(const char* path);

static void print_banner(void) {
    printf("===========================================\n");
    printf("%s v%s\n", ikm_engine_name(), ikm_engine_version_string());
    printf("Fighting Game Engine - C11 Implementation\n");
    printf("===========================================\n\n");
}

static void print_system_info(const ikm_system_info_t* info) {
    printf("System Information:\n");
    printf("  Data directory:     %s\n", info->data_dir);
    printf("  External directory: %s\n", info->external_dir);
    printf("  Font directory:     %s\n", info->font_dir);
    printf("  Save directory:     %s\n", info->save_dir);
    printf("  System definition:  %s %s\n", info->system_def,
           info->has_system_def ? "[OK]" : "[MISSING]");
    printf("  Common files:       %s\n", info->has_common_files ? "[OK]" : "[MISSING]");
    printf("\n");
}

static void print_config_summary(const ikm_config_t* cfg) {
    printf("Configuration:\n");
    printf("  Video:\n");
    printf("    Resolution:  %dx%d\n", cfg->width, cfg->height);
    printf("    Fullscreen:  %s\n", cfg->fullscreen ? "Yes" : "No");
    printf("    Gamma:       %.2f\n", cfg->gamma);
    printf("    MSAA:        %dx\n", cfg->msaa);
    printf("    Render Mode: %s\n", cfg->render_mode);
    printf("  Audio:\n");
    printf("    Master:      %d%%\n", cfg->master_volume);
    printf("    BGM:         %d%%\n", cfg->bgm_volume);
    printf("    SFX:         %d%%\n", cfg->sfx_volume);
    printf("    Frequency:   %d Hz\n", cfg->frequency);
    printf("    Channels:    %d\n", cfg->channels);
    printf("  System:\n");
    printf("    Script:      %s\n", cfg->system_script);
    printf("    Motif:       %s\n", cfg->motif);
    printf("    Common AIR:  %s\n", cfg->common_air);
    printf("    Common CMD:  %s\n", cfg->common_cmd);
    printf("  Gameplay:\n");
    printf("    Difficulty:  %d\n", cfg->difficulty);
    printf("    Life:        %.1f%%\n", cfg->life_multiplier * 100.0);
    printf("    Time:        %d seconds\n", cfg->time_limit);
    printf("    Rounds:      %d\n", cfg->rounds_to_win);
    printf("\n");
}

int main(int argc, char** argv) {
    ikm_engine_t* engine = NULL;
    ikm_config_t config;
    ikm_system_info_t system_info;
    const char* base_path = ".";
    const char* config_path = "save/config.ini";
    char stats_path[IKM_MAX_PATH];
    char log_path[IKM_MAX_PATH];
    int result;
    
    /* Print banner */
    print_banner();
    
    /* Parse command line arguments (like Go's processCommandLine) */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-config") == 0 && i + 1 < argc) {
            config_path = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0) {
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  -h, -?            Show this help\n");
            printf("  -config <path>    Use specific config file\n");
            printf("  -log <path>       Write log to file\n");
            printf("  -debug            Enable debug mode\n");
            return EXIT_SUCCESS;
        }
    }
    
    /* Setup logging (like Go's createLog) */
    snprintf(log_path, sizeof(log_path), "save/logs/ikemen_%ld.log", (long)time(NULL));
    ikm_log_set_file(log_path);
    ikm_log_set_level(IKM_LOG_INFO);
    
    ikm_log(IKM_LOG_INFO, "=== %s v%s Starting ===", 
            ikm_engine_name(), ikm_engine_version_string());
    ikm_log(IKM_LOG_INFO, "Base path: %s", base_path);
    ikm_log(IKM_LOG_INFO, "Config path: %s", config_path);
    
    /* Step 1: Create save directories (os.Mkdir in Go's main) */
    printf("[1/7] Creating save directories...\n");
    result = ikm_create_directories(base_path);
    if (result != IKM_SUCCESS) {
        fprintf(stderr, "ERROR: %s\n", ikm_get_last_error());
        ikm_log(IKM_LOG_ERROR, "Failed to create directories: %s", ikm_get_last_error());
        return EXIT_FAILURE;
    }
    ikm_log(IKM_LOG_INFO, "Save directories created");
    printf("  ✓ save/\n  ✓ save/replays/\n  ✓ save/logs/\n\n");
    
    /* Step 2: Initialize stats file (Go's save/stats.json initialization) */
    printf("[2/7] Initializing stats file...\n");
    snprintf(stats_path, sizeof(stats_path), "save/stats.json");
    if (!file_exists(stats_path)) {
        FILE* f = fopen(stats_path, "w");
        if (f) {
            fprintf(f, "{}");
            fclose(f);
            ikm_log(IKM_LOG_INFO, "Created stats file");
            printf("  ✓ Created %s\n\n", stats_path);
        } else {
            ikm_log(IKM_LOG_WARNING, "Could not create stats file");
            printf("  ⚠ Could not create stats file\n\n");
        }
    } else {
        printf("  ✓ Stats file exists\n\n");
    }
    
    /* Step 3: Load configuration (loadConfig in Go) */
    printf("[3/7] Loading configuration...\n");
    result = ikm_config_load(config_path, &config);
    if (result != IKM_SUCCESS) {
        fprintf(stderr, "ERROR: %s\n", ikm_get_last_error());
        ikm_log(IKM_LOG_ERROR, "Failed to load config: %s", ikm_get_last_error());
        printf("  ⚠ Using default configuration\n\n");
    } else {
        ikm_log(IKM_LOG_INFO, "Configuration loaded from %s", config_path);
        printf("  ✓ Loaded from %s\n\n", config_path);
    }
    
    print_config_summary(&config);
    
    /* Step 4: Verify system files (Go checks sys.cfg.Config.System) */
    printf("[4/7] Verifying system files...\n");
    result = ikm_verify_installation(base_path);
    if (result != IKM_SUCCESS) {
        fprintf(stderr, "ERROR: %s\n", ikm_get_last_error());
        fprintf(stderr, "\nRequired files/directories:\n");
        fprintf(stderr, "  - data/ (with system.base.def)\n");
        fprintf(stderr, "  - external/\n");
        fprintf(stderr, "  - font/\n");
        fprintf(stderr, "\nPlease ensure all required assets are present.\n");
        ikm_log(IKM_LOG_FATAL, "System verification failed: %s", ikm_get_last_error());
        return EXIT_FAILURE;
    }
    printf("  ✓ data/ directory\n");
    printf("  ✓ external/ directory\n");
    printf("  ✓ font/ directory\n");
    printf("  ✓ system.base.def\n");
    ikm_log(IKM_LOG_INFO, "System verification passed");
    printf("\n");
    
    /* Check for main Lua script (Go opens sys.cfg.Config.System) */
    if (!file_exists(config.system_script)) {
        fprintf(stderr, "ERROR: Main Lua script not found: %s\n", config.system_script);
        ikm_log(IKM_LOG_FATAL, "Main Lua script missing: %s", config.system_script);
        return EXIT_FAILURE;
    }
    printf("  ✓ Main script: %s\n\n", config.system_script);
    
    /* Step 5: Get system information */
    printf("[5/7] Gathering system information...\n");
    result = ikm_get_system_info(base_path, &system_info);
    if (result == IKM_SUCCESS) {
        ikm_log(IKM_LOG_INFO, "System info gathered");
        print_system_info(&system_info);
    }
    
    /* Step 6: Initialize engine (sys.init in Go) */
    printf("[6/7] Initializing engine...\n");
    engine = ikm_engine_create();
    if (!engine) {
        fprintf(stderr, "ERROR: Failed to create engine instance\n");
        ikm_log(IKM_LOG_FATAL, "Engine creation failed");
        return EXIT_FAILURE;
    }
    
    result = ikm_engine_initialize(engine, &config);
    if (result != IKM_SUCCESS) {
        fprintf(stderr, "ERROR: %s\n", ikm_get_last_error());
        ikm_log(IKM_LOG_FATAL, "Engine initialization failed: %s", ikm_get_last_error());
        ikm_engine_destroy(engine);
        return EXIT_FAILURE;
    }
    
    printf("  ✓ Engine core initialized\n");
    printf("  ✓ Game size: %dx%d\n", config.width, config.height);
    printf("  ✓ Render mode: %s\n", config.render_mode);
    ikm_log(IKM_LOG_INFO, "Engine initialized successfully");
    printf("\n");
    
    /* Step 7: Execute main script (sys.luaLState.DoFile in Go) */
    printf("[7/7] Executing main script: %s\n", config.system_script);
    ikm_log(IKM_LOG_INFO, "Would execute Lua script: %s", config.system_script);
    
    printf("\n===========================================\n");
    printf("Engine initialization complete!\n");
    printf("===========================================\n\n");
    
    printf("Implementation Status:\n");
    printf("  ✓ Directory setup\n");
    printf("  ✓ Configuration loading\n");
    printf("  ✓ System verification\n");
    printf("  ✓ Engine initialization\n");
    printf("  ✗ Window creation (requires graphics backend)\n");
    printf("  ✗ Lua script execution (requires Lua integration)\n");
    printf("  ✗ Graphics rendering (requires OpenGL/Vulkan)\n");
    printf("  ✗ Audio system (requires audio backend)\n");
    printf("  ✗ Input handling (requires input backend)\n");
    printf("  ✗ Game loop (requires all above systems)\n\n");
    
    printf("Next Implementation Steps:\n");
    printf("  1. Integrate Lua interpreter\n");
    printf("  2. Add OpenGL/Vulkan rendering backend\n");
    printf("  3. Implement audio system\n");
    printf("  4. Add input handling\n");
    printf("  5. Create game loop\n");
    printf("  6. Load and parse MUGEN assets (SFF, AIR, CNS)\n");
    printf("  7. Implement character state machine\n");
    printf("  8. Add match logic and gameplay\n\n");
    
    ikm_log(IKM_LOG_INFO, "Engine ready for extended implementation");
    
    /* Cleanup (defer sys.shutdown in Go) */
    ikm_engine_shutdown(engine);
    ikm_engine_destroy(engine);
    ikm_log(IKM_LOG_INFO, "=== Engine shutdown complete ===");
    
    return EXIT_SUCCESS;
}
