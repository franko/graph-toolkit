
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "fonts.h"

agg::font_engine_freetype_int32 global_font_eng;
agg::font_cache_manager<agg::font_engine_freetype_int32> global_font_man(global_font_eng);

int initialize_fonts()
{
    const char* font_name = get_font_name();
    if (!font_name)
        return init_fonts_not_found;
    agg::glyph_rendering gren = agg::glyph_ren_outline;
    if (!global_font_eng.load_font(font_name, 0, gren)) {
        return init_fonts_load_fail;
    }
    global_font_eng.hinting(true);
    return init_fonts_success;
}

int initialize_fonts_lua(lua_State* L)
{
    int status = initialize_fonts();
    if (status == init_fonts_not_found) {
        luaL_error(L, "cannot find a suitable truetype font");
    } else if (status == init_fonts_load_fail) {
        luaL_error(L, "cannot load truetype font: %s", get_font_name());
    }
    return 0;
}

agg::font_engine_freetype_int32& font_engine()
{
    return global_font_eng;
}

agg::font_cache_manager<agg::font_engine_freetype_int32>& font_manager()
{
    return global_font_man;
}
