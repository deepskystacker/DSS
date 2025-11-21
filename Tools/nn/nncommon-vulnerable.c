/******************************************************************************
 *
 * File:           nncommon-vulnerable.c
 *
 * Created:        05/08/2004
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Stuff for NN interpolation library found to be vulnerable
 *                 from -O2 optimisation by gcc.
 *
 * Description:    None
 *
 * Revisions:      07/04/2005 PS: Changed numerics to force underflow when
 *                 there is a substantial loss of precision.
 *                 15/04/2005 PS: Further improved numerics. Looks like it
 *                 became pretty good, so I had to split circle_build() into
 *                 circle_build1() -- for general use, and circle_build2() --
 *                 for use in nnpi_triangle_process() (it signals loss of
 *                 precision in Watson's algorithm).
 *
 *****************************************************************************/

#include <stdlib.h>
#include <math.h>
#include "nan.h"
#include "nn.h"
#include "nncommon.h"

#define MULT 1.0e+7

int circle_build1(circle* c, point* p1, point* p2, point* p3)
{
    double x2 = p2->x - p1->x;
    double y2 = p2->y - p1->y;
    double x3 = p3->x - p1->x;
    double y3 = p3->y - p1->y;

    double denom = x2 * y3 - y2 * x3;
    double frac;

    if (denom == 0.0) {
        c->x = NaN;
        c->y = NaN;
        c->r = NaN;
        return 0;
    }

    frac = (x2 * (x2 - x3) + y2 * (y2 - y3)) / denom;
    c->x = (x3 + frac * y3) / 2.0;
    c->y = (y3 - frac * x3) / 2.0;
    c->r = hypot(c->x, c->y);
    c->x += p1->x;
    c->y += p1->y;

    return 1;
}

int circle_build2(circle* c, point* p1, point* p2, point* p3)
{
    double x2 = p2->x - p1->x;
    double y2 = p2->y - p1->y;
    double x3 = p3->x - p1->x;
    double y3 = p3->y - p1->y;

    double denom = x2 * y3 - y2 * x3;
    double frac;

    if (denom == 0) {
        c->x = NaN;
        c->y = NaN;
        c->r = NaN;
        return 0;
    }

    frac = (x2 * (x2 - x3) + y2 * (y2 - y3)) / denom;
    c->x = (x3 + frac * y3) / 2.0;
    c->y = (y3 - frac * x3) / 2.0;
    c->r = hypot(c->x, c->y);
    if (c->r > (fabs(x2) + fabs(x3) + fabs(y2) + fabs(y3)) * MULT) {
        c->x = NaN;
        c->y = NaN;
    } else {
        c->x += p1->x;
        c->y += p1->y;
    }

    return 1;
}
