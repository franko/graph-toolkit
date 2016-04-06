#ifndef GT_WINDOW_UPDATE_H
#define GT_WINDOW_UPDATE_H

#include "agg_basics.h"

struct WindowUpdate {
    virtual void updateRegion(const agg::rect_i& r) = 0;
};

#endif
