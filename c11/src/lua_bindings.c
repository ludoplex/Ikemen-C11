/*
 * Ikemen C11 Engine - Lua Integration
 * Implements Lua 5.1 scripting interface matching Go's lua bindings
 */

#include "ikemen_engine.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Lua state wrapper */
struct ikm_lua_state {
    lua_State* L;
    int initialized;
};

/* System functions registered with Lua */

static int lua_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (i > 1) printf("\t");
        if (lua_isstring(L, i)) {
            printf("%s", lua_tostring(L, i));
        } else if (lua_isnil(L, i)) {
            printf("nil");
        } else if (lua_isboolean(L, i)) {
            printf("%s", lua_toboolean(L, i) ? "true" : "false");
        } else {
            printf("%s", luaL_typename(L, i));
        }
    }
    printf("\n");
    return 0;
}

static int lua_getGameWidth(lua_State* L) {
    lua_pushinteger(L, 640);
    return 1;
}

static int lua_getGameHeight(lua_State* L) {
    lua_pushinteger(L, 480);
    return 1;
}

static int lua_getFrameCount(lua_State* L) {
    lua_pushinteger(L, 0);
    return 1;
}

static int lua_refresh(lua_State* L) {
    UNUSED(L);
    return 0;
}

static int lua_commandGetState(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

static int lua_setStage(lua_State* L) {
    const char* stage_name = lua_tostring(L, 1);
    ikm_log(IKM_LOG_INFO, "Lua: setStage(%s)", stage_name);
    return 0;
}

static int lua_setCharacter(lua_State* L) {
    int player = (int)lua_tonumber(L, 1);
    const char* char_name = lua_tostring(L, 2);
    ikm_log(IKM_LOG_INFO, "Lua: setCharacter(p%d, %s)", player, char_name);
    return 0;
}

ikm_lua_state_t* ikm_lua_create(void) {
    ikm_lua_state_t* state = (ikm_lua_state_t*)calloc(1, sizeof(ikm_lua_state_t));
    if (!state) {
        return NULL;
    }
    
    state->L = luaL_newstate();
    if (!state->L) {
        free(state);
        return NULL;
    }
    
    luaL_openlibs(state->L);
    
    lua_register(state->L, "print", lua_print);
    lua_register(state->L, "getGameWidth", lua_getGameWidth);
    lua_register(state->L, "getGameHeight", lua_getGameHeight);
    lua_register(state->L, "getFrameCount", lua_getFrameCount);
    lua_register(state->L, "refresh", lua_refresh);
    lua_register(state->L, "commandGetState", lua_commandGetState);
    lua_register(state->L, "setStage", lua_setStage);
    lua_register(state->L, "setCharacter", lua_setCharacter);
    
    state->initialized = 1;
    ikm_log(IKM_LOG_INFO, "Lua state created with 8 registered functions");
    
    return state;
}

void ikm_lua_destroy(ikm_lua_state_t* state) {
    if (state) {
        if (state->L) {
            lua_close(state->L);
        }
        free(state);
    }
}

int ikm_lua_dofile(ikm_lua_state_t* state, const char* filename) {
    if (!state || !state->L) {
        ikm_set_error("Invalid Lua state");
        return IKM_ERROR_UNKNOWN;
    }
    
    ikm_log(IKM_LOG_INFO, "Executing Lua script: %s", filename);
    
    if (luaL_dofile(state->L, filename) != 0) {
        const char* err = lua_tostring(state->L, -1);
        ikm_set_error("Lua error: %s", err);
        ikm_log(IKM_LOG_ERROR, "Lua error: %s", err);
        lua_pop(state->L, 1);
        return IKM_ERROR_UNKNOWN;
    }
    
    return IKM_SUCCESS;
}

int ikm_lua_dostring(ikm_lua_state_t* state, const char* code) {
    if (!state || !state->L) {
        ikm_set_error("Invalid Lua state");
        return IKM_ERROR_UNKNOWN;
    }
    
    if (luaL_dostring(state->L, code) != 0) {
        const char* err = lua_tostring(state->L, -1);
        ikm_set_error("Lua error: %s", err);
        lua_pop(state->L, 1);
        return IKM_ERROR_UNKNOWN;
    }
    
    return IKM_SUCCESS;
}

void* ikm_lua_get_state(ikm_lua_state_t* state) {
    return state ? state->L : NULL;
}
