#ifndef BITMAP_PLOT_H
#define BITMAP_PLOT_H

#include "sg_plot.h"

enum {
	bitmap_plot_success = 0,
	bitmap_plot_no_memory,
	bitmap_plot_error_writing_file,
};

extern int bitmap_save_image(sg_plot *p, const char *fn, unsigned w, unsigned h);

#endif
