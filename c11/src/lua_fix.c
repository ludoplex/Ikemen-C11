/* Suppress unused parameter warnings for Lua callback functions */
#define UNUSED(x) (void)(x)

static int lua_getGameWidth(lua_State* L) {
    UNUSED(L);
    lua_pushinteger(L, 640);
    return 1;
}
