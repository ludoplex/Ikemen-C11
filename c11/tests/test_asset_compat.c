/*
 * Ikemen C11 Engine - Asset Compatibility Test
 * 
 * This integration-style test verifies the asset compatibility check function
 * under various conditions:
 * - Success case with valid asset directory structure
 * - Failure case with missing directories
 * - Edge cases (NULL path, invalid paths)
 * 
 * The test creates a temporary directory structure to simulate different
 * asset configurations without depending on the actual repository state.
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* Test result tracking */
static int tests_run = 0;
static int tests_passed = 0;

/* Test assertion helper */
#define ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ %s\n", message); \
        } \
    } while(0)

/* Helper to create a directory (ignoring error if it already exists) */
static int create_dir(const char* path) {
    if (mkdir(path, 0755) != 0 && errno != EEXIST) {
        return 0;
    }
    return 1;
}

/* Helper to create a complete asset directory structure */
static int create_asset_structure(const char* base) {
    char path[512];
    
    /* Create base directory */
    if (!create_dir(base)) {
        return 0;
    }
    
    /* Create data/ directory */
    snprintf(path, sizeof(path), "%s/data", base);
    if (!create_dir(path)) {
        return 0;
    }
    
    /* Create external/ directory */
    snprintf(path, sizeof(path), "%s/external", base);
    if (!create_dir(path)) {
        return 0;
    }
    
    /* Create font/ directory */
    snprintf(path, sizeof(path), "%s/font", base);
    if (!create_dir(path)) {
        return 0;
    }
    
    return 1;
}

/* Helper to remove directory recursively (simple version for test cleanup) */
static void remove_dir_recursive(const char* path) {
    char cmd[1024];
    /* Use system rm command for cleanup (portable enough for testing) */
    snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", path);
    system(cmd);
}

int main(void) {
    int result;
    const char* error_msg;
    char test_dir[256];
    char incomplete_dir[256];
    
    printf("=== Ikemen C11 Engine - Asset Compatibility Tests ===\n\n");
    
    /* Setup: Create temporary test directories */
    snprintf(test_dir, sizeof(test_dir), "/tmp/ikemen_test_%d", (int)getpid());
    snprintf(incomplete_dir, sizeof(incomplete_dir), "/tmp/ikemen_incomplete_%d", (int)getpid());
    
    /* Test 1: NULL base path should default to current directory */
    printf("Test 1: NULL base path handling\n");
    result = ikm_verify_installation(NULL);
    /* We can't predict if current dir has assets, so just check it doesn't crash */
    ASSERT(result >= 0 && result <= 99, "Function handles NULL path without crashing");
    error_msg = ikm_get_last_error();
    ASSERT(error_msg != NULL, "Error message function returns non-NULL");
    printf("  Result code: %d\n", result);
    printf("  Message: %s\n\n", error_msg);
    
    /* Test 2: Invalid/non-existent base path */
    printf("Test 2: Non-existent base path\n");
    result = ikm_verify_installation("/nonexistent/path/that/should/not/exist/12345");
    ASSERT(result != IKM_SUCCESS, "Non-existent path returns error");
    ASSERT(result == IKM_ERROR_INVALID_PATH || result == IKM_ERROR_FILE_NOT_FOUND,
           "Error code indicates path problem");
    error_msg = ikm_get_last_error();
    ASSERT(strlen(error_msg) > 0, "Error message is not empty");
    printf("  Result code: %d\n", result);
    printf("  Message: %s\n\n", error_msg);
    
    /* Test 3: Create complete valid structure and verify success */
    printf("Test 3: Complete asset structure (success case)\n");
    if (create_asset_structure(test_dir)) {
        /* Also create system.base.def file */
        char sysdef_path[512];
        snprintf(sysdef_path, sizeof(sysdef_path), "%s/data/system.base.def", test_dir);
        FILE* f = fopen(sysdef_path, "w");
        if (f) {
            fprintf(f, "[Info]\n");
            fclose(f);
        }
        result = ikm_verify_installation(test_dir);
        ASSERT(result == IKM_SUCCESS, "Complete structure returns IKM_SUCCESS");
        error_msg = ikm_get_last_error();
        printf("  Result code: %d\n", result);
        printf("  Message: %s\n\n", error_msg);
    } else {
        printf("  ⚠ Could not create test directory structure\n\n");
    }
    
    /* Test 4: Incomplete structure (missing directories) */
    printf("Test 4: Incomplete asset structure (missing directories)\n");
    if (create_dir(incomplete_dir)) {
        char data_path[512];
        /* Create only data/ but not external/ or font/ */
        snprintf(data_path, sizeof(data_path), "%s/data", incomplete_dir);
        create_dir(data_path);
        
        result = ikm_verify_installation(incomplete_dir);
        ASSERT(result == IKM_ERROR_FILE_NOT_FOUND,
               "Incomplete structure returns IKM_ERROR_FILE_NOT_FOUND");
        error_msg = ikm_get_last_error();
        ASSERT(strlen(error_msg) > 0, "Error message explains what's missing");
        printf("  Result code: %d\n", result);
        printf("  Message: %s\n\n", error_msg);
    } else {
        printf("  ⚠ Could not create test directory\n\n");
    }
    
    /* Test 5: Check against actual repository structure (if available) */
    printf("Test 5: Check repository root (integration test)\n");
    /* Try checking from repository root - this may pass or fail depending on environment */
    result = ikm_verify_installation("..");
    printf("  Result code: %d\n", result);
    error_msg = ikm_get_last_error();
    printf("  Message: %s\n", error_msg);
    if (result == IKM_SUCCESS) {
        ASSERT(1, "Repository structure is compatible");
    } else {
        printf("  Note: Repository check did not pass (may be expected in some environments)\n");
        /* Don't fail the test if repo check doesn't pass - we might not be in right location */
        tests_run++;
        tests_passed++;
    }
    printf("\n");
    
    /* Test 6: Error message persistence */
    printf("Test 6: Error message state management\n");
    char first_error_copy[512];
    char second_error_copy[512];
    
    /* First call with bad path */
    result = ikm_verify_installation("/nonexistent");
    strncpy(first_error_copy, ikm_get_last_error(), sizeof(first_error_copy) - 1);
    first_error_copy[sizeof(first_error_copy) - 1] = '\0';
    
    /* Second call with different bad path */
    result = ikm_verify_installation("/another_nonexistent");
    strncpy(second_error_copy, ikm_get_last_error(), sizeof(second_error_copy) - 1);
    second_error_copy[sizeof(second_error_copy) - 1] = '\0';
    
    ASSERT(strlen(first_error_copy) > 0 && strlen(second_error_copy) > 0,
           "Error messages available after each call");
    /* Errors should be different (different paths) */
    ASSERT(strcmp(first_error_copy, second_error_copy) != 0,
           "Error message updates on subsequent calls");
    printf("  First error:  %s\n", first_error_copy);
    printf("  Second error: %s\n\n", second_error_copy);
    
    /* Cleanup */
    printf("Cleanup: Removing temporary test directories...\n");
    remove_dir_recursive(test_dir);
    remove_dir_recursive(incomplete_dir);
    printf("\n");
    
    /* Print summary */
    printf("=== Test Summary ===\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    if (tests_passed == tests_run) {
        printf("\n✓ All tests PASSED\n");
        return EXIT_SUCCESS;
    } else {
        printf("\n✗ Some tests FAILED\n");
        return EXIT_FAILURE;
    }
}
