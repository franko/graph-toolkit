#include "window.h"
#include "platform_support_ext.h"

window::drawing_area *window::get_drawing_area(int i)
{
    if (i >= 0 && unsigned(i) < m_drawing_areas.size()) {
        return m_drawing_areas[i];
    }
    return NULL;
}

void
window::drawing_area::save_image (agg::rendering_buffer& win_buf,
                         agg::rect_base<int>& r,
                         int img_bpp, bool flip_y)
{
    int w = r.x2 - r.x1, h = r.y2 - r.y1;
    int row_len = w * (img_bpp / 8);

    if (layer_buf == 0)
    {
        unsigned int bufsize = row_len * h;
        layer_buf = new(std::nothrow) unsigned char[bufsize];
    }

    if (layer_buf != 0)
    {
        layer_img.attach(layer_buf, w, h, flip_y ? -row_len : row_len);
        rendering_buffer_get_region (layer_img, win_buf, r, img_bpp / 8);
    }
}

void window::draw_area(window::drawing_area& drawing_area, bool draw_image)
{
    agg::trans_affine mtx(drawing_area.matrix);
    this->scale(mtx);

    agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);
    m_canvas->clear_box(r);

    if (drawing_area.plot)
    {
        AGG_LOCK();
        drawing_area.plot->draw(*m_canvas, mtx, &drawing_area.inf);
        AGG_UNLOCK();
    }

    if (draw_image)
        update_region(r);
}

void
window::draw_slot(int slot_id, bool clean_req)
{
    drawing_area *drawing_area = get_drawing_area(slot_id);
    if (drawing_area && m_canvas) {
        bool redraw = clean_req || drawing_area->plot->need_redraw();
        if (redraw) {
            draw_area(*drawing_area, false);
            drawing_area->dispose_buffer();
        }
        draw_area_queue(*drawing_area, redraw);
        drawing_area->valid_rect = true;
    }
}

void
window::save_slot_image(int slot_id)
{
    drawing_area *drawing_area = get_drawing_area(slot_id);
    if (drawing_area != 0) {
        agg::trans_affine mtx(drawing_area->matrix);
        this->scale(mtx);
        agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);
        drawing_area->save_image(this->rbuf_window(), r, this->bpp(), this->flip_y());
    }
}

void
window::restore_slot_image(int slot_id)
{
    drawing_area *drawing_area = get_drawing_area(slot_id);
    if (drawing_area != 0) {
        agg::trans_affine mtx(drawing_area->matrix);
        this->scale(mtx);
        agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);
        if (drawing_area->layer_buf == 0) {
            m_canvas->clear_box(r);
            draw_area(*drawing_area, false);
            drawing_area->save_image(this->rbuf_window(), r, this->bpp(), this->flip_y());
        } else {
            agg::rendering_buffer& img = drawing_area->layer_img;
            agg::rendering_buffer& win = this->rbuf_window();
            rendering_buffer_put_region (win, img, r, this->bpp() / 8);
        }
    }
}

void
window::draw_area_queue(drawing_area& drawing_area, bool draw_all)
{
    agg::trans_affine mtx(drawing_area.matrix);
    this->scale(mtx);

    opt_rect<double> rect;

    if (!drawing_area.valid_rect || draw_all)
        rect.set(rect_of_slot_matrix<double>(mtx));

    AGG_LOCK();
    opt_rect<double> draw_rect;
    drawing_area.plot->draw_queue(*m_canvas, mtx, drawing_area.inf, draw_rect);
    rect.add<rect_union>(draw_rect);
    rect.add<rect_union>(drawing_area.dirty_rect);
    drawing_area.dirty_rect = draw_rect;
    AGG_UNLOCK();

    if (rect.is_defined())
    {
        const int m = 4;
        const agg::rect_base<double>& r = rect.rect();
        const agg::rect_base<int> ri(r.x1 - m, r.y1 - m, r.x2 + m, r.y2 + m);
        update_region (ri);
    }
}

void
window::on_draw()
{
    if (m_canvas) {
        for (unsigned i = 0; i < m_drawing_areas.size(); i++) {
            draw_area(*m_drawing_areas[i], false);
        }
    }
}

void
window::on_resize(int sx, int sy)
{
    this->canvas_window::on_resize(sx, sy);
    for (unsigned i = 0; i < m_drawing_areas.size(); i++) {
        m_drawing_areas[i]->dispose_buffer();
    }
}

bool window::attach(drawing* plot, int slot_id)
{
    drawing_area *r = get_drawing_area(slot_id);
    if (r) {
        r->plot = plot;
    }
    return (r != NULL);
}

void window::clear_drawing_areas() {
    for (unsigned k = 0; k < m_drawing_areas.size(); k++) {
        delete m_drawing_areas[k];
    }
}

void window::add_drawing_area(const agg::trans_affine& m)
{
    drawing_area *r = new drawing_area();
    r->matrix = m;
    m_drawing_areas.add(r);
}

int window::start_with_id(int window_id)
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

    void call(window::drawing_area* drawing_area, unsigned i)
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

void window::save_svg(FILE *f, double w, double h)
{
    svg_writer svg_writer(f, w, h);
    svg_writer.write_header();
    for (unsigned i = 0; i < m_drawing_areas.size(); i++) {
        svg_writer.call(m_drawing_areas[i], i + 1);
    }
    svg_writer.write_end();
}
