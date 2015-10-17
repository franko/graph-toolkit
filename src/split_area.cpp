#include "split_area.h"
#include "tree.h"
#include "split-parser.h"
#include "utils.h"

struct cell { };

static void split_calculate(tree::node<cell, direction_e> *t, agg::pod_bvector<agg::trans_affine>& areas, const agg::trans_affine& m)
{
    cell *r = t->content();
    if (r) {
        areas.add(m);
    }
    int nb = list<tree::node<cell, direction_e>*>::length(t->tree());
    if (nb > 0) {
        double frac = 1 / (double) nb;
        direction_e dir;
        list<tree::node<cell, direction_e>*> *ls = t->tree(dir);
        if (ls) {
            agg::trans_affine lm;
            double* p = (dir == along_x ? &lm.tx : &lm.ty);
            double* s = (dir == along_x ? &lm.sx : &lm.sy);
            *s = frac;
            for ( ; ls; ls = ls->next(), *p += frac) {
                agg::trans_affine sm(lm);
                trans_affine_compose(sm, m);
                split_calculate(ls->content(), areas, sm);
            }
        }
    }
}

bool build_split_regions(agg::pod_bvector<agg::trans_affine>& areas, const char *spec)
{
    ::split<cell>::lexer lexbuf(spec);
    tree::node<cell, direction_e> *parse_tree = ::split<cell>::parse(lexbuf);
    if (!parse_tree) return false;
    agg::trans_affine m0;
    split_calculate(parse_tree, areas, m0);
    delete parse_tree;
    return true;
}
