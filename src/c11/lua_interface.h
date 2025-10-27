/**
 * Lua interface for Ikemen C11
 */

#ifndef LUA_INTERFACE_H
#define LUA_INTERFACE_H

#include "system.h"

/**
 * Initialize Lua state
 */
int lua_init(system_t *sys, const char *script_path);

/**
 * Execute a Lua script
 */
int lua_execute(system_t *sys, const char *script_path);

/**
 * Shutdown Lua state
 */
void lua_shutdown(system_t *sys);

#endif /* LUA_INTERFACE_H */
