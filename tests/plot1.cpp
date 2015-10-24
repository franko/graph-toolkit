#include <unistd.h>

#include "window.h"
#include "fonts.h"
#include "plot-auto.h"
#include "path.h"

static double my_fun_s(double x) { return (x*x + 0.2) * sin(x * 30); }
static double my_fun_c(double x) { return (x*x + 0.2) * cos(x * 30); }

draw::path *build_curve(double (*f)(double), double x1, double x2, int n)
{
    draw::path *vs = new draw::path();
    agg::path_storage& p = vs->self();
    p.move_to(x1, f(x1));
    for (int i = 0; i < n; i++) {
        double x = x1 + (i + 1) * (x2 - x1) / n;
        p.line_to(x, f(x));
    }
    return vs;
}

int main()
{
    pthread_mutex_init(agg_mutex, NULL);
    int status = initialize_fonts();
    if (status != init_fonts_success) return 1;

    window *win = new window(agg::pix_format_rgb24);
    plot_auto *p = new plot_auto();
    draw::path *line = build_curve(my_fun_s, -1.0, 1.0, 256);
    sg_object *sline = new trans::scaling(line);
    agg::rgba8 red(190,10,10,255);
    p->add(sline, red, true);
    p->commit_pending_draw();
    int slot_id = 0;
    win->attach(new drawing_adapter<plot_auto>(*p), slot_id);
    win->start_with_id(1);

    sleep(2);

    draw::path *line_c = build_curve(my_fun_c, -0.4, 0.4, 128);
    agg::rgba8 blue(10,10,190,255);
    sg_object *sline_c = new trans::scaling(line_c);
    p->add(sline_c, blue, true);
    win->draw_slot(slot_id, false);
    p->commit_pending_draw();

    sleep(10);
    return 0;
}
