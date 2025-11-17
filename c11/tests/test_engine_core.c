/*
 * MIT License
 *
 * Copyright (c) 2024 Ikemen C11 Contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "engine_core.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * Test: Engine initialization and shutdown
 */
static void test_engine_init_shutdown(void) {
    printf("Running: test_engine_init_shutdown\n");
    
    /* Test successful initialization */
    assert(ikemen_engine_init() == 0);
    
    /* Test shutdown */
    ikemen_engine_shutdown();
    
    printf("  PASSED\n");
}

/**
 * Test: Engine version string
 */
static void test_engine_version(void) {
    printf("Running: test_engine_version\n");
    
    const char* version = ikemen_engine_get_version();
    
    /* Version string should not be NULL */
    assert(version != NULL);
    
    /* Version string should not be empty */
    assert(strlen(version) > 0);
    
    /* Version should match expected format (e.g., "0.1.0") */
    assert(strcmp(version, "0.1.0") == 0);
    
    printf("  Version: %s\n", version);
    printf("  PASSED\n");
}

/**
 * Test: Multiple initialization attempts
 */
static void test_engine_multiple_init(void) {
    printf("Running: test_engine_multiple_init\n");
    
    /* Initialize once */
    assert(ikemen_engine_init() == 0);
    
    /* Try to initialize again (should handle gracefully) */
    assert(ikemen_engine_init() == 0);
    
    /* Cleanup */
    ikemen_engine_shutdown();
    
    printf("  PASSED\n");
}

int main(void) {
    printf("Ikemen C11 Engine Tests\n");
    printf("=======================\n\n");
    
    test_engine_version();
    test_engine_init_shutdown();
    test_engine_multiple_init();
    
    printf("\n=======================\n");
    printf("All tests passed!\n");
    
    return 0;
}
