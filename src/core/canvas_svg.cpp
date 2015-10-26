#include "canvas_svg.h"

void canvas_svg::draw(sg_object& vs, agg::rgba8 c)
{
    int id = m_current_id ++;
    str s = vs.write_svg(id, c, m_height);
    writeln(m_output, s, "   ");
}

void canvas_svg::draw_outline(sg_object& vs, agg::rgba8 c)
{
    int id = m_current_id ++;
    str path;
    svg_property_list* ls = vs.svg_path(path, m_height);
    str s = svg_stroke_path(path, default_stroke_width, id, c, ls);
    svg_property_list::free(ls);
    writeln(m_output, s, "   ");
}

void canvas_svg::draw_outline_alias(sg_object& vs, agg::rgba8 c)
{
    this->draw_outline(vs, c);
}
