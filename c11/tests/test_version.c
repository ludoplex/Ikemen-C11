/*
 * Ikemen C11 Engine - Version Function Tests
 * 
 * This test verifies that the version query functions return expected values.
 * This is a unit-style test focusing on the API contract.
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(void) {
    int major, minor, patch;
    const char* version_str;
    char expected_version[32];
    
    printf("=== Ikemen C11 Engine - Version Function Tests ===\n\n");
    
    /* Test 1: Major version should be non-negative */
    printf("Test 1: Major version function\n");
    major = ikm_engine_major_version();
    ASSERT(major >= 0, "Major version is non-negative");
    printf("  Value: %d\n\n", major);
    
    /* Test 2: Minor version should be non-negative */
    printf("Test 2: Minor version function\n");
    minor = ikm_engine_minor_version();
    ASSERT(minor >= 0, "Minor version is non-negative");
    printf("  Value: %d\n\n", minor);
    
    /* Test 3: Patch version should be non-negative */
    printf("Test 3: Patch version function\n");
    patch = ikm_engine_patch_version();
    ASSERT(patch >= 0, "Patch version is non-negative");
    printf("  Value: %d\n\n", patch);
    
    /* Test 4: Version string should not be NULL */
    printf("Test 4: Version string function\n");
    version_str = ikm_engine_version_string();
    ASSERT(version_str != NULL, "Version string is not NULL");
    ASSERT(strlen(version_str) > 0, "Version string is not empty");
    printf("  Value: \"%s\"\n\n", version_str);
    
    /* Test 5: Version string format should match individual components */
    printf("Test 5: Version string format consistency\n");
    snprintf(expected_version, sizeof(expected_version), "%d.%d.%d", 
             major, minor, patch);
    ASSERT(strcmp(version_str, expected_version) == 0,
           "Version string matches individual component values");
    printf("  Expected: \"%s\"\n", expected_version);
    printf("  Actual:   \"%s\"\n\n", version_str);
    
    /* Test 6: Multiple calls should return consistent values */
    printf("Test 6: Consistency of repeated calls\n");
    ASSERT(ikm_engine_major_version() == major,
           "Major version consistent across calls");
    ASSERT(ikm_engine_minor_version() == minor,
           "Minor version consistent across calls");
    ASSERT(ikm_engine_patch_version() == patch,
           "Patch version consistent across calls");
    printf("\n");
    
    /* Test 7: Version constants match function return values */
    printf("Test 7: Version constants match functions\n");
    ASSERT(IKM_ENGINE_MAJOR == major,
           "IKM_ENGINE_MAJOR constant matches function");
    ASSERT(IKM_ENGINE_MINOR == minor,
           "IKM_ENGINE_MINOR constant matches function");
    ASSERT(IKM_ENGINE_PATCH == patch,
           "IKM_ENGINE_PATCH constant matches function");
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
