#include "bitmap-plot.h"
#include "canvas.h"
#include "colors.h"
#include "pixel_fmt.h"
#include "agg-pixfmt-config.h"
#include "platform_support_ext.h"
#include "image_write.h"

int
bitmap_save_image(sg_plot *p, const char *fn, unsigned w, unsigned h)
{
    agg::rendering_buffer rbuf_tmp;
    unsigned row_size = w * (LIBGRAPH_BPP / 8);
    unsigned buf_size = h * row_size;

    unsigned char* buffer = new(std::nothrow) unsigned char[buf_size];
    if (!buffer) return bitmap_plot_no_memory;

    rbuf_tmp.attach(buffer, w, h, gslshell::flip_y ? row_size : -row_size);

    canvas can(rbuf_tmp, w, h, colors::white);
    agg::trans_affine mtx(w, 0.0, 0.0, h, 0.0, 0.0);

    agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);
    can.clear_box(r);

    p->draw(can, mtx, NULL);

    bool success = save_image_file (rbuf_tmp, fn, gslshell::pixel_format);

    if (!success) return bitmap_plot_error_writing_file;

    delete [] buffer;
    return 0;
}
