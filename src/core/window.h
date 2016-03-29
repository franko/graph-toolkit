#ifndef AGGPLOT_WINDOW_H
#define AGGPLOT_WINDOW_H

#include "canvas-window-cpp.h"
#include "drawing.h"
#include "graph_locks.h"
#include "colors.h"

#include "agg_color_rgba.h"
#include "agg_trans_affine.h"

enum {
    window_is_running = 1,
    window_cannot_start_thread,
};

class window : public canvas_window {
public:
    int window_id;

    struct drawing_area {
        drawing* plot;

        plot_render_info inf;
        agg::trans_affine matrix;

        unsigned char *layer_buf;
        agg::rendering_buffer layer_img;

        bool valid_rect;
        opt_rect<double> dirty_rect;

        drawing_area(drawing* p = 0):
            plot(p), matrix(), layer_buf(0), valid_rect(true), dirty_rect()
        {};

        ~drawing_area() {
            delete plot;
            delete[] layer_buf;
        }

        void dispose_buffer()
        {
            valid_rect = false;
            delete[] layer_buf;
            layer_buf = 0;
        }

        void save_image (agg::rendering_buffer& winbuf, agg::rect_base<int>& r,
                         int bpp, bool flip_y);
    };

private:
    void draw_area(drawing_area& drawing_area, bool dirty);
    void draw_area_queue(drawing_area& drawing_area, bool draw_all);

    agg::pod_bvector<drawing_area*> m_drawing_areas;

public:
    window(agg::pix_format_e pixel_format, render_type_e render_type, agg::rgba8 bgcol= colors::white):
        canvas_window(pixel_format, render_type, bgcol)
    {
        const agg::trans_affine identity_matrix;
        add_drawing_area(identity_matrix);
    }

    ~window() {
        clear_drawing_areas();
    }

    void clear_drawing_areas();
    void add_drawing_area(const agg::trans_affine& m);
    bool attach(drawing *plot, int slot_id);
    void draw_slot(int slot_id, bool update_req);
    int start_with_id(int window_id);

    void save_slot_image(int slot_id);
    void restore_slot_image(int slot_id);

    void draw_slot(int slot_id);

    virtual void on_draw();
    virtual void on_resize(int sx, int sy);

    template <typename T>
    void accept(T& visitor) {
        const int n = m_drawing_areas.size();
        for (int k = 0; k < n; k++) {
            drawing *p = m_drawing_areas[k]->plot;
            if (p) {
                visitor.drawing(p, k);
            }
        }
    }

    void save_svg(FILE *f, double w, double h);

private:
    drawing_area *get_drawing_area(int i);
};

#endif
