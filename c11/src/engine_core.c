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
#include <stdbool.h>

static bool engine_initialized = false;

int ikemen_engine_init(void) {
    if (engine_initialized) {
        fprintf(stderr, "Warning: Engine already initialized\n");
        return 0;
    }
    
    printf("Ikemen C11 Engine initializing...\n");
    printf("Version: %s\n", ikemen_engine_get_version());
    
    /* TODO: Add actual initialization code here as modules are ported */
    
    engine_initialized = true;
    printf("Ikemen C11 Engine initialized successfully\n");
    
    return 0;
}

void ikemen_engine_shutdown(void) {
    if (!engine_initialized) {
        fprintf(stderr, "Warning: Engine not initialized\n");
        return;
    }
    
    printf("Ikemen C11 Engine shutting down...\n");
    
    /* TODO: Add actual cleanup code here as modules are ported */
    
    engine_initialized = false;
    printf("Ikemen C11 Engine shutdown complete\n");
}

const char* ikemen_engine_get_version(void) {
    static char version[32];
    snprintf(version, sizeof(version), "%d.%d.%d",
             IKEMEN_C11_VERSION_MAJOR,
             IKEMEN_C11_VERSION_MINOR,
             IKEMEN_C11_VERSION_PATCH);
    return version;
}
