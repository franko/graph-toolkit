#ifndef AGGPLOT_WINDOW_RENDERER_H
#define AGGPLOT_WINDOW_RENDERER_H

// #include "canvas-window-cpp.h"
#include "drawing.h"
// #include "graph_locks.h"
#include "colors.h"

#include "agg_color_rgba.h"
#include "agg_trans_affine.h"

enum {
    window_is_running = 1,
    window_cannot_start_thread,
};

struct DrawingArea {
    drawing* plot;

    plot_render_info inf;
    agg::trans_affine matrix;

    unsigned char *layer_buf;
    agg::rendering_buffer layer_img;

    bool valid_rect;
    opt_rect<double> dirty_rect;

    DrawingArea(drawing* p = 0):
        plot(p), matrix(), layer_buf(0), valid_rect(true), dirty_rect()
    {};

    ~DrawingArea() {
        delete plot;
        delete[] layer_buf;
    }

    void disposeBuffer()
    {
        valid_rect = false;
        delete[] layer_buf;
        layer_buf = 0;
    }

    void saveImage(agg::rendering_buffer& winbuf, agg::rect_base<int>& r, int bpp, bool flip_y);
};

class WindowRenderer {
private:
    void drawArea(DrawingArea& drawing_area, bool dirty);
    void drawAreaQueue(DrawingArea& drawing_area, bool draw_all);

    agg::pod_bvector<DrawingArea*> m_drawing_areas;

public:
    WindowRenderer(render_type_e canvas_type, agg::rgba8 bgcol);

    ~WindowRenderer() {
        clear_drawing_areas();
    }

    void clear_drawing_areas();
    void addDrawingArea(const agg::trans_affine& m);
    bool attach(drawing *plot, int slot_id);
    void drawSlot(int slot_id, bool update_req);
    void refreshSlot(int slot_id);
    int start_with_id(int window_id);

    void saveSlotImage(int slot_id);
    void restoreSlotImage(int slot_id);

    void draw_slot(int slot_id);

    virtual void on_draw();
    void onResize(int sx, int sy);

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
    DrawingArea *getDrawingArea(int i);

    void scaleToViewportSize(agg::trans_affine& m) {
        trans_affine_compose(m, m_viewport_matrix);
    };

    int m_window_id;
    agg::rendering_buffer m_ren_buffer;
    canvas* m_rendering_canvas;
    render_type_e m_render_type;
    agg::rgba8 m_bg_color;
    agg::trans_affine m_viewport_matrix;
    SomeType m_target_window; // To be better defined.
    SomeOtherType m_format_info;
};

#endif
