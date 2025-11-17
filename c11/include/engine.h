/**
 * @file engine.h
 * @brief Core engine definitions and API for Ikemen C11
 * 
 * This is the main public header for the Ikemen C11 engine.
 * It provides core types, initialization, and engine management functions.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Engine version information
 */
#define IKEMEN_C11_VERSION_MAJOR 0
#define IKEMEN_C11_VERSION_MINOR 1
#define IKEMEN_C11_VERSION_PATCH 0
#define IKEMEN_C11_VERSION_STRING "0.1.0-scaffolding"

/**
 * @brief Error codes returned by engine functions
 */
typedef enum {
    ENGINE_SUCCESS = 0,
    ENGINE_ERROR_INVALID_ARGUMENT = -1,
    ENGINE_ERROR_INITIALIZATION_FAILED = -2,
    ENGINE_ERROR_OUT_OF_MEMORY = -3,
    ENGINE_ERROR_NOT_IMPLEMENTED = -4,
    ENGINE_ERROR_UNKNOWN = -100
} EngineError;

/**
 * @brief Engine configuration structure
 */
typedef struct {
    uint32_t window_width;
    uint32_t window_height;
    bool fullscreen;
    const char* title;
} EngineConfig;

/**
 * @brief Initialize the engine with the given configuration
 * 
 * @param config Pointer to engine configuration
 * @return ENGINE_SUCCESS on success, error code otherwise
 */
EngineError engine_init(const EngineConfig* config);

/**
 * @brief Shutdown the engine and free all resources
 */
void engine_shutdown(void);

/**
 * @brief Get the engine version string
 * 
 * @return Version string
 */
const char* engine_get_version(void);

/**
 * @brief Get a human-readable error message for an error code
 * 
 * @param error Error code
 * @return Error message string
 */
const char* engine_error_string(EngineError error);

/**
 * @brief Check if the engine is initialized
 * 
 * @return true if initialized, false otherwise
 */
bool engine_is_initialized(void);
