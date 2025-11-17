/**
 * @file test_engine.c
 * @brief Unit tests for the core engine functionality
 */

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Test helper macros
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, message); \
            return 1; \
        } \
    } while(0)

#define TEST_PASS() \
    do { \
        printf("  PASS\n"); \
        return 0; \
    } while(0)

/**
 * Test engine version retrieval
 */
static int test_engine_version(void) {
    printf("Test: engine_get_version()...");
    
    const char* version = engine_get_version();
    TEST_ASSERT(version != NULL, "Version string is NULL");
    TEST_ASSERT(strlen(version) > 0, "Version string is empty");
    TEST_ASSERT(strstr(version, "0.1.0") != NULL, "Version string incorrect");
    
    TEST_PASS();
}

/**
 * Test engine initialization with valid config
 */
static int test_engine_init_valid(void) {
    printf("Test: engine_init() with valid config...");
    
    // Make sure we start clean
    if (engine_is_initialized()) {
        engine_shutdown();
    }
    
    EngineConfig config = {
        .window_width = 800,
        .window_height = 600,
        .fullscreen = false,
        .title = "Test Window"
    };
    
    EngineError result = engine_init(&config);
    TEST_ASSERT(result == ENGINE_SUCCESS, "Init failed");
    TEST_ASSERT(engine_is_initialized(), "Engine not initialized");
    
    // Clean up
    engine_shutdown();
    TEST_ASSERT(!engine_is_initialized(), "Engine still initialized after shutdown");
    
    TEST_PASS();
}

/**
 * Test engine initialization with NULL config
 */
static int test_engine_init_null_config(void) {
    printf("Test: engine_init() with NULL config...");
    
    if (engine_is_initialized()) {
        engine_shutdown();
    }
    
    EngineError result = engine_init(NULL);
    TEST_ASSERT(result == ENGINE_ERROR_INVALID_ARGUMENT, "Should reject NULL config");
    TEST_ASSERT(!engine_is_initialized(), "Engine should not be initialized");
    
    TEST_PASS();
}

/**
 * Test engine initialization with invalid dimensions
 */
static int test_engine_init_invalid_dimensions(void) {
    printf("Test: engine_init() with invalid dimensions...");
    
    if (engine_is_initialized()) {
        engine_shutdown();
    }
    
    EngineConfig config = {
        .window_width = 0,
        .window_height = 0,
        .fullscreen = false,
        .title = "Test"
    };
    
    EngineError result = engine_init(&config);
    TEST_ASSERT(result == ENGINE_ERROR_INVALID_ARGUMENT, "Should reject zero dimensions");
    TEST_ASSERT(!engine_is_initialized(), "Engine should not be initialized");
    
    TEST_PASS();
}

/**
 * Test error string function
 */
static int test_engine_error_string(void) {
    printf("Test: engine_error_string()...");
    
    const char* msg = engine_error_string(ENGINE_SUCCESS);
    TEST_ASSERT(msg != NULL, "Error string is NULL");
    TEST_ASSERT(strlen(msg) > 0, "Error string is empty");
    
    msg = engine_error_string(ENGINE_ERROR_INVALID_ARGUMENT);
    TEST_ASSERT(msg != NULL, "Error string is NULL");
    TEST_ASSERT(strlen(msg) > 0, "Error string is empty");
    
    TEST_PASS();
}

/**
 * Test double initialization
 */
static int test_engine_double_init(void) {
    printf("Test: Double initialization...");
    
    if (engine_is_initialized()) {
        engine_shutdown();
    }
    
    EngineConfig config = {
        .window_width = 640,
        .window_height = 480,
        .fullscreen = false,
        .title = "Test"
    };
    
    EngineError result1 = engine_init(&config);
    TEST_ASSERT(result1 == ENGINE_SUCCESS, "First init failed");
    
    EngineError result2 = engine_init(&config);
    TEST_ASSERT(result2 == ENGINE_SUCCESS, "Second init should succeed (already initialized)");
    TEST_ASSERT(engine_is_initialized(), "Engine should still be initialized");
    
    engine_shutdown();
    
    TEST_PASS();
}

/**
 * Test shutdown without initialization
 */
static int test_engine_shutdown_not_initialized(void) {
    printf("Test: Shutdown without initialization...");
    
    if (engine_is_initialized()) {
        engine_shutdown();
    }
    
    // This should not crash
    engine_shutdown();
    TEST_ASSERT(!engine_is_initialized(), "Engine should not be initialized");
    
    TEST_PASS();
}

/**
 * Main test runner
 */
int main(void) {
    int failed = 0;
    int total = 0;
    
    printf("==============================================\n");
    printf("  Ikemen C11 Engine Tests\n");
    printf("==============================================\n\n");
    
    // Define test suite
    typedef int (*TestFunc)(void);
    struct {
        const char* name;
        TestFunc func;
    } tests[] = {
        {"Version", test_engine_version},
        {"Valid Init", test_engine_init_valid},
        {"NULL Config", test_engine_init_null_config},
        {"Invalid Dimensions", test_engine_init_invalid_dimensions},
        {"Error Strings", test_engine_error_string},
        {"Double Init", test_engine_double_init},
        {"Shutdown Not Init", test_engine_shutdown_not_initialized},
    };
    
    total = sizeof(tests) / sizeof(tests[0]);
    
    // Run all tests
    for (int i = 0; i < total; i++) {
        if (tests[i].func() != 0) {
            failed++;
        }
    }
    
    printf("\n==============================================\n");
    printf("  Test Results\n");
    printf("==============================================\n");
    printf("Total:  %d\n", total);
    printf("Passed: %d\n", total - failed);
    printf("Failed: %d\n", failed);
    printf("==============================================\n");
    
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
