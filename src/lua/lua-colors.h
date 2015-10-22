#ifndef LUA_COLORS_H
#define LUA_COLORS_H

extern "C" {
#include "lua.h"
}

#include "colors.h"

extern agg::rgba8 color_arg_lookup   (lua_State *L, int index);

#endif
