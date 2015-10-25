#include "gamma.h"

#ifndef DISABLE_GAMMA_CORR
gamma_type sys_gamma(1.5);
#endif
// Original values for the subpixel color filter. These are quite conservative
// to avoid color fringes.
// agg::lcd_distribution_lut subpixel_lut(3./9., 2./9., 1./9.);

// Slightly more agressive values with more weight to primary channel.
// Some people may notice colorer fringes but fonts looks sharper.
agg::lcd_distribution_lut subpixel_lut(0.448, 0.184, 0.092);
