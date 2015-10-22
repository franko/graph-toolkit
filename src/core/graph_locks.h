#ifndef AGGPLOT_GRAPH_LOCKS_H
#define AGGPLOT_GRAPH_LOCKS_H

#include <pthread.h>

#include "defs.h"

__BEGIN_DECLS

extern pthread_mutex_t agg_mutex[1];

#define AGG_LOCK() pthread_mutex_lock (agg_mutex);
#define AGG_UNLOCK() pthread_mutex_unlock (agg_mutex);

__END_DECLS

#endif
