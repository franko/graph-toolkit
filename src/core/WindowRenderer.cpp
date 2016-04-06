#include <new>

#include "WindowRenderer.h"
#include "rendering_buffer_utils.h"
#include "graph_locks.h"
// #include "platform_support_ext.h"

WindowRenderer::WindowRenderer(render_type_e canvas_type, agg::rgba8 bgcol)
    : m_canvas(nullptr), m_render_type(canvas_type), m_bg_color(bgcol)
{
    const agg::trans_affine identity_matrix;
    addDrawingArea(identity_matrix);
}

WindowRenderer::~WindowRenderer() {
    clearDrawingAreas();
}

DrawingArea* WindowRenderer::getDrawingArea(int i) {
    if (i >= 0 && unsigned(i) < m_drawing_areas.size()) {
        return m_drawing_areas[i];
    }
    return NULL;
}

void DrawingArea::saveImage(agg::rendering_buffer& win_buf, agg::rect_base<int>& r, int img_bpp, bool flip_y) {
    int w = r.x2 - r.x1, h = r.y2 - r.y1;
    int row_len = w * (img_bpp / 8);

    if (layer_buf == 0) {
        unsigned int bufsize = row_len * h;
        layer_buf = new(std::nothrow) unsigned char[bufsize];
    }

    if (layer_buf != 0) {
        layer_img.attach(layer_buf, w, h, flip_y ? -row_len : row_len);
        rendering_buffer_get_region(layer_img, win_buf, r, img_bpp / 8);
    }
}

agg::rect_i WindowRenderer::getDrawingAreaRect(DrawingArea* drawing_area) {
    agg::trans_affine m(drawing_area->matrix);
    scaleToViewportSize(m);
    return rect_of_slot_matrix<int>(m);
}

void WindowRenderer::onResize(int sx, int sy) {
    delete m_canvas;

    if (m_render_type == render_gray_aa) {
        m_canvas = new canvas_gray_aa(m_ren_buffer, sx, sy, m_bg_color);
    } else if (m_render_type == render_subpixel_aa) {
        m_canvas = new canvas_subpixel_aa(m_ren_buffer, sx, sy, m_bg_color);
    } else {
        m_canvas = nullptr;
    }

    for (unsigned i = 0; i < m_drawing_areas.size(); i++) {
        m_drawing_areas[i]->disposeBuffer();
    }

    m_viewport_matrix.sx = sx;
    m_viewport_matrix.sy = sy;
}

void WindowRenderer::drawArea(DrawingArea *drawing_area, bool draw_image) {
    agg::trans_affine drawing_area_matrix(drawing_area->matrix);
    scaleToViewportSize(drawing_area_matrix);

    auto r = rect_of_slot_matrix<int>(drawing_area_matrix);
    m_canvas->clear_box(r);

    if (drawing_area->plot) {
        AGG_LOCK();
        drawing_area->plot->draw(*m_canvas, drawing_area_matrix, &drawing_area->inf);
        AGG_UNLOCK();
    }

    if (draw_image) {
        m_target_window->updateRegion(r);
        // update_region(r);
    }
}

void
WindowRenderer::drawSlot(int slot_id, bool clean_req)
{
    DrawingArea* drawing_area = getDrawingArea(slot_id);
    if (drawing_area && m_canvas) {
        bool redraw = clean_req || drawing_area->plot->need_redraw();
        if (redraw) {
            drawArea(drawing_area, false);
            drawing_area->disposeBuffer();
        }
        drawAreaQueue(drawing_area, redraw);
        drawing_area->valid_rect = true;
    }
}

void WindowRenderer::saveSlotImage(int slot_id) {
    DrawingArea* drawing_area = getDrawingArea(slot_id);
    if (drawing_area != 0) {
        auto r = getDrawingAreaRect(drawing_area);
        drawing_area->saveImage(m_ren_buffer, r, m_pixel_bpp, m_flip_y);
    }
}

void WindowRenderer::restoreSlotImage(int slot_id) {
    DrawingArea* drawing_area = getDrawingArea(slot_id);
    if (drawing_area != nullptr) {
        auto r = getDrawingAreaRect(drawing_area);
        if (drawing_area->layer_buf == nullptr) {
            m_canvas->clear_box(r);
            drawArea(drawing_area, false);
            drawing_area->saveImage(m_ren_buffer, r, m_pixel_bpp, m_flip_y);
        } else {
            agg::rendering_buffer& img = drawing_area->layer_img;
            agg::rendering_buffer& win = m_ren_buffer;
            rendering_buffer_put_region(win, img, r, m_pixel_bpp / 8);
        }
    }
}

void
WindowRenderer::drawAreaQueue(DrawingArea *drawing_area, bool draw_all) {
    agg::trans_affine mtx(drawing_area->matrix);
    scaleToViewportSize(mtx);

    opt_rect<double> rect;

    if (!drawing_area->valid_rect || draw_all)
        rect.set(rect_of_slot_matrix<double>(mtx));

    AGG_LOCK();
    opt_rect<double> draw_rect;
    drawing_area->plot->draw_queue(*m_canvas, mtx, drawing_area->inf, draw_rect);
    rect.add<rect_union>(draw_rect);
    rect.add<rect_union>(drawing_area->dirty_rect);
    drawing_area->dirty_rect = draw_rect;
    AGG_UNLOCK();

    if (rect.is_defined()) {
        const int pad = 4;
        const agg::rect_d& r = rect.rect();
        const agg::rect_i ri(r.x1 - pad, r.y1 - pad, r.x2 + pad, r.y2 + pad);
        m_target_window->updateRegion(ri);
        // update_region (ri);
    }
}

void
WindowRenderer::onDraw() {
    if (m_canvas) {
        for (unsigned i = 0; i < m_drawing_areas.size(); i++) {
            drawArea(m_drawing_areas[i], false);
        }
    }
}

#if 0
void
WindowRenderer::on_resize(int sx, int sy)
{
    this->canvas_window::on_resize(sx, sy);
    for (unsigned i = 0; i < m_drawing_areas.size(); i++) {
        m_drawing_areas[i]->disposeBuffer();
    }
}
#endif

bool WindowRenderer::attach(drawing* plot, int slot_id)
{
    DrawingArea *r = getDrawingArea(slot_id);
    if (r) {
        r->plot = plot;
    }
    return (r != NULL);
}

void WindowRenderer::clearDrawingAreas() {
    for (unsigned k = 0; k < m_drawing_areas.size(); k++) {
        delete m_drawing_areas[k];
    }
}

void WindowRenderer::addDrawingArea(const agg::trans_affine& m) {
    auto drawing_area = new DrawingArea();
    drawing_area->matrix = m;
    m_drawing_areas.add(drawing_area);
}

#if 0
int WindowRenderer::start_with_id(int window_id)
{
    this->lock();
    if (status != canvas_window::running) {
        typedef canvas_window::thread_info thread_info;
        std::auto_ptr<thread_info> inf(new thread_info(this, window_id));
        if (!this->start_new_thread(inf)) {
            this->unlock();
            return window_cannot_start_thread;
        }
    } else {
        this->unlock();
        return window_is_running;
    }
    return 0;
}

class svg_writer {
public:
    svg_writer(FILE* f, double w, double h):
    m_canvas(f, h), m_width(w), m_height(h)
    { }

    void write_header() { m_canvas.write_header(m_width, m_height); }
    void write_end() { m_canvas.write_end(); }

    void call(WindowRenderer::DrawingArea* drawing_area, unsigned i)
    {
        char plot_name[64];
        drawing* p = drawing_area->plot;
        if (p)
        {
            agg::trans_affine mtx = drawing_area->matrix;
            agg::trans_affine_scaling scale(m_width, m_height);
            trans_affine_compose(mtx, scale);
            sprintf(plot_name, "plot%u", i);
            m_canvas.write_group_header(plot_name);
            p->draw(m_canvas, mtx, NULL);
            m_canvas.write_group_end(plot_name);
        }
    }

private:
    canvas_svg m_canvas;
    double m_width, m_height;
};

void WindowRenderer::save_svg(FILE *f, double w, double h)
{
    svg_writer svg_writer(f, w, h);
    svg_writer.write_header();
    for (unsigned i = 0; i < m_drawing_areas.size(); i++) {
        svg_writer.call(m_drawing_areas[i], i + 1);
    }
    svg_writer.write_end();
}
#endif
