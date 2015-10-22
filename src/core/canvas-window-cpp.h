#ifndef CANVAS_WINDOW_CPP_H
#define CANVAS_WINDOW_CPP_H

#include <memory>
#include <pthread.h>

#include "agg-pixfmt-config.h"
#include "platform_support_ext.h"
#include "agg_trans_affine.h"
#include "agg_color_rgba.h"

#include "defs.h"
#include "canvas.h"
#include "utils.h"

class canvas_window : public platform_support_ext {
protected:
    canvas *m_canvas;
    agg::rgba8 m_bgcolor;

    agg::trans_affine m_matrix;

    pthread_t m_thread;

public:

    struct thread_info {
        canvas_window *win;
        int window_id;
        thread_info (canvas_window *win, int id): win(win), window_id(id) {};
    };

    enum win_status_e { not_ready, running, error, closed };

    enum win_status_e status;

    canvas_window(agg::rgba8 bgcol):
        platform_support_ext(gslshell::pixel_format, true),
        m_canvas(NULL), m_bgcolor(bgcol), m_matrix(), status(not_ready)
    { };

    virtual ~canvas_window()
    {
        if (m_canvas)
            delete m_canvas;
    };

    virtual void on_init();
    virtual void on_resize(int sx, int sy);

    void shutdown_close(bool send_close_request);

    bool start_new_thread (std::auto_ptr<thread_info>& inf);

    void scale (agg::trans_affine& m) {
        trans_affine_compose (m, m_matrix);
    };
};

#endif
