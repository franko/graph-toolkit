#pragma once

#include "defs.h"

__BEGIN_DECLS

#include <lua.h>

extern void grtk_lua_register(lua_State *L, const luaL_Reg *regs);
extern unsigned int grtk_lua_rawlen(lua_State *L, int index);
extern int grtk_lua_setfenv(lua_State *L, int index);
extern int grtk_lua_getfenv(lua_State *L, int index);

#ifdef GRAPH_TK_USE_LUA54
#define luaL_optint luaL_optinteger
#endif

__END_DECLS

