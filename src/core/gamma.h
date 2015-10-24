#ifndef AGGPLOT_GAMMA_H
#define AGGPLOT_GAMMA_H

#include "agg_gamma_lut.h"
#include "agg_pixfmt_rgb24_lcd.h"

typedef agg::gamma_lut<agg::int8u, agg::int16u, 8, 12> gamma_type;

#ifndef DISABLE_GAMMA_CORR
extern gamma_type sys_gamma;
#endif

extern agg::lcd_distribution_lut subpixel_lut;

#endif
