
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "lua-defs.h"

#include "window_lua.h"
#include "window.h"
#include "window_registry.h"
#include "lua-cpp-utils.h"
#include "sg_plot.h"
#include "gs-types.h"
#include "split-parser.h"
#include "lua-utils.h"
#include "window_hooks.h"
#include "split_area.h"

__BEGIN_DECLS

static int window_free            (lua_State *L);
static int window_split           (lua_State *L);
static int window_save_svg        (lua_State *L);

static const struct luaL_Reg window_functions[] = {
    {"window",        window_new},
    {NULL, NULL}
};

static const struct luaL_Reg window_methods[] = {
    {"show",           window_show          },
    {"attach",         window_attach        },
    {"layout",         window_split         },
    {"update",         window_update        },
    {"close",          window_close         },
    {"save_svg",       window_save_svg      },
    {"__gc",           window_free          },
    {NULL, NULL}
};

__END_DECLS

struct refs_remove_visitor {
    refs_remove_visitor(lua_State *_L, int k): L(_L), window_index(k) {}
    void drawing(drawing *d, int i) {
        window_refs_remove(L, i, window_index);
    }
    lua_State* L;
    int window_index;
};

typedef void (window::*window_slot_method_type)(int slot_id);

int window_generic_oper (lua_State *L, window_slot_method_type method)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    int slot_id = luaL_checkinteger (L, 2);

    win->lock();
    if (win->status == canvas_window::running)
    {
        (win->*method)(slot_id);
    }
    win->unlock();

    return 0;
}

template <class param_type>
int window_generic_oper_ext (lua_State *L,
                             void (window::*method)(int, param_type),
                             param_type param)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    int slot_id = luaL_checkinteger (L, 2);

    win->lock();
    if (win->status == canvas_window::running)
    {
        (win->*method)(slot_id, param);
    }
    win->unlock();

    return 0;
}

static void
show_window(lua_State* L, window* win)
{
    int window_id = window_index_add(L, -1);
    int status = win->start_with_id(window_id);
    if (status == window_is_running) {
        luaL_error (L, "window is already active (reported during window creation)");
    } else if (status == window_cannot_start_thread) {
        luaL_error (L, "error during thread creation (reported during window creation)");
    }
    if (status != 0) {
        window_index_remove(L, window_id);
    }
}

static int
window_is_closed(lua_State *L)
{
    window *win = object_cast<window>(L, 1, GS_WINDOW);
    if (win && win->status == canvas_window::closed) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

static bool split_window(window *win, const char *spec)
{
    agg::pod_bvector<agg::trans_affine> areas;
    if (!build_split_regions(areas, spec)) {
        return false;
    }
    for (unsigned k = 0; k < areas.size(); k++) {
        win->add_drawing_area(areas[k]);
    }
    return true;
}

int
window_new (lua_State *L)
{
    lua_pushcfunction(L, window_is_closed);
    window_index_remove_fun(L);

    const char *spec = lua_tostring (L, 1);
    int defer_show = (lua_gettop(L) >= 2 ? lua_toboolean(L, 2) : 0);

    agg::pix_format_e pixel_format = agg::pix_format_rgb24;
    render_type_e render_type = render_subpixel_aa;
    window *win = push_new_object<window, agg::pix_format_e>(L, GS_WINDOW, pixel_format, render_type);
    if (spec) {
        if (!split_window(win, spec)) {
            return luaL_error(L, "invalid layout specification");
        }
    }
    if (!defer_show) {
        show_window(L, win);
    }
    return 1;
}

int
window_show (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    show_window(L, win);
    return 0;
}

int
window_free (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    win->~window();
    return 0;
}

int
window_split (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    const char *spec = luaL_checkstring (L, 2);

    win->lock();

    refs_remove_visitor refs_remove(L, 1);
    win->accept(refs_remove);

    if (!split_window(win, spec)) {
        if (win->status == canvas_window::running)
            win->do_window_update();
        win->unlock();
        return luaL_error(L, "invalid window subdivision specification");
    }

    if (win->status == canvas_window::running)
    {
        win->on_draw();
        win->do_window_update();
    }

    win->unlock();
    return 0;
}

int
window_attach (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    sg_plot* plot = object_check<sg_plot>(L, 2, GS_PLOT);
    int slot_id = (lua_gettop(L) > 2 ? lua_tointeger(L, 3) : 0);

    win->lock();
    drawing_adapter<sg_plot, manage_owner> *pdrawing = new drawing_adapter<sg_plot, manage_owner>(plot);
    if (win->attach(pdrawing, slot_id)) {
        if (win->status == canvas_window::running)
            win->draw_slot(slot_id, true);
        win->unlock();
        window_refs_add(L, slot_id, 1, 2);
    } else {
        win->unlock();
        luaL_error(L, "invalid slot specification");
    }
    return 0;
}

int
window_slot_update (lua_State *L)
{
    return window_generic_oper_ext (L, &window::draw_slot, true);
}

int
window_slot_refresh (lua_State *L)
{
    return window_generic_oper_ext (L, &window::draw_slot, false);
}

int
window_update (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);

    win->lock();
    if (win->status == canvas_window::running)
    {
        win->on_draw();
        win->do_window_update();
    }
    win->unlock();

    return 0;
}

int
window_save_slot_image (lua_State *L)
{
    return window_generic_oper (L, &window::save_slot_image);
}

int
window_restore_slot_image (lua_State *L)
{
    return window_generic_oper (L, &window::restore_slot_image);
}

int
window_close (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    win->lock();
    if (win->status == canvas_window::running)
        win->close_request();
    win->unlock();
    return 0;
}

int
window_close_wait (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    win->shutdown_close(true);
    return 0;
}

int
window_wait (lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    win->shutdown_close(false);
    return 0;
}

static int
window_save_svg_try(lua_State *L)
{
    window *win = object_check<window>(L, 1, GS_WINDOW);
    const char *filename = lua_tostring(L, 2);
    const double w = luaL_optnumber(L, 3, 600.0);
    const double h = luaL_optnumber(L, 4, 600.0);

    if (!filename) return type_error_return(L, 2, "string");

    unsigned fnlen = strlen(filename);
    if (fnlen <= 4 || strcmp(filename + (fnlen - 4), ".svg") != 0)
    {
        const char* basename = (fnlen > 0 ? filename : "unnamed");
        lua_pushfstring(L, "%s.svg", basename);
        filename = lua_tostring(L, -1);
    }

    FILE* f = fopen(filename, "w");
    if (!f)
    {
        lua_pushfstring(L, "cannot open filename: %s", filename);
        return (-1);
    }
    win->save_svg(f, w, h);
    fclose(f);
    return 0;
}

int
window_save_svg(lua_State *L)
{
    int nret = window_save_svg_try(L);
    if (nret < 0) return lua_error(L);
    return nret;
}

struct window_hooks nat_window_hooks[1] = {{
        window_new, window_show, window_attach,
        window_slot_update, window_slot_refresh,
        window_close_wait, window_wait,
        window_save_slot_image, window_restore_slot_image
    }
};

void
natwin_register(lua_State *L)
{
    app_window_hooks = nat_window_hooks;

    luaL_newmetatable (L, GS_METATABLE(GS_WINDOW));
    lua_pushvalue (L, -1);
    lua_setfield (L, -2, "__index");
    luaL_register (L, NULL, window_methods);
    lua_pop (L, 1);

    luaL_register (L, NULL, window_functions);
}
