#include <lua.h>
#include <lauxlib.h>

#include "lua-compat.h"

// Define a function equivalent to luaL_register with a NULL
// second argument, to write in the table on top of the stack.
void
grtk_lua_register(lua_State *L, const luaL_Reg *regs)
{
#ifdef GRAPH_TK_USE_LUA54
    luaL_setfuncs (L, regs, 0);
#else
    grtk_lua_register (L, NULL, regs);
#endif
}

unsigned int grtk_lua_rawlen(lua_State *L, int index)
{
  /* In reality they are no equivalent because objlen may
   * invoke a meta-methods but we ignore this difference. */
#ifdef GRAPH_TK_USE_LUA54
    return lua_rawlen (L, index);
#else
    return lua_objlen (L, index);
#endif
}

int grtk_lua_setfenv(lua_State *L, int index)
{
#ifdef GRAPH_TK_USE_LUA54
    return lua_setiuservalue (L, index, 1);
#else
    return lua_setfenv (L, index);
#endif
}

int grtk_lua_getfenv(lua_State *L, int index)
{
#ifdef GRAPH_TK_USE_LUA54
    return lua_getiuservalue (L, index, 1);
#else
    return lua_getfenv (L, index);
#endif
}