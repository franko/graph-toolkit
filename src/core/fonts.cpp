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

agg::font_engine_freetype_int32& font_engine()
{
    return global_font_eng;
}

agg::font_cache_manager<agg::font_engine_freetype_int32>& font_manager()
{
    return global_font_man;
}
