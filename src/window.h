#ifndef AGGPLOT_WINDOW_H
#define AGGPLOT_WINDOW_H

#include "canvas-window-cpp.h"
#include "drawing.h"
#include "split-parser.h"
#include "lua-graph.h"
#include "tree.h"
#include "colors.h"
#include "list.h"

#include "agg_color_rgba.h"
#include "agg_trans_affine.h"

enum {
    window_is_running = 1,
    window_cannot_start_thread,
};

class window : public canvas_window {
public:
    int window_id;

    typedef agg::trans_affine bmatrix;

    struct ref {
        typedef tree::node<ref, direction_e> node;

        drawing* plot;
        int slot_id;

        plot_render_info inf;
        bmatrix matrix;

        unsigned char *layer_buf;
        agg::rendering_buffer layer_img;

        bool valid_rect;
        opt_rect<double> dirty_rect;

        ref(drawing* p = 0):
            plot(p), matrix(), layer_buf(0), valid_rect(true), dirty_rect()
        {};

        ~ref() {
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

        static void compose(bmatrix& a, const bmatrix& b);
        static int calculate(node *t, const bmatrix& m, int id);
    };

private:
    void draw_slot_by_ref(ref& ref, bool dirty);
    void refresh_slot_by_ref(ref& ref, bool draw_all);

    static ref *ref_lookup (ref::node *p, int slot_id);

    template <class Function>
    void plot_apply_rec(Function& f, ref::node* n) {
        list<ref::node*> *ls;
        for (ls = n->tree(); ls != NULL; ls = ls->next()) {
            this->plot_apply_rec(f, ls->content());
        }
        ref* ref = n->content();
        if (ref) {
            f.call(ref);
        }
    }

    ref::node* m_tree;

public:
    window(agg::rgba8 bgcol= colors::white):
        canvas_window(bgcol), m_tree(0)
    {
        this->split(".");
    }

    ~window() { delete m_tree; }

    template <class Function> void plot_apply(Function& f) { this->plot_apply_rec(f, m_tree); }

    bool split(const char *spec);
    int attach(drawing *plot, const char *spec);
    void draw_slot(int slot_id, bool update_req);
    void refresh_slot(int slot_id);
    int start_with_id(int window_id);

    void save_slot_image(int slot_id);
    void restore_slot_image(int slot_id);

    void draw_slot(int slot_id);

    virtual void on_draw();
    virtual void on_resize(int sx, int sy);

    void save_svg(FILE *f, double w, double h);

private:
    struct slot_draw_function
    {
        slot_draw_function(window* w): win(w) { }
        void call(window::ref* ref) { win->draw_slot_by_ref(*ref, false); }
        window* win;
    };

    struct dispose_buffer_function {
        void call(window::ref* ref) { ref->dispose_buffer(); }
    };
};

#endif
