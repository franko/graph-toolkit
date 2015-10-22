#ifndef AGGPLOT_SPLIT_AREA_H
#define AGGPLOT_SPLIT_AREA_H

#include "agg_trans_affine.h"
#include "agg_array.h"

extern bool build_split_regions(agg::pod_bvector<agg::trans_affine>& areas, const char *spec);

#endif
