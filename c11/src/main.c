/**
 * @file main.c
 * @brief Ikemen C11 scaffold executable
 * 
 * This is a minimal stub executable that demonstrates the C11 engine
 * initialization and serves as a starting point for development.
 */

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    (void)argc;  // Unused parameter
    (void)argv;  // Unused parameter

    printf("==============================================\n");
    printf("  Ikemen C11 Engine Scaffold\n");
    printf("==============================================\n");
    printf("\n");
    
    // Display version
    printf("Version: %s\n\n", engine_get_version());
    
    // Configure the engine
    EngineConfig config = {
        .window_width = 1280,
        .window_height = 720,
        .fullscreen = false,
        .title = "Ikemen C11 Scaffold"
    };
    
    // Initialize the engine
    printf("Initializing engine...\n");
    EngineError result = engine_init(&config);
    
    if (result != ENGINE_SUCCESS) {
        fprintf(stderr, "Error: Failed to initialize engine: %s\n", 
                engine_error_string(result));
        return EXIT_FAILURE;
    }
    
    printf("\nEngine initialized successfully!\n");
    printf("This is a minimal scaffold. Full engine implementation coming soon.\n");
    printf("\n");
    
    // Verify initialization
    if (engine_is_initialized()) {
        printf("Status: Engine is running\n");
    }
    
    // Shutdown
    printf("\nShutting down...\n");
    engine_shutdown();
    
    printf("==============================================\n");
    printf("  Scaffold execution complete\n");
    printf("==============================================\n");
    
    return EXIT_SUCCESS;
}
