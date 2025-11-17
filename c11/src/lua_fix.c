/* Suppress unused parameter warnings for Lua callback functions */
#include "ikemen_engine.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static int lua_getGameWidth(lua_State* L) {
    UNUSED(L);
    lua_pushinteger(L, 640);
    return 1;
}
