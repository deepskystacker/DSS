/******************************************************************************
 *
 * File:           linear.c
 *
 * Created:        04/08/2000
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        2D linear interpolation
 *
 * Description:    `lpi' -- "Linear Point Interpolator" -- is
 *                 a structure for conducting linear interpolation on a given
 *                 data on a "point-to-point" basis. It interpolates linearly
 *                 within each triangle resulted from the Delaunay
 *                 triangluation of input data. `lpi' is much
 *                 faster than all Natural Neighbours interpolators in `nn'
 *                 library.
 *
 * Revisions:      None
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "nan.h"
#include "nn.h"
#include "nncommon.h"
#include "delaunay_internal.h"

typedef struct {
    double w[3];
} lweights;

struct lpi {
    delaunay* d;
    lweights* weights;
    int first_id;
};

int delaunay_xytoi(delaunay* d, point* p, int seed);

/* Builds linear interpolator.
 *
 * @param d Delaunay triangulation
 * @return Linear interpolator
 */
lpi* lpi_build(delaunay* d)
{
    int i;
    lpi* l = malloc(sizeof(lpi));

    l->d = d;
    l->weights = malloc(d->ntriangles * sizeof(lweights));
    l->first_id = -1;

    for (i = 0; i < d->ntriangles; ++i) {
        triangle* t = &d->triangles[i];
        lweights* lw = &l->weights[i];
        double x0 = d->points[t->vids[0]].x;
        double y0 = d->points[t->vids[0]].y;
        double z0 = d->points[t->vids[0]].z;
        double x1 = d->points[t->vids[1]].x;
        double y1 = d->points[t->vids[1]].y;
        double z1 = d->points[t->vids[1]].z;
        double x2 = d->points[t->vids[2]].x;
        double y2 = d->points[t->vids[2]].y;
        double z2 = d->points[t->vids[2]].z;
        double x02 = x0 - x2;
        double y02 = y0 - y2;
        double z02 = z0 - z2;
        double x12 = x1 - x2;
        double y12 = y1 - y2;
        double z12 = z1 - z2;

        if (fabs(y12) > fabs(x12)) {
            double y0212 = y02 / y12;

            lw->w[0] = (z02 - z12 * y0212) / (x02 - x12 * y0212);
            lw->w[1] = (z12 - lw->w[0] * x12) / y12;
            lw->w[2] = (z2 - lw->w[0] * x2 - lw->w[1] * y2);
        } else {
            double x0212 = x02 / x12;

            lw->w[1] = (z02 - z12 * x0212) / (y02 - y12 * x0212);
            lw->w[0] = (z12 - lw->w[1] * y12) / x12;
            lw->w[2] = (z2 - lw->w[0] * x2 - lw->w[1] * y2);
        }
    }

    return l;
}

/* Destroys linear interpolator.
 *
 * @param l Structure to be destroyed
 */
void lpi_destroy(lpi* l)
{
    free(l->weights);
    free(l);
}

/* Finds linearly interpolated value in a point.
 *
 * @param l Linear interpolation
 * @param p Point to be interpolated (p->x, p->y -- input; p->z -- output)
 */
void lpi_interpolate_point(lpi* l, point* p)
{
    delaunay* d = l->d;
    int tid = delaunay_xytoi(d, p, l->first_id);

    if (tid >= 0) {
        lweights* lw = &l->weights[tid];

        l->first_id = tid;
        p->z = p->x * lw->w[0] + p->y * lw->w[1] + lw->w[2];
    } else
        p->z = NaN;
}

/* Linearly interpolates data in an array of points.
 *
 * @param nin Number of input points
 * @param pin Array of input points [pin]
 * @param nout Number of ouput points
 * @param pout Array of output points [nout]
 */
void lpi_interpolate_points(delaunay* d, int nout, point pout[])
{
    lpi* l = lpi_build(d);
    int seed = 0;
    int i;

    if (nn_verbose) {
        fprintf(stderr, "xytoi:\n");
        for (i = 0; i < nout; ++i) {
            point* p = &pout[i];

            fprintf(stderr, "(%.7g,%.7g) -> %d\n", p->x, p->y, delaunay_xytoi(d, p, seed));
        }
    }

    for (i = 0; i < nout; ++i)
        lpi_interpolate_point(l, &pout[i]);

    if (nn_verbose) {
        fprintf(stderr, "output:\n");
        for (i = 0; i < nout; ++i) {
            point* p = &pout[i];;
            fprintf(stderr, "  %d:%15.7g %15.7g %15.7g\n", i, p->x, p->y, p->z);
        }
    }

    lpi_destroy(l);
}
