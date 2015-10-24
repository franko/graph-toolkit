#ifndef AGG_PIXFMT_CONFIG_H
#define AGG_PIXFMT_CONFIG_H

#include "platform/agg_platform_support.h"
#include "agg_gamma_lut.h"
#include "agg_pixfmt_rgb24_lcd.h"

namespace gslshell
{
const agg::pix_format_e pixel_format = agg::pix_format_rgb24;
const bool flip_y = true;

extern agg::pix_format_e  sys_pixel_format;
extern unsigned      sys_bpp;
}

#endif
