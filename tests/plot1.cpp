#include <unistd.h>

#include "window-cpp.h"
#include "path.h"

static double my_fun_s(double x) { return (x*x + 0.2) * sin(x * 30); }
static double my_fun_c(double x) { return (x*x + 0.2) * cos(x * 30); }

draw::path *build_curve(double (*f)(double))
{
    const int N = 256;
    draw::path *vs = new draw::path();
    agg::path_storage& p = vs->self();
    p.move_to(-1.0, f(-1.0));
    for (int i = 0; i < N; i++) {
        double x = -1.0 + (i + 1) * 2.0 / N;
        p.line_to(x, f(x));
    }
    return vs;
}

int main()
{
    pthread_mutex_init(agg_mutex, NULL);
    int status = initialize_fonts();
    if (status != init_fonts_success) return 1;

    window *win = new window();
    plot *p = new plot_auto();
    draw::path *line = build_curve(my_fun_s);
    sg_object *sline = new trans::scaling(line);
    agg::rgba8 red(190,10,10,255);
    p->add(sline, red, true);
    p->commit_pending_draw();
    int slot_id = win->attach(p, "");
    win->start_with_id(1);

    sleep(2);

    draw::path *line_c = build_curve(my_fun_c);
    agg::rgba8 blue(10,10,190,255);
    sg_object *sline_c = new trans::scaling(line_c);
    p->add(sline_c, blue, true);
    win->draw_slot(slot_id, false);
    p->commit_pending_draw();

    sleep(10);
    return 0;
}
