#ifndef CANVAS_SVG_H
#define CANVAS_SVG_H

#include <stdio.h>

#include <agg_trans_affine.h>
#include <agg_color_rgba.h>

#include "defs.h"
#include "strpp.h"
#include "sg_object.h"
#include "canvas.h"
#include "draw_svg.h"

static const char *svg_header =                                                \
        "<?xml version=\"1.0\" standalone=\"no\"?>\n"                                \
        "<!-- Created using GSL Shell -->\n"                                        \
        "<svg\n"                                                                \
        "   xmlns=\"http://www.w3.org/2000/svg\"\n"                                \
        "   version=\"1.1\"\n"                                                \
        "   width=\"%g\"\n"                                                        \
        "   height=\"%g\"\n"                                                        \
        "   font-family=\"Helvetica\">\n";

static const char *svg_end = "</svg>\n";

class canvas_svg : public canvas {
    enum { default_stroke_width = 1 };

public:
    canvas_svg(FILE *f, double height):
        m_output(f), m_height(height), m_current_id(0)  { }

    virtual void clip_box(const agg::rect_base<int>& clip) { }
    virtual void reset_clipping() { }
    virtual void clear_box(const agg::rect_base<int>& r) { }

    virtual void draw(sg_object& vs, agg::rgba8 c);
    virtual void draw_outline(sg_object& vs, agg::rgba8 c);
    virtual void draw_outline_alias(sg_object& vs, agg::rgba8 c);

    void write_header(double w, double h) {
        fprintf(m_output, svg_header, w, h);
    }
    void write_end() {
        fputs(svg_end, m_output);
    }

    void write_group_header(const char* id) {
        fprintf(m_output, "<g id=\"%s\">\n", id);
    }

    void write_group_end(const char* id) {
        fprintf(m_output, "</g>\n");
    }

    static void writeln(FILE* f, str& s, const char* indent = 0) {
        if (str_is_null(&s))
            return;
        if (indent)
            fputs(indent, f);
        fprintf(f, "%s\n", s.cstr());
    }

private:
    FILE *m_output;
    double m_height;
    int m_current_id;
};

#endif
