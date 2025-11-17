/**
 * @file engine.c
 * @brief Core engine implementation for Ikemen C11
 */

#include "engine.h"
#include <stdio.h>
#include <string.h>

static bool g_engine_initialized = false;
static EngineConfig g_engine_config = {0};

EngineError engine_init(const EngineConfig* config) {
    if (config == NULL) {
        return ENGINE_ERROR_INVALID_ARGUMENT;
    }

    if (g_engine_initialized) {
        fprintf(stderr, "Warning: Engine already initialized\n");
        return ENGINE_SUCCESS;
    }

    // Copy configuration
    g_engine_config = *config;
    
    // Basic validation
    if (g_engine_config.window_width == 0 || g_engine_config.window_height == 0) {
        return ENGINE_ERROR_INVALID_ARGUMENT;
    }

    g_engine_initialized = true;
    
    printf("Ikemen C11 Engine initialized\n");
    printf("  Version: %s\n", IKEMEN_C11_VERSION_STRING);
    printf("  Window: %dx%d\n", config->window_width, config->window_height);
    printf("  Fullscreen: %s\n", config->fullscreen ? "yes" : "no");
    if (config->title) {
        printf("  Title: %s\n", config->title);
    }

    return ENGINE_SUCCESS;
}

void engine_shutdown(void) {
    if (!g_engine_initialized) {
        return;
    }

    printf("Ikemen C11 Engine shutting down\n");
    
    // Clear configuration
    memset(&g_engine_config, 0, sizeof(g_engine_config));
    g_engine_initialized = false;
}

const char* engine_get_version(void) {
    return IKEMEN_C11_VERSION_STRING;
}

const char* engine_error_string(EngineError error) {
    switch (error) {
        case ENGINE_SUCCESS:
            return "Success";
        case ENGINE_ERROR_INVALID_ARGUMENT:
            return "Invalid argument";
        case ENGINE_ERROR_INITIALIZATION_FAILED:
            return "Initialization failed";
        case ENGINE_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case ENGINE_ERROR_NOT_IMPLEMENTED:
            return "Not implemented";
        case ENGINE_ERROR_UNKNOWN:
        default:
            return "Unknown error";
    }
}

bool engine_is_initialized(void) {
    return g_engine_initialized;
}
