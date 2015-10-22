
/* lua-graph.c
 *
 * Copyright (C) 2009 Francesco Abbate
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "lua-graph.h"
#include "fonts.h"
#include "window_registry.h"
#include "lua-draw.h"
#include "lua-text.h"
#include "window_lua.h"
#include "lua-plot.h"
#include "window_hooks.h"

int
stub_window_fn(lua_State *L)
{
    return luaL_error(L, "no windows module loaded");
}

struct window_hooks stub_window_hooks[1] = {{
    stub_window_fn, stub_window_fn, stub_window_fn, stub_window_fn,
    stub_window_fn, stub_window_fn, stub_window_fn, stub_window_fn,
    stub_window_fn,
}};

struct window_hooks *app_window_hooks = stub_window_hooks;

static luaL_Reg dummy_entries[] = { {NULL, NULL} };

static int initialize_fonts_lua(lua_State* L)
{
    int status = initialize_fonts();
    if (status == init_fonts_not_found) {
        luaL_error(L, "cannot find a suitable truetype font");
    } else if (status == init_fonts_load_fail) {
        luaL_error(L, "cannot load truetype font: %s", get_font_name());
    }
    return 0;
}

int
luaopen_graphcore(lua_State *L)
{
    pthread_mutex_init(agg_mutex, NULL);
    window_registry_prepare(L);
    luaL_register(L, "graphcore", dummy_entries);
    draw_register(L);
    text_register(L);
    plot_register(L);
    initialize_fonts_lua(L);
    natwin_register(L);
    return 1;
}
