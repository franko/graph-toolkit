#ifndef FONTS_H
#define FONTS_H

#include "defs.h"

#include "agg_font_freetype.h"

__BEGIN_DECLS

enum {
    init_fonts_success = 0,
    init_fonts_not_found,
    init_fonts_load_fail,
};

extern int initialize_fonts();

extern const char *get_font_name();
extern const char *get_console_font_name();

__END_DECLS

extern agg::font_engine_freetype_int32& font_engine();
extern agg::font_cache_manager<agg::font_engine_freetype_int32>& font_manager();

#endif
