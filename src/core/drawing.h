#ifndef AGGPLOT_DRAWING_H
#define AGGPLOT_DRAWING_H

#include "canvas-window-cpp.h"
#include "canvas_svg.h"
#include "rect.h"

struct plot_render_info {
    agg::trans_affine active_area;
};

struct drawing {
    drawing() {};
    virtual ~drawing() {}
    virtual void draw(canvas& c, const agg::trans_affine& m, plot_render_info *inf) = 0;
    virtual void draw_queue(canvas& c, const agg::trans_affine& m, const plot_render_info& inf, opt_rect<double>& bbox) = 0;
    virtual bool need_redraw() = 0;
};

template <typename T>
class drawing_adapter : public drawing {
public:
    drawing_adapter(T& d): m_drawing(d) {}
    ~drawing_adapter() {}
    virtual void draw(canvas& c, const agg::trans_affine& m, plot_render_info *inf) { m_drawing.draw(c, m, inf); }
    virtual void draw_queue(canvas& c, const agg::trans_affine& m, const plot_render_info& inf, opt_rect<double>& bbox) { m_drawing.draw_queue(c, m, inf, bbox); }
    virtual bool need_redraw() { return m_drawing.need_redraw(); }
private:
    T& m_drawing;
};

#endif
