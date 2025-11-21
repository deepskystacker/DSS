/*****************************************************************************
 *
 * File:           nnpi.c
 *
 * Created:        15/11/2002
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Code for:
 *                 -- Natural Neighbours Point Interpolator
 *                 -- Natural Neighbours Point Hashing Interpolator
 *
 * Description:    `nnpi' -- "Natural Neighbours Point Interpolator" -- is a
 *                 structure for conducting Natural Neighbours interpolation on
 *                 a "point-to-point" basis. Because it calculates weights for
 *                 each output point, `nnpi' does not take advantage of
 *                 repeated interpolations when locations of input and output
 *                 data points do not change -- use `nnhpi' or `nnai' in these
 *                 cases.
 *
 *                 `nnhpi' -- "Natural Neighbours Hashing Point Interpolator"
 *                 is a structure for conducting repeated Natural Neighbours
 *                 interpolations when (i) input data points have constant
 *                 locations and (ii) locations of output data points are often
 *                 repeated.
 *
 *                 For Sibson NN interpolation this code uses Dave Watson's
 *                 algorithm (Watson, D. F. nngridr: An implementation of
 *                 natural neighbour interpolation. David Watson, 1994).
 *
 *                 For non-Sibsonian NN interpolation this code uses Eq.(40)
 *                 from Sukumar, N., Moran, B., Semenov, A. Yu, and
 *                 Belikov V. V. Natural neighbour Galerkin methods.
 *                 Int. J. Numer. Meth. Engng 2001, v.50: 1­27.
 *
 *
 * Revisions:      01/04/2003 PS: modified nnpi_triangle_process(): for
 *                   Sibson interpolation, if circle_build fails(), now a
 *                   local copy of a point is moved slightly rather than the
 *                   data point itself. The later approach have found leading
 *                   to inconsistencies of the new point position with the 
 *                   earlier built triangulation.
 *                 22/11/2006 PS: introduced special treatment for big circles
 *                   by moving their centers in a certain way, closer to the
 *                   data points; added hashtable nn->bad to account for
 *                   such events. Modified _nnpi_calculate_weights() to handle
 *                   the case when instead of being in between two data points
 *                   the interpolation point is close to a data point.
 *                 30/10/2007 PS: Modified treatment of degenerate cases in 
 *                   nnpi_triangle_process(), many thanks to John Gerschwitz,
 *                   Petroleum Geo-Services, for exposing the defect introduced
 *                   in v. 1.69. Changed EPS_SAME from 1.0e-15 to 1.0e-8. Also
 *                   modified nnpi_calculate_weights().
 *                 30/10/2007 PS: Got rid of memset(nn->d->flags, ...) in
 *                   nnpi_reset(). The flags are reset now internally on return
 *                   from delaunay_circles_find(). This is very important
 *                   for large datasets, many thanks to John Gerschwitz,
 *                   Petroleum Geo-Services, for identifying the problem.
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "nan.h"
#include "hash.h"
#include "nn.h"
#include "nncommon.h"
#include "istack_internal.h"
#include "delaunay_internal.h"

struct nnpi {
    dsearch* ds;
    double wmin;
    int n;                      /* number of points processed */
    /*
     * work variables 
     */
    int ncircles;
    int nvertices;
    int nallocated;
    int* vertices;              /* vertex indices */
    double* weights;
    double dx, dy;              /* vertex perturbation */
    hashtable* bad;             /* ids of vertices that require a special
                                 * treatment */
};

#define NSTART 10
#define NINC 10
#define EPS_SHIFT 1.0e-5
#define BIGNUMBER 1.0e+100
#define EPS_WMIN 1.0e-6
#define HT_SIZE 100
#define EPS_SAME 1.0e-8

/* Creates Natural Neighbours point interpolator.
 *
 * @param d Delaunay triangulation
 * @return Natural Neighbours interpolation
 */
nnpi* nnpi_create(delaunay* d)
{
    nnpi* nn = malloc(sizeof(nnpi));

    nn->ds = dsearch_build(d);
    nn->wmin = -DBL_MAX;
    nn->n = 0;
    nn->ncircles = 0;
    nn->vertices = calloc(NSTART, sizeof(int));
    nn->weights = calloc(NSTART, sizeof(double));
    nn->nvertices = 0;
    nn->nallocated = NSTART;
    nn->bad = NULL;

    return nn;
}

/* Destroys Natural Neighbours point interpolator.
 *
 * @param nn Structure to be destroyed
 */
void nnpi_destroy(nnpi* nn)
{
    dsearch_destroy(nn->ds);
    free(nn->weights);
    free(nn->vertices);
    free(nn);
}

void nnpi_reset(nnpi* nn)
{
    nn->nvertices = 0;
    nn->ncircles = 0;
    if (nn->bad != NULL) {
        ht_destroy(nn->bad);
        nn->bad = NULL;
    }
}

static void nnpi_add_weight(nnpi* nn, int vertex, double w)
{
    int i;

    /*
     * find whether the vertex is already in the list 
     */
    /*
     * For clustered data the number of natural neighbours for a point may
     * be quite big ( a few hundreds in example 2), and using hashtable here
     * could accelerate things a bit. However, profiling shows that use of
     * linear search is not a major issue.
     */
    for (i = 0; i < nn->nvertices; ++i)
        if (nn->vertices[i] == vertex)
            break;

    if (i == nn->nvertices) {   /* not in the list */
        /*
         * get more memory if necessary 
         */
        if (nn->nvertices == nn->nallocated) {
            nn->vertices = realloc(nn->vertices, (nn->nallocated + NINC) * sizeof(int));
            nn->weights = realloc(nn->weights, (nn->nallocated + NINC) * sizeof(double));
            nn->nallocated += NINC;
        }

        /*
         * add the vertex to the list 
         */
        nn->vertices[i] = vertex;
        nn->weights[i] = w;
        nn->nvertices++;
    } else                      /* in the list */
        nn->weights[i] += w;
}

/* This is a central procedure for the Natural Neighbours interpolation. It
 * uses the Watson's algorithm for the required areas calculation and implies
 * that the vertices of the delaunay triangulation are listed in uniform
 * (clockwise or counterclockwise) order.
 */
static void nnpi_triangle_process(nnpi* nn, point* p, int i)
{
    delaunay* d = nn->ds->d;
    triangle* t = &d->triangles[i];
    circle* c = &d->circles[i];
    circle cs[3];
    int j;

    /*
     * There used to be a useful assertion here:
     *
     * assert(circle_contains(c, p));
     *
     * I removed it after introducing flag `contains' to 
     * delaunay_circles_find(). It looks like the code is robust enough to
     * run without this assertion.
     */

    /*
     * Sibson interpolation by using Watson's algorithm 
     */
    for (j = 0; j < 3; ++j) {
        int j1 = (j + 1) % 3;
        int j2 = (j + 2) % 3;
        int v1 = t->vids[j1];
        int v2 = t->vids[j2];

        if (!circle_build2(&cs[j], &d->points[v1], &d->points[v2], p)) {
            point* p1 = &d->points[v1];
            point* p2 = &d->points[v2];

            if ((fabs(p1->x - p->x) + fabs(p1->y - p->y)) / c->r < EPS_SAME) {
                /*
                 * if (p1->x == p->x && p1->y == p->y) {
                 */
                nnpi_add_weight(nn, v1, BIGNUMBER);
                return;
            } else if ((fabs(p2->x - p->x) + fabs(p2->y - p->y)) / c->r < EPS_SAME) {
                /*
                 * } else if (p2->x == p->x && p2->y == p->y) {
                 */
                nnpi_add_weight(nn, v2, BIGNUMBER);
                return;
            }
        }
    }

    for (j = 0; j < 3; ++j) {
        int j1 = (j + 1) % 3;
        int j2 = (j + 2) % 3;
        double det = ((cs[j1].x - c->x) * (cs[j2].y - c->y) - (cs[j2].x - c->x) * (cs[j1].y - c->y));

        if (isnan(det)) {
            /*
             * Here, if the determinant is NaN, then the interpolation point
             * is almost in between two data points. This case is difficult to
             * handle robustly because the areas (determinants) calculated by
             * Watson's algorithm are obtained as a diference between two big
             * numbers. This case is handled here in the following way.
             *
             * If a circle is recognised as very large in circle_build2(), then
             * its parameters are replaced by NaNs, which results in the
             * variable `det' above being NaN.
             * 
             * When this happens inside convex hall of the data, there is
             * always a triangle on another side of the edge, processing of
             * which also produces an invalid circle. Processing of this edge
             * yields two pairs of infinite determinants, with singularities 
             * of each pair cancelling if the point moves slightly off the edge.
             *
             * Each of the determinants corresponds to the (signed) area of a
             * triangle, and an inifinite determinant corresponds to the area of
             * a triangle with one vertex moved to infinity. "Subtracting" one
             * triangle from another within each pair yields a valid
             * quadrilateral (in fact, a trapezoid). The doubled area of these
             * quadrilaterals is calculated in the cycle over ii below.
             */
            int j1bad = isnan(cs[j1].x);
            int key[2];
            double* v = NULL;

            key[0] = t->vids[j];

            if (nn->bad == NULL)
                nn->bad = ht_create_i2(HT_SIZE);

            key[1] = (j1bad) ? t->vids[j2] : t->vids[j1];
            v = ht_find(nn->bad, &key);

            if (v == NULL) {
                v = malloc(8 * sizeof(double));
                if (j1bad) {
                    v[0] = cs[j2].x;
                    v[1] = cs[j2].y;
                } else {
                    v[0] = cs[j1].x;
                    v[1] = cs[j1].y;
                }
                v[2] = c->x;
                v[3] = c->y;
                (void) ht_insert(nn->bad, &key, v);
                det = 0.0;
            } else {
                int ii;

                /*
                 * Looking for a matching "bad" triangle. I guess it is
                 * possible that the first circle will come out from
                 * circle_build2()as "bad", but the matching cicle will not;
                 * hence the ">" condition below.
                 */
                if (j1bad || cs[j1].r > cs[j2].r) {
                    v[6] = cs[j2].x;
                    v[7] = cs[j2].y;
                } else {
                    v[6] = cs[j1].x;
                    v[7] = cs[j1].y;
                }
                v[4] = c->x;
                v[5] = c->y;

                det = 0;
                for (ii = 0; ii < 4; ++ii) {
                    int ii1 = (ii + 1) % 4;

                    det += (v[ii * 2] + v[ii1 * 2]) * (v[ii * 2 + 1] - v[ii1 * 2 + 1]);
                }
                det = fabs(det);

                free(v);
                ht_delete(nn->bad, &key);
            }
        }

        nnpi_add_weight(nn, t->vids[j], det);
    }
}

static int compare_int(const void* p1, const void* p2)
{
    int* v1 = (int*) p1;
    int* v2 = (int*) p2;

    if (*v1 > *v2)
        return 1;
    else if (*v1 < *v2)
        return -1;
    else
        return 0;
}

typedef struct {
    point* p0;
    point* p1;
    point* p;
    int i;
} indexedpoint;

static int onleftside(point* p, point* p0, point* p1)
{
    return (p0->x - p->x) * (p1->y - p->y) > (p1->x - p->x) * (p0->y - p->y);
}

static int compare_indexedpoints(const void* pp1, const void* pp2)
{
    indexedpoint* ip1 = (indexedpoint*) pp1;
    indexedpoint* ip2 = (indexedpoint*) pp2;
    point* p0 = ip1->p0;
    point* p1 = ip1->p1;
    point* a = ip1->p;
    point* b = ip2->p;

    if (onleftside(a, p0, b)) {
        if (onleftside(a, p0, p1) && !onleftside(b, p0, p1))
            /*
             * (the reason for the second check is that while we want to sort
             * the natural neighbours in a clockwise manner, one needs to break
             * the circuit at some point)
             */
            return 1;
        else
            return -1;
    } else {
        if (onleftside(b, p0, p1) && !onleftside(a, p0, p1))
            /*
             * (see the comment above)
             */
            return -1;
        else
            return 1;
    }
}

static void nnpi_getneighbours(nnpi* nn, point* p, int nt, int* tids, int* n, int** nids)
{
    delaunay* d = nn->ds->d;
    istack* neighbours = istack_create();
    indexedpoint* v = NULL;
    int i;

    for (i = 0; i < nt; ++i) {
        triangle* t = &d->triangles[tids[i]];

        istack_push(neighbours, t->vids[0]);
        istack_push(neighbours, t->vids[1]);
        istack_push(neighbours, t->vids[2]);
    }
    qsort(neighbours->v, neighbours->n, sizeof(int), compare_int);

    v = malloc(sizeof(indexedpoint) * neighbours->n);

    v[0].p = &d->points[neighbours->v[0]];
    v[0].i = neighbours->v[0];
    *n = 1;
    for (i = 1; i < neighbours->n; ++i) {
        if (neighbours->v[i] == neighbours->v[i - 1])
            continue;
        v[*n].p = &d->points[neighbours->v[i]];
        v[*n].i = neighbours->v[i];
        (*n)++;
    }

    /*
     * I assume that if there is exactly one tricircle the point belongs to,
     * then number of natural neighbours *n = 3, and they are already sorted
     * in the right way in triangulation process.
     */
    if (*n > 3) {
        v[0].p0 = NULL;
        v[0].p1 = NULL;
        for (i = 1; i < *n; ++i) {
            v[i].p0 = p;
            v[i].p1 = v[0].p;
        }

        qsort(&v[1], *n - 1, sizeof(indexedpoint), compare_indexedpoints);
    }

    (*nids) = malloc(*n * sizeof(int));

    for (i = 0; i < *n; ++i)
        (*nids)[i] = v[i].i;

    istack_destroy(neighbours);
    free(v);
}

static int nnpi_neighbours_process(nnpi* nn, point* p, int n, int* nids)
{
    delaunay* d = nn->ds->d;
    int i;

    for (i = 0; i < n; ++i) {
        int im1 = (i + n - 1) % n;
        int ip1 = (i + 1) % n;
        point* p0 = &d->points[nids[i]];
        point* pp1 = &d->points[nids[ip1]];
        point* pm1 = &d->points[nids[im1]];
        double nom1, nom2, denom1, denom2;

        denom1 = (p0->x - p->x) * (pp1->y - p->y) - (p0->y - p->y) * (pp1->x - p->x);
        denom2 = (p0->x - p->x) * (pm1->y - p->y) - (p0->y - p->y) * (pm1->x - p->x);
        if (denom1 == 0.0) {
            if (p->x == p0->x && p->y == p0->y) {
                nnpi_add_weight(nn, nids[i], BIGNUMBER);
                return 1;
            } else if (p->x == pp1->x && p->y == pp1->y) {
                nnpi_add_weight(nn, nids[ip1], BIGNUMBER);
                return 1;
            } else {
                nn->dx = EPS_SHIFT * (pp1->y - p0->y);
                nn->dy = -EPS_SHIFT * (pp1->x - p0->x);
                return 0;
            }
        }
        if (denom2 == 0.0) {
            if (p->x == pm1->x && p->y == pm1->y) {
                nnpi_add_weight(nn, nids[im1], BIGNUMBER);
                return 1;
            } else {
                nn->dx = EPS_SHIFT * (pm1->y - p0->y);
                nn->dy = -EPS_SHIFT * (pm1->x - p0->x);
                return 0;
            }
        }

        nom1 = (p0->x - pp1->x) * (pp1->x - p->x) + (p0->y - pp1->y) * (pp1->y - p->y);
        nom2 = (p0->x - pm1->x) * (pm1->x - p->x) + (p0->y - pm1->y) * (pm1->y - p->y);
        nnpi_add_weight(nn, nids[i], nom1 / denom1 - nom2 / denom2);
    }

    return 1;
}

static int _nnpi_calculate_weights(nnpi* nn, point* p)
{
    int* tids = NULL;
    int i;

    dsearch_circles_find(nn->ds, p, &nn->ncircles, &tids);
    if (nn->ncircles == 0)
        return 1;

    /*
     * The algorithms of calculating weights for Sibson and non-Sibsonian
     * interpolations are quite different; in the first case, the weights are
     * calculated by processing Delaunay triangles whose tricircles contain
     * the interpolated point; in the second case, they are calculated by
     * processing triplets of natural neighbours by moving clockwise or
     * counterclockwise around the interpolated point.
     */
    if (nn_rule == SIBSON) {
        for (i = 0; i < nn->ncircles; ++i)
            nnpi_triangle_process(nn, p, tids[i]);
        if (nn->bad != NULL) {
            int nentries = ht_getnentries(nn->bad);

            if (nentries > 0) {
                ht_process(nn->bad, free);
                return 0;
            }
        }
        return 1;
    } else if (nn_rule == NON_SIBSONIAN) {
        int nneigh = 0;
        int* nids = NULL;
        int status;

        nnpi_getneighbours(nn, p, nn->ncircles, tids, &nneigh, &nids);
        status = nnpi_neighbours_process(nn, p, nneigh, nids);
        free(nids);

        return status;
    } else
        nn_quit("programming error");

    return 0;
}

static void nnpi_normalize_weights(nnpi* nn)
{
    int n = nn->nvertices;
    double sum = 0.0;
    int i;

    for (i = 0; i < n; ++i)
        sum += nn->weights[i];

    for (i = 0; i < n; ++i)
        nn->weights[i] /= sum;
}

#define RANDOM (double) rand() / ((double) RAND_MAX + 1.0)

void nnpi_calculate_weights(nnpi* nn, point* p)
{
    delaunay* d = nn->ds->d;
    point pp;
    int nvertices = 0;
    int* vertices = NULL;
    double* weights = NULL;
    int i;

    nnpi_reset(nn);

    if (_nnpi_calculate_weights(nn, p)) {
        nnpi_normalize_weights(nn);
        return;
    }

    nnpi_reset(nn);

    nn->dx = (d->xmax - d->xmin) * EPS_SHIFT;
    nn->dy = (d->ymax - d->ymin) * EPS_SHIFT;

    pp.x = p->x + nn->dx;
    pp.y = p->y + nn->dy;

    while (!_nnpi_calculate_weights(nn, &pp)) {
        nnpi_reset(nn);
        pp.x = p->x + nn->dx * RANDOM;
        pp.y = p->y + nn->dy * RANDOM;
    }
    nnpi_normalize_weights(nn);

    nvertices = nn->nvertices;
    if (nvertices > 0) {
        vertices = malloc(nvertices * sizeof(int));
        memcpy(vertices, nn->vertices, nvertices * sizeof(int));
        weights = malloc(nvertices * sizeof(double));
        memcpy(weights, nn->weights, nvertices * sizeof(double));
    }

    nnpi_reset(nn);

    pp.x = 2.0 * p->x - pp.x;
    pp.y = 2.0 * p->y - pp.y;

    while (!_nnpi_calculate_weights(nn, &pp) || nn->nvertices == 0) {
        nnpi_reset(nn);
        pp.x = p->x + nn->dx * RANDOM;
        pp.y = p->y + nn->dy * RANDOM;
    }
    nnpi_normalize_weights(nn);

    if (nvertices > 0)
        for (i = 0; i < nn->nvertices; ++i)
            nn->weights[i] /= 2.0;

    for (i = 0; i < nvertices; ++i)
        nnpi_add_weight(nn, vertices[i], weights[i] / 2.0);

    if (nvertices > 0) {
        free(vertices);
        free(weights);
    }
}

typedef struct {
    double* v;
    int i;
} indexedvalue;

static int cmp_iv(const void* p1, const void* p2)
{
    double v1 = *((indexedvalue *) p1)->v;
    double v2 = *((indexedvalue *) p2)->v;

    if (v1 > v2)
        return -1;
    if (v1 < v2)
        return 1;
    return 0;
}

/* Performs Natural Neighbours interpolation in a point.
 *
 * @param nn NN interpolation
 * @param p Point to be interpolated (p->x, p->y -- input; p->z -- output)
 */
void nnpi_interpolate_point(nnpi* nn, point* p)
{
    delaunay* d = nn->ds->d;
    int i;

    nnpi_calculate_weights(nn, p);

    if (nn_verbose) {
        if (nn_test_vertice == -1) {
            indexedvalue* ivs = NULL;

            if (nn->nvertices > 0) {
                ivs = malloc(nn->nvertices * sizeof(indexedvalue));

                for (i = 0; i < nn->nvertices; ++i) {
                    ivs[i].i = nn->vertices[i];
                    ivs[i].v = &nn->weights[i];
                }

                qsort(ivs, nn->nvertices, sizeof(indexedvalue), cmp_iv);
            }

            if (nn->n == 0)
                fprintf(stderr, "weights:\n");
            fprintf(stderr, "  %d: (%.10g, %10g)\n", nn->n, p->x, p->y);
            fprintf(stderr, "  %4s %15s %15s %15s %15s\n", "id", "x", "y", "z", "w");
            for (i = 0; i < nn->nvertices; ++i) {
                int ii = ivs[i].i;
                point* pp = &d->points[ii];

                fprintf(stderr, "  %5d %15.10g %15.10g %15.10g %15f\n", ii, pp->x, pp->y, pp->z, *ivs[i].v);
            }

            if (nn->nvertices > 0)
                free(ivs);
        } else {
            double w = 0.0;

            if (nn->n == 0)
                fprintf(stderr, "weight of vertex %d:\n", nn_test_vertice);
            for (i = 0; i < nn->nvertices; ++i) {
                if (nn->vertices[i] == nn_test_vertice) {
                    w = nn->weights[i];
                    break;
                }
            }
            fprintf(stderr, "  (%.10g, %.10g): %.7g\n", p->x, p->y, w);
        }
    }

    nn->n++;

    if (nn->nvertices == 0) {
        p->z = NaN;
        return;
    }

    p->z = 0.0;
    for (i = 0; i < nn->nvertices; ++i) {
        double weight = nn->weights[i];

        if (weight < nn->wmin) {
            p->z = NaN;
            return;
        }
        p->z += d->points[nn->vertices[i]].z * weight;
    }
}

/* Performs Natural Neighbours interpolation for an array of points.
 *
 * @param d Delaunay triangulation
 * @param pin Array of input points [pin]
 * @param wmin Minimal allowed weight
 * @param nout Number of output points
 * @param pout Array of output points [nout]
 */
void nnpi_interpolate_points(delaunay* d, double wmin, int nout, point pout[])
{
    nnpi* nn = nnpi_create(d);
    int seed = 0;
    int i;

    nnpi_setwmin(nn, wmin);

    if (nn_verbose) {
        fprintf(stderr, "xytoi:\n");
        for (i = 0; i < nout; ++i) {
            point* p = &pout[i];

            fprintf(stderr, "(%.7g,%.7g) -> %d\n", p->x, p->y, delaunay_xytoi(d, p, seed));
        }
    }

    for (i = 0; i < nout; ++i)
        nnpi_interpolate_point(nn, &pout[i]);

    if (nn_verbose) {
        fprintf(stderr, "output:\n");
        for (i = 0; i < nout; ++i) {
            point* p = &pout[i];

            fprintf(stderr, "  %d:%15.7g %15.7g %15.7g\n", i, p->x, p->y, p->z);
        }
    }

    nnpi_destroy(nn);
}

/* Sets minimal allowed weight for Natural Neighbours interpolation.
 *
 * For Sibson interpolation, setting wmin = 0 is equivalent to interpolating
 * inside convex hall of the data only (returning NaNs otherwise).
 *
 * @param nn Natural Neighbours point interpolator
 * @param wmin Minimal allowed weight
 */
void nnpi_setwmin(nnpi* nn, double wmin)
{
    nn->wmin = (wmin == 0) ? -EPS_WMIN : wmin;
}

/* Gets number of data points involved in current interpolation. For use by
 * `nnai'.
 *
 * @return Number of data points involved in current interpolation
 */
int nnpi_get_nvertices(nnpi* nn)
{
    return nn->nvertices;
}

/* Gets indices of data points involved in current interpolation. For use by
 * `nnai'.
 *
 * @return indices of data points involved in current interpolation
 */
int* nnpi_get_vertices(nnpi* nn)
{
    return nn->vertices;
}

/* Gets weights of data points involved in current interpolation. For use by
 * `nnai'.
 * @return weights of data points involved in current interpolation
 */
double* nnpi_get_weights(nnpi* nn)
{
    return nn->weights;
}

/*
 * nnhpi
 */

struct nnhpi {
    nnpi* nnpi;
    hashtable* ht_data;
    hashtable* ht_weights;
    int n;                      /* number of points processed */
};

typedef struct {
    int nvertices;
    int* vertices;              /* vertex indices [nvertices] */
    double* weights;            /* vertex weights [nvertices] */
} nn_weights;

/* Creates Natural Neighbours hashing point interpolator.
 *
 * @param d Delaunay triangulation
 * @param size Hash table size (should be of order of number of output points)
 * @return Natural Neighbours interpolation
 */
nnhpi* nnhpi_create(delaunay* d, int size)
{
    nnhpi* nn = malloc(sizeof(nnhpi));
    int i;

    nn->nnpi = nnpi_create(d);

    nn->ht_data = ht_create_d2(d->npoints);
    nn->ht_weights = ht_create_d2(size);
    nn->n = 0;

    for (i = 0; i < d->npoints; ++i)
        ht_insert(nn->ht_data, &d->points[i], &d->points[i]);

    return nn;
}

static void free_nn_weights(void* data)
{
    nn_weights* weights = (nn_weights*) data;

    free(weights->vertices);
    free(weights->weights);
    free(weights);
}

/* Destroys Natural Neighbours hashing point interpolation.
 *
 * @param nn Structure to be destroyed
 */
void nnhpi_destroy(nnhpi* nn)
{
    ht_destroy(nn->ht_data);
    ht_process(nn->ht_weights, free_nn_weights);
    ht_destroy(nn->ht_weights);
    nnpi_destroy(nn->nnpi);
}

/* Finds Natural Neighbours-interpolated value in a point.
 *
 * @param nnhpi NN point hashing interpolator
 * @param p Point to be interpolated (p->x, p->y -- input; p->z -- output)
 */
void nnhpi_interpolate(nnhpi* nnhpi, point* p)
{
    nnpi* nnpi = nnhpi->nnpi;
    delaunay* d = nnpi->ds->d;
    hashtable* ht_weights = nnhpi->ht_weights;
    nn_weights* weights;
    int i;

    if (ht_find(ht_weights, p) != NULL) {
        weights = ht_find(ht_weights, p);
        if (nn_verbose)
            fprintf(stderr, "  <hashtable>\n");
    } else {
        nnpi_calculate_weights(nnpi, p);

        weights = malloc(sizeof(nn_weights));
        weights->vertices = malloc(sizeof(int) * nnpi->nvertices);
        weights->weights = malloc(sizeof(double) * nnpi->nvertices);

        weights->nvertices = nnpi->nvertices;

        for (i = 0; i < nnpi->nvertices; ++i) {
            weights->vertices[i] = nnpi->vertices[i];
            weights->weights[i] = nnpi->weights[i];
        }

        ht_insert(ht_weights, p, weights);

        if (nn_verbose) {
            if (nn_test_vertice == -1) {
                if (nnpi->n == 0)
                    fprintf(stderr, "weights:\n");
                fprintf(stderr, "  %d: {", nnpi->n);

                for (i = 0; i < nnpi->nvertices; ++i) {
                    fprintf(stderr, "(%d,%.5g)", nnpi->vertices[i], nnpi->weights[i]);

                    if (i < nnpi->nvertices - 1)
                        fprintf(stderr, ", ");
                }
                fprintf(stderr, "}\n");
            } else {
                double w = 0.0;

                if (nnpi->n == 0)
                    fprintf(stderr, "weights for vertex %d:\n", nn_test_vertice);
                for (i = 0; i < nnpi->nvertices; ++i) {
                    if (nnpi->vertices[i] == nn_test_vertice) {
                        w = nnpi->weights[i];

                        break;
                    }
                }
                fprintf(stderr, "%15.7g %15.7g %15.7g\n", p->x, p->y, w);
            }
        }

        nnpi->n++;
    }

    nnhpi->n++;

    if (weights->nvertices == 0) {
        p->z = NaN;
        return;
    }

    p->z = 0.0;
    for (i = 0; i < weights->nvertices; ++i) {
        if (weights->weights[i] < nnpi->wmin) {
            p->z = NaN;
            return;
        }
        p->z += d->points[weights->vertices[i]].z * weights->weights[i];
    }
}

/* Modifies interpolated data.
 *
 * Finds point* pd in the underlying Delaunay triangulation such that
 * pd->x = p->x and pd->y = p->y, and copies p->z to pd->z. Exits with error
 * if the point is not found.
 *
 * @param nnhpi Natural Neighbours hashing point interpolator
 * @param p New data
 */
void nnhpi_modify_data(nnhpi* nnhpi, point* p)
{
    point* orig = ht_find(nnhpi->ht_data, p);

    assert(orig != NULL);
    orig->z = p->z;
}

/* Sets minimal allowed weight for Natural Neighbours interpolation.
 *
 * For Sibson interpolation, setting wmin = 0 is equivalent to interpolating
 * inside convex hall of the data only (returning NaNs otherwise).
 *
 * @param nn Natural Neighbours point hashing interpolator
 * @param wmin Minimal allowed weight
 */
void nnhpi_setwmin(nnhpi* nn, double wmin)
{
    nn->nnpi->wmin = wmin;
}

#if defined(NNPHI_TEST)

#include <sys/time.h>

#define NPOINTSIN 10000
#define NMIN 10
#define NX 101
#define NXMIN 1

#define SQ(x) ((x) * (x))

static double franke(double x, double y)
{
    x *= 9.0;
    y *= 9.0;
    return 0.75 * exp((-SQ(x - 2.0) - SQ(y - 2.0)) / 4.0)
        + 0.75 * exp(-SQ(x - 2.0) / 49.0 - (y - 2.0) / 10.0)
        + 0.5 * exp((-SQ(x - 7.0) - SQ(y - 3.0)) / 4.0)
        - 0.2 * exp(-SQ(x - 4.0) - SQ(y - 7.0));
}

static void usage()
{
    printf("Usage: nnhpi_test [-a] [-n <nin> <nxout>] [-v|-V]\n");
    printf("Options:\n");
    printf("  -a              -- use non-Sibsonian interpolation rule\n");
    printf("  -n <nin> <nout>:\n");
    printf("            <nin> -- number of input points (default = 10000)\n");
    printf("           <nout> -- number of output points per side (default = 64)\n");
    printf("  -v              -- verbose\n");
    printf("  -V              -- very verbose\n");

    exit(0);
}

int main(int argc, char* argv[])
{
    int nin = NPOINTSIN;
    int nx = NX;
    int nout = 0;
    point* pin = NULL;
    delaunay* d = NULL;
    point* pout = NULL;
    nnhpi* nn = NULL;
    int cpi = -1;               /* control point index */
    struct timeval tv0, tv1;
    struct timezone tz;
    int i;

    i = 1;
    while (i < argc) {
        switch (argv[i][1]) {
        case 'a':
            i++;
            nn_rule = NON_SIBSONIAN;
            break;
        case 'n':
            i++;
            if (i >= argc)
                nn_quit("no number of data points found after -n\n");
            nin = atoi(argv[i]);
            i++;
            if (i >= argc)
                nn_quit("no number of ouput points per side found after -i\n");
            nx = atoi(argv[i]);
            i++;
            break;
        case 'v':
            i++;
            nn_verbose = 1;
            break;
        case 'V':
            i++;
            nn_verbose = 2;
            break;
        default:
            usage();
            break;
        }
    }

    if (nin < NMIN)
        nin = NMIN;
    if (nx < NXMIN)
        nx = NXMIN;

    printf("\nTest of Natural Neighbours hashing point interpolator:\n\n");
    printf("  %d data points\n", nin);
    printf("  %d output points\n", nx * nx);

    /*
     * generate data 
     */
    printf("  generating data:\n");
    fflush(stdout);
    pin = malloc(nin * sizeof(point));
    for (i = 0; i < nin; ++i) {
        point* p = &pin[i];

        p->x = (double) random() / RAND_MAX;
        p->y = (double) random() / RAND_MAX;
        p->z = franke(p->x, p->y);
        if (nn_verbose)
            printf("    (%f, %f, %f)\n", p->x, p->y, p->z);
    }

    /*
     * triangulate
     */
    printf("  triangulating:\n");
    fflush(stdout);
    d = delaunay_build(nin, pin, 0, NULL, 0, NULL);

    /*
     * generate output points 
     */
    points_generate(-0.1, 1.1, -0.1, 1.1, nx, nx, &nout, &pout);
    cpi = (nx / 2) * (nx + 1);

    gettimeofday(&tv0, &tz);

    /*
     * create interpolator 
     */
    printf("  creating interpolator:\n");
    fflush(stdout);
    nn = nnhpi_create(d, nout);

    fflush(stdout);
    gettimeofday(&tv1, &tz);
    {
        long dt = 1000000 * (tv1.tv_sec - tv0.tv_sec) + tv1.tv_usec - tv0.tv_usec;

        printf("    interpolator creation time = %ld us (%.2f us / point)\n", dt, (double) dt / nout);
    }

    /*
     * interpolate 
     */
    printf("  interpolating:\n");
    fflush(stdout);
    gettimeofday(&tv1, &tz);
    for (i = 0; i < nout; ++i) {
        point* p = &pout[i];

        nnhpi_interpolate(nn, p);
        if (nn_verbose)
            printf("    (%f, %f, %f)\n", p->x, p->y, p->z);
    }

    fflush(stdout);
    gettimeofday(&tv0, &tz);
    {
        long dt = 1000000.0 * (tv0.tv_sec - tv1.tv_sec) + tv0.tv_usec - tv1.tv_usec;

        printf("    interpolation time = %ld us (%.2f us / point)\n", dt, (double) dt / nout);
    }

    if (!nn_verbose)
        printf("    control point: (%f, %f, %f) (expected z = %f)\n", pout[cpi].x, pout[cpi].y, pout[cpi].z, franke(pout[cpi].x, pout[cpi].y));

    printf("  interpolating one more time:\n");
    fflush(stdout);
    gettimeofday(&tv0, &tz);
    for (i = 0; i < nout; ++i) {
        point* p = &pout[i];

        nnhpi_interpolate(nn, p);
        if (nn_verbose)
            printf("    (%f, %f, %f)\n", p->x, p->y, p->z);
    }

    fflush(stdout);
    gettimeofday(&tv1, &tz);
    {
        long dt = 1000000.0 * (tv1.tv_sec - tv0.tv_sec) + tv1.tv_usec - tv0.tv_usec;

        printf("    interpolation time = %ld us (%.2f us / point)\n", dt, (double) dt / nout);
    }

    if (!nn_verbose)
        printf("    control point: (%f, %f, %f) (expected z = %f)\n", pout[cpi].x, pout[cpi].y, pout[cpi].z, franke(pout[cpi].x, pout[cpi].y));

    printf("  entering new data:\n");
    fflush(stdout);
    for (i = 0; i < nin; ++i) {
        point* p = &pin[i];

        p->z = p->x * p->x - p->y * p->y;
        nnhpi_modify_data(nn, p);
        if (nn_verbose)
            printf("    (%f, %f, %f)\n", p->x, p->y, p->z);
    }

    printf("  interpolating:\n");
    fflush(stdout);
    gettimeofday(&tv1, &tz);
    for (i = 0; i < nout; ++i) {
        point* p = &pout[i];

        nnhpi_interpolate(nn, p);
        if (nn_verbose)
            printf("    (%f, %f, %f)\n", p->x, p->y, p->z);
    }

    fflush(stdout);
    gettimeofday(&tv0, &tz);
    {
        long dt = 1000000.0 * (tv0.tv_sec - tv1.tv_sec) + tv0.tv_usec - tv1.tv_usec;

        printf("    interpolation time = %ld us (%.2f us / point)\n", dt, (double) dt / nout);
    }

    if (!nn_verbose)
        printf("    control point: (%f, %f, %f) (expected z = %f)\n", pout[cpi].x, pout[cpi].y, pout[cpi].z, pout[cpi].x * pout[cpi].x - pout[cpi].y * pout[cpi].y);

    printf("  restoring data:\n");
    fflush(stdout);
    for (i = 0; i < nin; ++i) {
        point* p = &pin[i];

        p->z = franke(p->x, p->y);
        nnhpi_modify_data(nn, p);
        if (nn_verbose)
            printf("    (%f, %f, %f)\n", p->x, p->y, p->z);
    }

    printf("  interpolating:\n");
    fflush(stdout);
    gettimeofday(&tv0, &tz);
    for (i = 0; i < nout; ++i) {
        point* p = &pout[i];

        nnhpi_interpolate(nn, p);
        if (nn_verbose)
            printf("    (%f, %f, %f)\n", p->x, p->y, p->z);
    }

    fflush(stdout);
    gettimeofday(&tv1, &tz);
    {
        long dt = 1000000.0 * (tv1.tv_sec - tv0.tv_sec) + tv1.tv_usec - tv0.tv_usec;

        printf("    interpolation time = %ld us (%.2f us / point)\n", dt, (double) dt / nout);
    }

    if (!nn_verbose)
        printf("    control point: (%f, %f, %f) (expected z = %f)\n", pout[cpi].x, pout[cpi].y, pout[cpi].z, franke(pout[cpi].x, pout[cpi].y));

    printf("  hashtable stats:\n");
    fflush(stdout);
    {
        hashtable* ht = nn->ht_data;

        printf("    input points: %d entries, %d table elements, %d filled elements\n", ht_getnentries(ht), ht_getsize(ht), ht_getnfilled(ht));
        ht = nn->ht_weights;
        printf("    weights: %d entries, %d table elements, %d filled elements\n", ht_getnentries(ht), ht_getsize(ht), ht_getnfilled(ht));
    }
    printf("\n");

    nnhpi_destroy(nn);
    free(pout);
    delaunay_destroy(d);
    free(pin);

    return 0;
}

#endif
