#include "bitmap-plot.h"
#include "canvas.h"
#include "colors.h"
#include "pixel_fmt.h"
#include "platform_support_ext.h"
#include "image_write.h"

int
bitmap_save_image(sg_plot *p, const char *fn, unsigned w, unsigned h)
{
    const agg::pix_format_e pixel_format = agg::pix_format_rgb24;
    const bool flip_y = true;

    agg::rendering_buffer rbuf_tmp;
    unsigned row_size = w * 3;
    unsigned buf_size = h * row_size;

    unsigned char* buffer = new(std::nothrow) unsigned char[buf_size];
    if (!buffer) return bitmap_plot_no_memory;

    rbuf_tmp.attach(buffer, w, h, flip_y ? row_size : -row_size);

    canvas_gray_aa can(rbuf_tmp, w, h, colors::white);
    agg::trans_affine mtx(w, 0.0, 0.0, h, 0.0, 0.0);

    agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);
    can.clear_box(r);

    p->draw(can, mtx, NULL);

    bool success = save_image_file (rbuf_tmp, fn, pixel_format);

    if (!success) return bitmap_plot_error_writing_file;

    delete [] buffer;
    return 0;
}
