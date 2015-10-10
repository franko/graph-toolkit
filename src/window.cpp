#include "window.h"
#include "sg_plot.h"
#include "platform_support_ext.h"

void window::ref::compose(bmatrix& a, const bmatrix& b)
{
    trans_affine_compose (a, b);
};

int window::ref::calculate(window::ref::node* t, const bmatrix& m, int id)
{
    ref *r = t->content();
    if (r)
    {
        r->slot_id = id++;
        r->matrix = m;
    }

    int nb = list<ref::node*>::length(t->tree());

    if (nb > 0)
    {
        double frac = 1 / (double) nb;

        direction_e dir;
        list<ref::node*> *ls = t->tree(dir);
        if (ls)
        {
            bmatrix lm;

            double* p = (dir == along_x ? &lm.tx : &lm.ty);
            double* s = (dir == along_x ? &lm.sx : &lm.sy);

            *s = frac;

            for ( ; ls; ls = ls->next(), *p += frac)
            {
                bmatrix sm(lm);
                window::ref::compose(sm, m);
                id = window::ref::calculate (ls->content(), sm, id);
            }
        }
    }

    return id;
}

void
window::ref::save_image (agg::rendering_buffer& win_buf,
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

window::ref* window::ref_lookup (ref::node *p, int slot_id)
{
    list<ref::node*> *t = p->tree();
    for (/* */; t; t = t->next())
    {
        ref *ref = window::ref_lookup(t->content(), slot_id);
        if (ref)
            return ref;
    }

    ref *ref = p->content();
    if (ref)
    {
        if (ref->slot_id == slot_id)
            return ref;
    }

    return NULL;
}

void window::draw_slot_by_ref(window::ref& ref, bool draw_image)
{
    agg::trans_affine mtx(ref.matrix);
    this->scale(mtx);

    agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);
    m_canvas->clear_box(r);

    if (ref.plot)
    {
        AGG_LOCK();
        ref.plot->draw(*m_canvas, mtx, &ref.inf);
        AGG_UNLOCK();
    }

    if (draw_image)
        update_region(r);
}

void
window::draw_slot(int slot_id, bool clean_req)
{
    ref *ref = window::ref_lookup (this->m_tree, slot_id);
    if (ref && m_canvas)
    {
        bool redraw = clean_req || ref->plot->need_redraw();

        if (redraw)
        {
            draw_slot_by_ref(*ref, false);
            ref->dispose_buffer();
        }

        refresh_slot_by_ref(*ref, redraw);
        ref->valid_rect = true;
    }
}

void
window::save_slot_image(int slot_id)
{
    ref *ref = window::ref_lookup (this->m_tree, slot_id);
    if (ref != 0)
    {
        agg::trans_affine mtx(ref->matrix);
        this->scale(mtx);

        agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);
        ref->save_image(this->rbuf_window(), r, this->bpp(), this->flip_y());
    }
}

void
window::restore_slot_image(int slot_id)
{
    ref *ref = window::ref_lookup (this->m_tree, slot_id);
    if (ref != 0)
    {
        agg::trans_affine mtx(ref->matrix);
        this->scale(mtx);

        agg::rect_base<int> r = rect_of_slot_matrix<int>(mtx);

        if (ref->layer_buf == 0)
        {
            m_canvas->clear_box(r);
            draw_slot_by_ref (*ref, false);
            ref->save_image(this->rbuf_window(), r, this->bpp(), this->flip_y());
        }
        else
        {
            agg::rendering_buffer& img = ref->layer_img;
            agg::rendering_buffer& win = this->rbuf_window();
            rendering_buffer_put_region (win, img, r, this->bpp() / 8);
        }
    }
}

void
window::refresh_slot_by_ref(ref& ref, bool draw_all)
{
    agg::trans_affine mtx(ref.matrix);
    this->scale(mtx);

    opt_rect<double> rect;

    if (!ref.valid_rect || draw_all)
        rect.set(rect_of_slot_matrix<double>(mtx));

    AGG_LOCK();
    opt_rect<double> draw_rect;
    ref.plot->draw_queue(*m_canvas, mtx, ref.inf, draw_rect);
    rect.add<rect_union>(draw_rect);
    rect.add<rect_union>(ref.dirty_rect);
    ref.dirty_rect = draw_rect;
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
    if (m_canvas)
    {
        slot_draw_function draw_func(this);
        this->plot_apply(draw_func);
    }
}

void
window::on_resize(int sx, int sy)
{
    this->canvas_window::on_resize(sx, sy);
    if (m_tree)
    {
        dispose_buffer_function dispose;
        this->plot_apply(dispose);
    }
}

bool
window::split(const char *spec)
{
    ::split<ref>::lexer lexbuf(spec);
    tree::node<ref, direction_e> *parse_tree = ::split<ref>::parse(lexbuf);
    delete m_tree;

    if (parse_tree)
        m_tree = parse_tree;
    else
        m_tree = new tree::leaf<ref, direction_e>();

    bmatrix m0;
    ref::calculate(m_tree, m0, 0);
    return (parse_tree != NULL);
}

static const char *
next_int (const char *str, int& val)
{
    while (*str == ' ')
        str++;
    if (*str == '\0')
        return NULL;

    char *eptr;
    val = strtol (str, &eptr, 10);

    if (eptr == str)
        return NULL;

    while (*eptr == ' ')
        eptr++;
    if (*eptr == ',')
        eptr++;
    return eptr;
}

/* Returns the slot_id or -1 in case of error. */
int window::attach(sg_plot* plot, const char *spec)
{
    ref::node *n = m_tree;
    const char *ptr;
    int k;

    for (ptr = next_int (spec, k); ptr; ptr = next_int (ptr, k))
    {
        list<ref::node*>* list = n->tree();

        if (! list)
            return -1;

        for (int j = 1; j < k; j++)
        {
            list = list->next();
            if (! list)
                return -1;
        }

        n = list->content();
    }

    ref* r = n->content();
    if (! r)
        return -1;

    r->plot = plot;

    return r->slot_id;
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

    void call(window::ref* ref)
    {
        char plot_name[64];
        sg_plot* p = ref->plot;
        if (p)
        {
            agg::trans_affine mtx = ref->matrix;
            agg::trans_affine_scaling scale(m_width, m_height);
            trans_affine_compose(mtx, scale);
            sprintf(plot_name, "plot%u", ref->slot_id + 1);
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
    this->plot_apply(svg_writer);
    svg_writer.write_end();
}
