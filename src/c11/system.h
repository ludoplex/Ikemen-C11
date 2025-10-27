/**
 * System management for Ikemen C11
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int width;
    int height;
    bool running;
    void *window;      // Platform-specific window handle
    void *gl_context;  // OpenGL context
    void *audio_ctx;   // Audio context
    void *lua_state;   // Lua state
} system_t;

/**
 * Initialize the game system
 */
int system_init(system_t *sys, int width, int height);

/**
 * Shutdown and cleanup the game system
 */
void system_shutdown(system_t *sys);

/**
 * Main game loop
 */
void system_run(system_t *sys);

#endif /* SYSTEM_H */
