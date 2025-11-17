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

#ifndef IKEMEN_ENGINE_CORE_H
#define IKEMEN_ENGINE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Engine version information
 */
#define IKEMEN_C11_VERSION_MAJOR 0
#define IKEMEN_C11_VERSION_MINOR 1
#define IKEMEN_C11_VERSION_PATCH 0

/**
 * @brief Initialize the Ikemen C11 engine
 * 
 * This function performs all necessary initialization for the engine.
 * Must be called before any other engine functions.
 * 
 * @return 0 on success, non-zero on failure
 */
int ikemen_engine_init(void);

/**
 * @brief Shutdown the Ikemen C11 engine
 * 
 * Cleans up all engine resources. Should be called before application exit.
 */
void ikemen_engine_shutdown(void);

/**
 * @brief Get the engine version string
 * 
 * @return A constant string containing the engine version
 */
const char* ikemen_engine_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* IKEMEN_ENGINE_CORE_H */
