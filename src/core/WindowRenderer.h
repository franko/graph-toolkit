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
public:
    WindowRenderer(render_type_e canvas_type, agg::rgba8 bgcol);
    ~WindowRenderer();

    void clearDrawingAreas();
    void addDrawingArea(const agg::trans_affine& m);
    bool attach(drawing *plot, int slot_id);
    void drawSlot(int slot_id, bool update_req);
    void refreshSlot(int slot_id);
    int start_with_id(int window_id);

    void saveSlotImage(int slot_id);
    void restoreSlotImage(int slot_id);

    agg::rect_i getDrawingAreaRect(DrawingArea* drawing_area);

    void onDraw();
    void onResize(int sx, int sy);
private:
    DrawingArea *getDrawingArea(int i);

    void drawArea(DrawingArea* drawing_area, bool dirty);
    void drawAreaQueue(DrawingArea* drawing_area, bool draw_all);

    void scaleToViewportSize(agg::trans_affine& m) {
        trans_affine_compose(m, m_viewport_matrix);
    };

    int m_window_id;
    agg::pod_bvector<DrawingArea*> m_drawing_areas;
    agg::rendering_buffer m_ren_buffer;
    canvas* m_rendering_canvas;
    render_type_e m_render_type;
    agg::rgba8 m_bg_color;
    agg::trans_affine m_viewport_matrix;
    int m_pixel_bpp;
    bool m_flip_y;
    SomeType m_target_window; // To be better defined.
};

#endif
