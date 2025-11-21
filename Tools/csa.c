/******************************************************************************
 *
 * File:           csa.c
 *
 * Created:        16/10/2002
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        2D data approximation with bivariate C1 cubic spline.
 *                 A set of library functions + standalone utility.
 *
 * Description:    See J. Haber, F. Zeilfelder, O.Davydov and H.-P. Seidel,
 *                 Smooth approximation and rendering of large scattered data
 *                 sets, in  ``Proceedings of IEEE Visualization 2001''
 *                 (Th.Ertl, K.Joy and A.Varshney, Eds.), pp.341-347, 571,
 *                 IEEE Computer Society, 2001. 
 *                 http://www.uni-giessen.de/www-Numerische-Mathematik/
 *                        davydov/VIS2001.ps.gz
 *                 http://www.math.uni-mannheim.de/~lsmath4/paper/
 *                        VIS2001.pdf.gz
 *
 * Revisions:      09/04/2003 PS: Modified points_read() to read from a
 *                   file specified by name, not by handle.
 *                 25/05/2009 PS: Added csa_approximatepoints2().
 *
 *****************************************************************************/
#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
//#include "config.h"
#include "version.h"
#include "nan.h"
#include "svd.h"
#include "csa.h"

int csa_verbose = 0;

#define NPASTART_S 10           /* Number of Points Allocated at Start for a
                                 * square */
#define NPASTART_T 100          /* Number of Points Allocated at Start for a
                                 * triangle */

/* default algorithm parameters */
#define NPMIN_DEF 3
#define NPMAX_DEF 40
#define K_DEF 140
#define NPPC_DEF 5

#define EPS 1.0e-15

struct square;
typedef struct square square;

typedef struct {
    square* parent;
    double xc, yc;
    double r;                   /* data visibility radius */

    int nallocated;
    int npoints;
    point** points;
    double** std;
} triangle;

struct square {
    csa* parent;
    int i, j;                   /* indices */

    double xmin, ymin;
    double xc, yc;

    int nallocated;
    int npoints;
    point** points;
    double** std;

    triangle* t;

    int primary;                /* flag -- whether this square contains a
                                 * primary triangle */
    int order;                  /* spline order */

    int hascoeffs[4];           /* flag -- whether there are no NaNs among
                                 * the spline coefficients */

    double coeffs[25];
};

struct csa {
    double xmin;
    double xmax;
    double ymin;
    double ymax;

    int npoints;
    point** points;
    int npointsallocated;

    int nstd;
    double** std;
    int nstdallocated;

    /*
     * squarization 
     */
    int ni;
    int nj;
    double h;
    square*** squares;          /* square* [j][i] */

    int npt;                    /* Number of Primary Triangles */
    triangle** pt;              /* Primary Triangles -- triangle* [npt] */
    int nincreased;             /* Number of sub-datasets thinned */
    int nthinned;               /* Number of sub-datasets increased */
    int norder[4];              /* Number of fittings of given interpolation
                                 * order */

    /*
     * algorithm parameters 
     */
    int npmin;                  /* minimal number of points locally involved
                                 * in spline calculation (normally = 3) */
    int npmax;                  /* maximal number of points locally involved
                                 * in spline calculation (required > 10,
                                 * recommended 20 < npmax < 60) */
    double k;                   /* relative tolerance multiple in fitting
                                 * spline coefficients: the higher this
                                 * value, the higher degree of the locally
                                 * fitted spline (recommended 80 < k < 200) */
    int nppc;                   /* average number of points per cell */
};

static void quit(char* format, ...)
{
    va_list args;

    fflush(stdout);             /* just in case -- to have the exit message
                                 * last */

    fprintf(stderr, "\n\n  error: csa: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n\n");

    exit(1);
}

/** Allocates ni x nj matrix of something and fills it with zeros. An element
 ** (i,j) will be accessed as [j][i]. For deallocation use free().
 *
 * @param nj Dimension 2
 * @param ni Dimension 1
 * @param unitsize Size of one matrix element in bytes
 * @return Matrix
 */
static void* alloc2d(size_t nj, size_t ni, size_t unitsize)
{
    size_t size;
    void* p;
    void** pp;
    int i;

    if (ni <= 0 || nj <= 0)
        quit("alloc2d(): invalid size (nj = %d, ni = %d)", nj, ni);

    size = nj * sizeof(void*) + nj * ni * unitsize;
    if ((p = malloc(size)) == NULL) {
        int errno_saved = errno;

        quit("alloc2d(): %s", strerror(errno_saved));
    }
    memset(p, 0, size);

    pp = p;
    p = &((size_t*) p)[nj];
    for (i = 0; i < nj; ++i)
        pp[i] = &((char*) p)[i * ni * unitsize];

    return pp;
}

/*
 * `index' changes from 0 to 3, indicating position of the triangle within 
 * the parent square:
 *    -----
 *   |  1  |
 *   |0   2|
 *   |  3  |
 *    -----
 */
static triangle* triangle_create(square* s)
{
    triangle* t = malloc(sizeof(triangle));
    double h = s->parent->h;

    t->parent = s;

    t->xc = s->xmin + h / 6.0;
    t->yc = s->ymin + h / 2.0;

    t->r = 0.0;
    t->points = NULL;
    t->std = NULL;
    t->nallocated = 0;
    t->npoints = 0;

    return t;
}

static void triangle_addpoint(triangle* t, point* p, double* std)
{
    if (t->nallocated == t->npoints) {
        if (t->nallocated == 0) {
            t->points = malloc(NPASTART_T * sizeof(point*));
            t->nallocated = NPASTART_T;
            if (std != NULL)
                t->std = malloc(NPASTART_T * sizeof(double*));
        } else {
            t->nallocated *= 2;
            t->points = realloc(t->points, t->nallocated * sizeof(point*));
            if (std != NULL)
                t->std = realloc(t->std, t->nallocated * sizeof(double*));
        }
    }

    t->points[t->npoints] = p;
    if (t->std != NULL)
        t->std[t->npoints] = std;
    t->npoints++;
}

static void triangle_destroy(triangle* t)
{
    if (t->points != NULL)
        free(t->points);
    if (t->std != NULL)
        free(t->std);
    free(t);
}

/* Calculates barycentric coordinates of a point.
 * Takes into account that possible triangles are rectangular, with the right 
 * angle at t->vertices[0], the vertices[1] vertex being in 
 * (-3*PI/4) + (PI/2) * t->index direction from vertices[0], and
 * vertices[2] being at (-5*PI/4) + (PI/2) * t->index.
 */
static void triangle_calculatebc(square* s, int tindex, point* p, double bc[])
{
    double dx = p->x - s->xc;
    double dy = p->y - s->yc;
    double h = s->parent->h;

    if (tindex == 0) {
        bc[1] = (dy - dx) / h;
        bc[2] = -(dx + dy) / h;
    } else if (tindex == 1) {
        bc[1] = (dx + dy) / h;
        bc[2] = (dy - dx) / h;
    } else if (tindex == 2) {
        bc[1] = (dx - dy) / h;
        bc[2] = (dx + dy) / h;
    } else {
        bc[1] = -(dx + dy) / h;
        bc[2] = (dx - dy) / h;
    }
    bc[0] = 1.0 - bc[1] - bc[2];
}

static square* square_create(csa* parent, double xmin, double ymin, int i, int j)
{
    int ii;

    square* s = malloc(sizeof(square));
    double h = parent->h;

    s->parent = parent;
    s->i = i;
    s->j = j;

    s->xmin = xmin;
    s->ymin = ymin;
    s->xc = xmin + h / 2.0;
    s->yc = ymin + h / 2.0;

    s->points = NULL;
    s->std = NULL;
    s->nallocated = 0;
    s->npoints = 0;

    s->t = triangle_create(s);

    s->primary = 0;
    s->order = -1;

    for (ii = 0; ii < 4; ++ii)
        s->hascoeffs[ii] = 0;

    for (ii = 0; ii < 25; ++ii)
        s->coeffs[ii] = NaN;

    return s;
}

static void square_destroy(square* s)
{
    if (s->t != NULL)
        triangle_destroy(s->t);
    if (s->points != NULL)
        free(s->points);
    if (s->std != NULL)
        free(s->std);
    free(s);
}

static void square_addpoint(square* s, point* p, double* std)
{
    if (s->nallocated == s->npoints) {
        if (s->nallocated == 0) {
            s->points = malloc(NPASTART_S * sizeof(point*));
            if (std != NULL)
                s->std = malloc(NPASTART_S * sizeof(double*));
            s->nallocated = NPASTART_S;
        } else {
            s->nallocated *= 2;
            s->points = realloc(s->points, s->nallocated * sizeof(point*));
            if (std != NULL)
                s->std = realloc(s->std, s->nallocated * sizeof(double*));
        }
    }

    s->points[s->npoints] = p;
    if (std != NULL)
        s->std[s->npoints] = std;
    s->npoints++;
}

csa* csa_create()
{
    csa* a = malloc(sizeof(csa));

    a->xmin = DBL_MAX;
    a->xmax = -DBL_MAX;
    a->ymin = DBL_MAX;
    a->ymax = -DBL_MAX;

    a->npoints = 0;
    a->points = malloc(NPASTART_T * sizeof(point*));
    a->npointsallocated = NPASTART_T;

    a->std = NULL;
    a->nstd = 0;
    a->nstdallocated = 0;

    a->ni = 0;
    a->nj = 0;
    a->h = NaN;
    a->squares = NULL;

    a->npt = 0;
    a->pt = NULL;
    a->nincreased = 0;
    a->nthinned = 0;
    a->norder[0] = 0;
    a->norder[1] = 0;
    a->norder[2] = 0;
    a->norder[3] = 0;

    a->npmin = NPMIN_DEF;
    a->npmax = NPMAX_DEF;
    a->k = K_DEF;
    a->nppc = NPPC_DEF;

    svd_verbose = (csa_verbose > 1) ? 1 : 0;

    return a;
}

void csa_destroy(csa* a)
{
    int i, j;

    if (a->squares != NULL) {
        for (j = 0; j < a->nj; ++j)
            for (i = 0; i < a->ni; ++i)
                square_destroy(a->squares[j][i]);
        free(a->squares);
    }
    if (a->pt != NULL)
        free(a->pt);
    if (a->points != NULL)
        free(a->points);
    if (a->std != NULL)
        free(a->std);
    free(a);
}

void csa_addpoints(csa* a, int n, point points[])
{
    int na = a->npointsallocated;
    int i;

    assert(a->squares == NULL);
    /*
     * (can be called prior to squarization only) 
     */

    while (na < a->npoints + n)
        na *= 2;
    if (na != a->npointsallocated) {
        a->points = realloc(a->points, na * sizeof(point*));
        a->npointsallocated = na;
    }

    for (i = 0; i < n; ++i) {
        point* p = &points[i];

        a->points[a->npoints] = p;
        a->npoints++;

        if (p->x < a->xmin)
            a->xmin = p->x;
        if (p->x > a->xmax)
            a->xmax = p->x;
        if (p->y < a->ymin)
            a->ymin = p->y;
        if (p->y > a->ymax)
            a->ymax = p->y;
    }
}

/* Adds std data.
 */
void csa_addstd(csa* a, int n, double std[])
{
    int na = a->nstdallocated;
    int i;

    if (std == NULL)
        return;

    if (a->std == NULL) {
        na = (n < NPASTART_S) ? NPASTART_S : n;
        a->std = malloc(na * sizeof(double*));
        a->nstdallocated = na;
    }

    while (na < a->nstd + n)
        na *= 2;
    if (na != a->nstdallocated) {
        a->std = realloc(a->std, na * sizeof(double*));
        a->nstdallocated = na;
    }

    for (i = 0; i < n; ++i) {
        assert(std[i] > 0.0);
        a->std[a->nstd] = &std[i];
        a->nstd++;
    }
}

/* Marks the squares containing "primary" triangles by setting "primary" flag 
 * to 1.
 */
static void csa_setprimaryflag(csa* a)
{
    square*** squares = a->squares;
    int nj1 = a->nj - 1;
    int ni1 = a->ni - 1;
    int i, j;

    for (j = 1; j < nj1; ++j) {
        for (i = 1; i < ni1; ++i) {
            if (squares[j][i]->npoints > 0) {
                if ((i + j) % 2 == 0) {
                    squares[j][i]->primary = 1;
                    squares[j - 1][i - 1]->primary = 1;
                    squares[j + 1][i - 1]->primary = 1;
                    squares[j - 1][i + 1]->primary = 1;
                    squares[j + 1][i + 1]->primary = 1;
                } else {
                    squares[j - 1][i]->primary = 1;
                    squares[j + 1][i]->primary = 1;
                    squares[j][i - 1]->primary = 1;
                    squares[j][i + 1]->primary = 1;
                }
            }
        }
    }
}

/* Splits the data domain in a number of squares.
 */
static void csa_squarize(csa* a)
{
    int nps[7] = { 0, 0, 0, 0, 0, 0 };  /* stats on number of points per
                                         * square */
    double dx = a->xmax - a->xmin;
    double dy = a->ymax - a->ymin;
    int npoints = a->npoints;
    double h;
    int i, j, ii, nadj;

    if (csa_verbose) {
        fprintf(stderr, "  squarizing:\n");
        fflush(stderr);
    }

    if (npoints == 0)
        return;

    assert(a->squares == NULL);
    /*
     * (can be done only once) 
     */

    h = sqrt(dx * dy * a->nppc / npoints);      /* square edge size */
    if (dx < h)
        h = dy * a->nppc / npoints;
    if (dy < h)
        h = dx * a->nppc / npoints;
    a->h = h;

    a->ni = (int) ceil(dx / h) + 2;
    a->nj = (int) ceil(dy / h) + 2;

    if (csa_verbose) {
        fprintf(stderr, "    %d x %d squares\n", a->ni, a->nj);
        fflush(stderr);
    }

    /*
     * create squares 
     */
    a->squares = alloc2d(a->nj, a->ni, sizeof(void*));
    for (j = 0; j < a->nj; ++j)
        for (i = 0; i < a->ni; ++i)
            a->squares[j][i] = square_create(a, a->xmin + h * (i - 1), a->ymin + h * (j - 1), i, j);

    /*
     * map points to squares 
     */
    for (ii = 0; ii < npoints; ++ii) {
        point* p = a->points[ii];

        i = (int) floor((p->x - a->xmin) / h) + 1;
        j = (int) floor((p->y - a->ymin) / h) + 1;
        square_addpoint(a->squares[j][i], p, (a->std == NULL) ? NULL : a->std[ii]);
    }

    /*
     * mark relevant squares with no points 
     */
    csa_setprimaryflag(a);

    /*
     * Create a list of "primary" triangles, for which spline coefficients
     * will be calculated directy (by least squares method), without using
     * C1 smoothness conditions. 
     */
    a->pt = malloc((a->ni / 2 + 1) * a->nj * sizeof(triangle*));
    for (j = 0, ii = 0, nadj = 0; j < a->nj; ++j) {
        for (i = 0; i < a->ni; ++i) {
            square* s = a->squares[j][i];

            if (s->npoints > 0) {
                int nn = s->npoints / 5;

                if (nn > 6)
                    nn = 6;
                nps[nn]++;
                ii++;
            }
            if (s->primary && s->npoints == 0)
                nadj++;
            if (s->primary) {
                a->pt[a->npt] = s->t;
                a->npt++;
            }
        }
    }
    assert(a->npt > 0);

    if (csa_verbose) {
        fprintf(stderr, "    %d non-empty squares\n", ii);
        fprintf(stderr, "    %d primary squares\n", a->npt);
        fprintf(stderr, "    %d primary squares with no data\n", nadj);
        fprintf(stderr, "    %.2f points per square \n", (double) a->npoints / ii);
    }

    if (csa_verbose == 2) {
        for (i = 0; i < 6; ++i)
            fprintf(stderr, "    %d-%d points -- %d squares\n", i * 5, i * 5 + 4, nps[i]);
        fprintf(stderr, "    %d or more points -- %d squares\n", i * 5, nps[i]);
    }

    if (csa_verbose == 2) {
        fprintf(stderr, " j\\i");
        for (i = 0; i < a->ni; ++i)
            fprintf(stderr, "%3d ", i);
        fprintf(stderr, "\n");
        for (j = a->nj - 1; j >= 0; --j) {
            fprintf(stderr, "%3d ", j);
            for (i = 0; i < a->ni; ++i) {
                square* s = a->squares[j][i];

                if (s->npoints > 0)
                    fprintf(stderr, "%3d ", s->npoints);
                else
                    fprintf(stderr, "  . ");
            }
            fprintf(stderr, "\n");
        }
    }

    /*
     * all necessary data is now copied to squares;
     * release redundant memory in the host structure
     */
    free(a->points);
    a->points = NULL;
    a->npoints = 0;
    if (a->std != NULL) {
        free(a->std);
        a->std = NULL;
    }

    if (csa_verbose)
        fflush(stderr);
}

/* Returns all squares intersecting with a square with center in t->middle
 * and edges of length 2*t->r parallel to X and Y axes.
 */
static void getsquares(csa* a, triangle* t, int* n, square*** squares)
{
    double h = t->parent->parent->h;
    int imin = (int) floor((t->xc - t->r - a->xmin) / h);
    int imax = (int) ceil((t->xc + t->r - a->xmin) / h);
    int jmin = (int) floor((t->yc - t->r - a->ymin) / h);
    int jmax = (int) ceil((t->yc + t->r - a->ymin) / h);
    int i, j;

    if (imin < 0)
        imin = 0;
    if (imax >= a->ni)
        imax = a->ni - 1;
    if (jmin < 0)
        jmin = 0;
    if (jmax >= a->nj)
        jmax = a->nj - 1;

    *n = 0;
    (*squares) = malloc((imax - imin + 1) * (jmax - jmin + 1) * sizeof(square*));

    for (j = jmin; j <= jmax; ++j) {
        for (i = imin; i <= imax; ++i) {
            square* s = a->squares[j][i];

            if (s->npoints > 0) {
                (*squares)[*n] = a->squares[j][i];
                (*n)++;
            }
        }
    }
}

static double distance(point* p1, point* p2)
{
    return hypot(p1->x - p2->x, p1->y - p2->y);
}

static double distance_xy(double x1, double y1, double x2, double y2)
{
    return hypot(x1 - x2, y1 - y2);
}

/* Thins data by creating an auxiliary regular grid and leaving only the most
 * central point within each grid cell.
 * (I follow the paper here. It is possible that taking average -- in terms of
 * both value and position -- of all points within a cell would be a bit more
 * robust. However, because of keeping only shallow copies of input points,
 * this would require quite a bit of structural changes. So, leaving it as is
 * for now.)
 */
static void thindata(triangle* t, int npmax)
{
    csa* a = t->parent->parent;
    int imax = (int) ceil(sqrt((double) (npmax * 3 / 2)));
    square*** squares = alloc2d(imax, imax, sizeof(void*));
    double h = t->r * 2.0 / imax;
    double h2 = h / 2.0;
    double xmin = t->xc - t->r;
    double ymin = t->yc - t->r;
    int i, j, ii;

    for (j = 0; j < imax; ++j)
        for (i = 0; i < imax; ++i)
            squares[j][i] = square_create(a, xmin + h * i, ymin + h * j, i, j);

    for (ii = 0; ii < t->npoints; ++ii) {
        point* p = t->points[ii];
        int i = (int) floor((p->x - xmin) / h);
        int j = (int) floor((p->y - ymin) / h);
        square* s = squares[j][i];

        if (s->npoints == 0)
            square_addpoint(s, p, (t->std == NULL) ? NULL : t->std[ii]);
        else {                  /* npoints == 1 */

            point pmiddle;

            pmiddle.x = xmin + h * i + h2;
            pmiddle.y = ymin + h * j + h2;

            if (s->std == NULL) {
                if (distance(s->points[0], &pmiddle) > distance(p, &pmiddle))
                    s->points[0] = p;
            } else {
                if ((*t->std[ii] < *s->std[0]) || (*t->std[ii] == *s->std[0] && distance(s->points[0], &pmiddle) > distance(p, &pmiddle))) {
                    s->points[0] = p;
                    s->std[0] = t->std[ii];
                }
            }
        }
    }

    t->npoints = 0;
    for (j = 0; j < imax; ++j) {
        for (i = 0; i < imax; ++i) {
            square* s = squares[j][i];

            if (squares[j][i]->npoints != 0)
                triangle_addpoint(t, s->points[0], (s->std == NULL) ? NULL : s->std[0]);
            square_destroy(s);
        }
    }

    free(squares);
    imax++;
}

/* Finds data points to be used in calculating spline coefficients for each 
 * primary triangle.
 */
static void csa_attachpointstriangle(csa* a, triangle* t)
{
    int increased = 0;

    if (csa_verbose > 1) {
        fprintf(stderr, ".");
        fflush(stderr);
    }

    t->r = a->h * 1.25;
    while (1) {
        int nsquares = 0;
        square** squares = NULL;
        int ii;

        getsquares(a, t, &nsquares, &squares);
        for (ii = 0; ii < nsquares; ++ii) {
            square* s = squares[ii];
            int iii;

            for (iii = 0; iii < s->npoints; ++iii) {
                point* p = s->points[iii];

                if (distance_xy(p->x, p->y, t->xc, t->yc) <= t->r)
                    triangle_addpoint(t, p, (s->std == NULL) ? NULL : s->std[iii]);
            }
        }

        free(squares);

        if (t->npoints < a->npmin) {
            if (!increased) {
                increased = 1;
                a->nincreased++;
            }
            t->r *= 1.25;
            t->npoints = 0;
        } else if (t->npoints > a->npmax) {
            a->nthinned++;
            thindata(t, a->npmax);
            if (t->npoints > a->npmin)
                break;
            else {
                /*
                 * Sometimes you have too much data, you thin it and -- 
                 * oops -- you have too little. This is not a frequent
                 * event, so let us not bother to put a new subdivision. 
                 */
                t->r *= 1.25;
                t->npoints = 0;
            }
        } else
            break;
    }
}

static int n2q(int n)
{
    assert(n >= 3);

    if (n >= 10)
        return 3;
    else if (n >= 6)
        return 2;
    else                        /* n == 3 */
        return 1;
}

/*
 *  square->coeffs[]:
 *
 *   ---------------------
 *  | 3    10    17    24 |
 *  |    6    13    20    |
 *  | 2     9    16    23 |
 *  |    5    12    19    |
 *  | 1     8    15    22 |
 *  |    4    11    18    |
 *  | 0     7    14    21 |
 *   ---------------------
 */

static void csa_findprimarycoeffstriangle(csa* a, triangle* t)
{
    square* s = t->parent;
    int npoints = t->npoints;
    point** points = t->points;
    double* z = malloc(npoints * sizeof(double));
    double* std = NULL;
    int q = n2q(t->npoints);
    int ok = 1;
    double b[10];
    double b1[6];
    int ii;

    if (csa_verbose > 1) {
        fprintf(stderr, ".");
        fflush(stderr);
    }

    for (ii = 0; ii < npoints; ++ii)
        z[ii] = points[ii]->z;

    if (t->std != NULL) {
        std = malloc(npoints * sizeof(double));
        for (ii = 0; ii < npoints; ++ii)
            std[ii] = *t->std[ii];
    }

    do {
        double bc[3];
        double wmin, wmax;

        if (!ok)
            q--;

        assert(q >= 0);

        if (q == 3) {
            double** A = alloc2d(npoints, 10, sizeof(double));
            double w[10];

            for (ii = 0; ii < npoints; ++ii) {
                double* aii = A[ii];
                double tmp;

                triangle_calculatebc(s, 0, points[ii], bc);

                /*
                 *  0   1   2   3   4   5   6   7   8   9 
                 * 300 210 201 120 111 102 030 021 012 003 
                 */
                tmp = bc[0] * bc[0];
                aii[0] = tmp * bc[0];
                tmp *= 3.0;
                aii[1] = tmp * bc[1];
                aii[2] = tmp * bc[2];
                tmp = bc[1] * bc[1];
                aii[6] = tmp * bc[1];
                tmp *= 3.0;
                aii[3] = tmp * bc[0];
                aii[7] = tmp * bc[2];
                tmp = bc[2] * bc[2];
                aii[9] = tmp * bc[2];
                tmp *= 3.0;
                aii[5] = tmp * bc[0];
                aii[8] = tmp * bc[1];
                aii[4] = bc[0] * bc[1] * bc[2] * 6.0;
            }

            svd_lsq(A, 10, npoints, z, std, w, b);

            wmin = w[0];
            wmax = w[0];
            for (ii = 1; ii < 10; ++ii) {
                if (w[ii] < wmin)
                    wmin = w[ii];
                else if (w[ii] > wmax)
                    wmax = w[ii];
            }
            if (wmin < wmax / a->k)
                ok = 0;

            free(A);

        } else if (q == 2) {
            double** A = alloc2d(npoints, 6, sizeof(double));
            double w[6];

            for (ii = 0; ii < npoints; ++ii) {
                double* aii = A[ii];

                triangle_calculatebc(s, 0, points[ii], bc);

                /*
                 *  0   1   2   3   4   5 
                 * 200 110 101 020 011 002 
                 */

                aii[0] = bc[0] * bc[0];
                aii[1] = bc[0] * bc[1] * 2.0;
                aii[2] = bc[0] * bc[2] * 2.0;
                aii[3] = bc[1] * bc[1];
                aii[4] = bc[1] * bc[2] * 2.0;
                aii[5] = bc[2] * bc[2];
            }

            svd_lsq(A, 6, npoints, z, std, w, b1);

            wmin = w[0];
            wmax = w[0];
            for (ii = 1; ii < 6; ++ii) {
                if (w[ii] < wmin)
                    wmin = w[ii];
                else if (w[ii] > wmax)
                    wmax = w[ii];
            }
            if (wmin < wmax / a->k)
                ok = 0;
            else {              /* degree raising */
                ok = 1;
                b[0] = b1[0];
                b[1] = (b1[0] + 2.0 * b1[1]) / 3.0;
                b[2] = (b1[0] + 2.0 * b1[2]) / 3.0;
                b[3] = (b1[3] + 2.0 * b1[1]) / 3.0;
                b[4] = (b1[1] + b1[2] + b1[4]) / 3.0;
                b[5] = (b1[5] + 2.0 * b1[2]) / 3.0;
                b[6] = b1[3];
                b[7] = (b1[3] + 2.0 * b1[4]) / 3.0;
                b[8] = (b1[5] + 2.0 * b1[4]) / 3.0;
                b[9] = b1[5];
            }

            free(A);

        } else if (q == 1) {
            double** A = alloc2d(npoints, 3, sizeof(double));
            double w[3];

            for (ii = 0; ii < npoints; ++ii) {
                double* aii = A[ii];

                triangle_calculatebc(s, 0, points[ii], bc);

                aii[0] = bc[0];
                aii[1] = bc[1];
                aii[2] = bc[2];
            }

            svd_lsq(A, 3, npoints, z, std, w, b1);

            wmin = w[0];
            wmax = w[0];
            for (ii = 1; ii < 3; ++ii) {
                if (w[ii] < wmin)
                    wmin = w[ii];
                else if (w[ii] > wmax)
                    wmax = w[ii];
            }
            if (wmin < wmax / a->k)
                ok = 0;
            else {              /* degree raising */
                ok = 1;
                b[0] = b1[0];
                b[1] = (2.0 * b1[0] + b1[1]) / 3.0;
                b[2] = (2.0 * b1[0] + b1[2]) / 3.0;
                b[3] = (2.0 * b1[1] + b1[0]) / 3.0;
                b[4] = (b1[0] + b1[1] + b1[2]) / 3.0;
                b[5] = (2.0 * b1[2] + b1[0]) / 3.0;
                b[6] = b1[1];
                b[7] = (2.0 * b1[1] + b1[2]) / 3.0;
                b[8] = (2.0 * b1[2] + b1[1]) / 3.0;
                b[9] = b1[2];
            }

            free(A);
        } else if (q == 0) {
            double** A = alloc2d(npoints, 1, sizeof(double));
            double w[1];

            for (ii = 0; ii < npoints; ++ii)
                A[ii][0] = 1.0;

            svd_lsq(A, 1, npoints, z, std, w, b1);

            ok = 1;
            b[0] = b1[0];
            b[1] = b1[0];
            b[2] = b1[0];
            b[3] = b1[0];
            b[4] = b1[0];
            b[5] = b1[0];
            b[6] = b1[0];
            b[7] = b1[0];
            b[8] = b1[0];
            b[9] = b1[0];

            free(A);
        }
    } while (!ok);

    a->norder[q]++;
    s->order = q;

    {
        square* s = t->parent;
        double* coeffs = s->coeffs;

        coeffs[12] = b[0];
        coeffs[9] = b[1];
        coeffs[6] = b[3];
        coeffs[3] = b[6];
        coeffs[2] = b[7];
        coeffs[1] = b[8];
        coeffs[0] = b[9];
        coeffs[4] = b[5];
        coeffs[8] = b[2];
        coeffs[5] = b[4];
    }

    free(z);
    if (std != NULL)
        free(std);

    if (t->points != NULL) {
        free(t->points);
        t->points = NULL;
        t->npoints = 0;
        t->nallocated = 0;
    }
    if (t->std != NULL) {
        free(t->std);
        t->std = NULL;
    }
}

/* Calculates spline coefficients in each primary triangle by least squares
 * fitting to data attached by csa_attachpointstriangle().
 */
static void csa_findprimarycoeffs(csa* a)
{
    int i;

    if (csa_verbose)
        fprintf(stderr, "  calculating spline coefficients for primary triangles:\n  ");

    for (i = 0; i < a->npt; ++i) {
        triangle* t = a->pt[i];

        csa_attachpointstriangle(a, t);
        csa_findprimarycoeffstriangle(a, t);
    }

    if (csa_verbose) {
        fprintf(stderr, "\n    3rd order -- %d sets\n", a->norder[3]);
        fprintf(stderr, "    2nd order -- %d sets\n", a->norder[2]);
        fprintf(stderr, "    1st order -- %d sets\n", a->norder[1]);
        fprintf(stderr, "    0th order -- %d sets\n", a->norder[0]);
        fflush(stderr);
    }

    if (csa_verbose == 2) {
        int j;

        fprintf(stderr, " j\\i");
        for (i = 0; i < a->ni; ++i)
            fprintf(stderr, "%2d ", i);
        fprintf(stderr, "\n");
        for (j = a->nj - 1; j >= 0; --j) {
            fprintf(stderr, "%2d  ", j);
            for (i = 0; i < a->ni; ++i) {
                square* s = a->squares[j][i];

                if (s->primary)
                    fprintf(stderr, "%2d ", s->order);
                else
                    fprintf(stderr, " . ");
            }
            fprintf(stderr, "\n");
        }
    }
}

/* Finds spline coefficients in (adjacent to primary triangles) secondary
 * triangles from C1 smoothness conditions.
 */
static void csa_findsecondarycoeffs(csa* a)
{
    square*** squares = a->squares;
    int ni = a->ni;
    int nj = a->nj;
    int ii;

    if (csa_verbose) {
        fprintf(stderr, "  propagating spline coefficients to the remaining triangles:\n");
        fflush(stderr);
    }

    /*
     * red 
     */
    for (ii = 0; ii < a->npt; ++ii) {
        triangle* t = a->pt[ii];
        square* s = t->parent;
        int i = s->i;
        int j = s->j;
        double* c = s->coeffs;
        double* cl = (i > 0) ? squares[j][i - 1]->coeffs : NULL;
        double* cb = (j > 0) ? squares[j - 1][i]->coeffs : NULL;
        double* cbl = (i > 0 && j > 0) ? squares[j - 1][i - 1]->coeffs : NULL;
        double* ca = (j < nj - 1) ? squares[j + 1][i]->coeffs : NULL;
        double* cal = (j < nj - 1 && i > 0) ? squares[j + 1][i - 1]->coeffs : NULL;

        c[7] = 2.0 * c[4] - c[1];
        c[11] = 2.0 * c[8] - c[5];
        c[15] = 2.0 * c[12] - c[9];

        c[10] = 2.0 * c[6] - c[2];
        c[13] = 2.0 * c[9] - c[5];
        c[16] = 2.0 * c[12] - c[8];

        c[19] = 2.0 * c[15] - c[11];

        if (cl != NULL) {
            cl[21] = c[0];
            cl[22] = c[1];
            cl[23] = c[2];
            cl[24] = c[3];

            cl[18] = c[0] + c[1] - c[4];
            cl[19] = c[1] + c[2] - c[5];
            cl[20] = c[2] + c[3] - c[6];

            cl[17] = 2.0 * cl[20] - cl[23];
            cl[14] = 2.0 * cl[18] - cl[22];
        }

        if (cb != NULL) {
            cb[3] = c[0];
            cb[10] = c[7];

            cb[6] = c[0] + c[7] - c[4];
            cb[2] = 2.0 * cb[6] - cb[10];
        }

        if (cbl != NULL) {
            cbl[23] = cb[2];
            cbl[24] = cb[3];

            cbl[20] = cb[2] + cb[3] - cb[6];
            cbl[17] = cl[14];
        }

        if (ca != NULL) {
            ca[0] = c[3];
            ca[7] = c[10];

            ca[4] = c[3] + c[10] - c[6];
            ca[1] = 2.0 * ca[4] - ca[7];
        }

        if (cal != NULL) {
            cal[21] = c[3];
            cal[22] = ca[1];

            cal[18] = ca[0] + ca[1] - ca[4];
            cal[14] = cl[17];
        }
    }

    /*
     * blue 
     */
    for (ii = 0; ii < a->npt; ++ii) {
        triangle* t = a->pt[ii];
        square* s = t->parent;
        int i = s->i;
        int j = s->j;
        double* c = s->coeffs;
        double* cr = (i < ni - 1) ? squares[j][i + 1]->coeffs : NULL;
        double* car = (i < ni - 1 && j < nj - 1) ? squares[j + 1][i + 1]->coeffs : NULL;
        double* cbr = (i < ni - 1 && j > 0) ? squares[j - 1][i + 1]->coeffs : NULL;

        if (car != NULL)
            cr[13] = car[7] + car[14] - car[11];

        if (cbr != NULL)
            cr[11] = cbr[10] + cbr[17] - cbr[13];

        if (cr != NULL)
            cr[5] = c[22] + c[23] - c[19];
    }

    /*
     * green & yellow 
     */
    for (ii = 0; ii < a->npt; ++ii) {
        triangle* t = a->pt[ii];
        square* s = t->parent;
        int i = s->i;
        int j = s->j;
        double* cr = (i < ni - 1) ? squares[j][i + 1]->coeffs : NULL;

        if (cr != NULL) {
            cr[9] = (cr[5] + cr[13]) / 2.0;
            cr[8] = (cr[5] + cr[11]) / 2.0;
            cr[15] = (cr[11] + cr[19]) / 2.0;
            cr[16] = (cr[13] + cr[19]) / 2.0;
            cr[12] = (cr[8] + cr[16]) / 2.0;
        }
    }

    if (csa_verbose) {
        fprintf(stderr, "  checking that all coefficients have been set:\n");
        fflush(stderr);
    }

    for (ii = 0; ii < ni * nj; ++ii) {
        square* s = squares[0][ii];
        double* c = s->coeffs;
        int i;

        if (s->npoints == 0)
            continue;
        for (i = 0; i < 25; ++i)
            if (isnan(c[i]))
                fprintf(stderr, "    squares[%d][%d]->coeffs[%d] = NaN\n", s->j, s->i, i);
    }
}

static int i300[] = { 12, 12, 12, 12 };
static int i030[] = { 3, 24, 21, 0 };
static int i003[] = { 0, 3, 24, 21 };
static int i210[] = { 9, 16, 15, 8 };
static int i021[] = { 2, 17, 22, 7 };
static int i102[] = { 4, 6, 20, 18 };
static int i120[] = { 6, 20, 18, 4 };
static int i012[] = { 1, 10, 23, 14 };
static int i201[] = { 8, 9, 16, 15 };
static int i111[] = { 5, 13, 19, 11 };

static int* iall[] = { i300, i030, i003, i210, i021, i102, i120, i012, i201, i111 };

static void csa_sethascoeffsflag(csa* a)
{
    int i, j;

    for (j = 0; j < a->nj; ++j) {
        for (i = 0; i < a->ni; ++i) {
            square* s = a->squares[j][i];
            double* coeffs = s->coeffs;
            int ii;

            for (ii = 0; ii < 4; ++ii) {
                int cc;

                for (cc = 0; cc < 10; ++cc)
                    if (isnan(coeffs[iall[cc][ii]]))
                        break;
                if (cc == 10)
                    s->hascoeffs[ii] = 1;
            }
        }
    }
}

void csa_calculatespline(csa* a)
{
    if (a->std != NULL)
        assert(a->nstd == a->npoints);
    csa_squarize(a);
    csa_findprimarycoeffs(a);
    csa_findsecondarycoeffs(a);
    csa_sethascoeffsflag(a);
}

void csa_approximatepoint(csa* a, point* p)
{
    double h = a->h;
    double ii = (p->x - a->xmin) / h + 1.0;
    double jj = (p->y - a->ymin) / h + 1.0;
    int i, j;
    square* s;
    double fi, fj;
    int ti;
    double bc[3];

    if (a->squares == NULL) {
        p->z = NaN;
        return;
    }

    if (fabs(rint(ii) - ii) / h < EPS)
        ii = rint(ii);
    if (fabs(rint(jj) - jj) / h < EPS)
        jj = rint(jj);

    if (ii < 0.0 || jj < 0.0 || ii > (double) a->ni - 1.0 || jj > (double) a->nj - 1.0) {
        p->z = NaN;
        return;
    }

    i = (int) floor(ii);
    j = (int) floor(jj);
    s = a->squares[j][i];
    fi = ii - i;
    fj = jj - j;

    if (fj < fi) {
        if (fi + fj < 1.0)
            ti = 3;
        else
            ti = 2;
    } else {
        if (fi + fj < 1.0)
            ti = 0;
        else
            ti = 1;
    }

    if (!s->hascoeffs[ti]) {
        p->z = NaN;
        return;
    }

    triangle_calculatebc(s, ti, p, bc);

    {
        double* c = s->coeffs;
        double bc1 = bc[0];
        double bc2 = bc[1];
        double bc3 = bc[2];
        double tmp1 = bc1 * bc1;
        double tmp2 = bc2 * bc2;
        double tmp3 = bc3 * bc3;

        switch (ti) {
        case 0:
            p->z = c[12] * bc1 * tmp1 + c[3] * bc2 * tmp2 + c[0] * bc3 * tmp3 + 3.0 * (c[9] * tmp1 * bc2 + c[2] * tmp2 * bc3 + c[4] * tmp3 * bc1 + c[6] * bc1 * tmp2 + c[1] * bc2 * tmp3 + c[8] * tmp1 * bc3) + 6.0 * c[5] * bc1 * bc2 * bc3;
            break;
        case 1:
            p->z = c[12] * bc1 * tmp1 + c[24] * bc2 * tmp2 + c[3] * bc3 * tmp3 + 3.0 * (c[16] * tmp1 * bc2 + c[17] * tmp2 * bc3 + c[6] * tmp3 * bc1 + c[20] * bc1 * tmp2 + c[10] * bc2 * tmp3 + c[9] * tmp1 * bc3) + 6.0 * c[13] * bc1 * bc2 * bc3;
            break;
        case 2:
            p->z = c[12] * bc1 * tmp1 + c[21] * bc2 * tmp2 + c[24] * bc3 * tmp3 + 3.0 * (c[15] * tmp1 * bc2 + c[22] * tmp2 * bc3 + c[20] * tmp3 * bc1 + c[18] * bc1 * tmp2 + c[23] * bc2 * tmp3 + c[16] * tmp1 * bc3) + 6.0 * c[19] * bc1 * bc2 * bc3;
            break;
        default:               /* 3 */
            p->z = c[12] * bc1 * tmp1 + c[0] * bc2 * tmp2 + c[21] * bc3 * tmp3 + 3.0 * (c[8] * tmp1 * bc2 + c[7] * tmp2 * bc3 + c[18] * tmp3 * bc1 + c[4] * bc1 * tmp2 + c[14] * bc2 * tmp3 + c[15] * tmp1 * bc3) + 6.0 * c[11] * bc1 * bc2 * bc3;
        }
    }
}

void csa_approximatepoints(csa* a, int n, point* points)
{
    int ii;

    for (ii = 0; ii < n; ++ii)
        csa_approximatepoint(a, &points[ii]);
}

void csa_setnpmin(csa* a, int npmin)
{
    a->npmin = npmin;
}

void csa_setnpmax(csa* a, int npmax)
{
    a->npmax = npmax;
}

void csa_setk(csa* a, int k)
{
    a->k = k;
}

void csa_setnppc(csa* a, int nppc)
{
    a->nppc = nppc;
}

/** Approximates data in given locations. Specially for Rob. Allocates the
 ** output array - needs to be cleaned up by the calling code.
 * @param nin - number of input data points
 * @param xin - X coordinates of input data points [nin]
 * @param yin - Y coordinates of input data points [nin]
 * @param zin - Z coordinates of input data points [nin]
 * @param sigma - standard deviations of input data (optional) [nin]; or NULL
 * @param nout - number of output data points
 * @param xout - X coordinates of output data points [nout]
 * @param yout - Y coordinates of output data points [nout]
 * @param npmin - algorithm parameter NPMIN; 0 for default
 * @param npmax - algorithm parameter NPMAX; 0 for default
 * @param k - algorithm parameter K; 0.0 for default
 * @param nppc - algorithm parameter NPPC; 0 for default
 * @return - Z coordinates of output data points [nout] - to be deallocated by
 *           the calling code
 */
double* csa_approximatepoints2(int nin, double xin[], double yin[], double zin[], double sigma[], int nout, double xout[], double yout[], int npmin, int npmax, int k, int nppc)
{
    point* pin = NULL;
    point* pout = NULL;
    csa* a = NULL;
    double* zout = NULL;
    int ii;

    if (nin <= 0 || nout <= 0)
        return zout;

    /*
     * create approximator
     */
    a = csa_create();
    if (npmin > 0)
        csa_setnpmin(a, npmin);
    if (npmax > 0 && npmax > npmin)
        csa_setnpmax(a, npmax);
    if (k > 0.0)
        csa_setk(a, k);
    if (nppc > 0)
        csa_setnppc(a, nppc);

    /*
     * read input data into point array
     */
    pin = malloc(sizeof(point) * nin);
    for (ii = 0; ii < nin; ++ii) {
        point* p = &pin[ii];

        p->x = xin[ii];
        p->y = yin[ii];
        p->z = zin[ii];
    }
    csa_addpoints(a, nin, pin);
    if (sigma != NULL)
        csa_addstd(a, nin, sigma);

    /*
     * calculate spline
     */
    csa_calculatespline(a);

    /*
     * read ioutput data into point array
     */
    pout = malloc(sizeof(point) * nout);
    for (ii = 0; ii < nout; ++ii) {
        point* p = &pout[ii];

        p->x = xout[ii];
        p->y = yout[ii];
        p->z = NaN;
    }

    /*
     * approximate
     */
    csa_approximatepoints(a, nout, pout);

    /*
     * write results to the output array
     */
    zout = malloc(nout * sizeof(double));
    for (ii = 0; ii < nout; ++ii)
        zout[ii] = pout[ii].z;

    /*
     * clean up
     */
    csa_destroy(a);
    free(pin);
    free(pout);

    return zout;
}

#if defined(CSA_STANDALONE)

#include "minell.h"

#define NIMAX 2048
#define BUFSIZE 10240
#define STRBUFSIZE 64

static void points_generate(double xmin, double xmax, double ymin, double ymax, int nx, int ny, int* nout, point** pout)
{
    double stepx, stepy;
    double x0, xx, yy;
    int i, j, ii;

    if (nx < 1 || ny < 1) {
        *pout = NULL;
        *nout = 0;
        return;
    }

    *nout = nx * ny;
    *pout = malloc(*nout * sizeof(point));

    stepx = (nx > 1) ? (xmax - xmin) / (nx - 1) : 0.0;
    stepy = (ny > 1) ? (ymax - ymin) / (ny - 1) : 0.0;
    x0 = (nx > 1) ? xmin : (xmin + xmax) / 2.0;
    yy = (ny > 1) ? ymin : (ymin + ymax) / 2.0;

    ii = 0;
    for (j = 0; j < ny; ++j) {
        xx = x0;
        for (i = 0; i < nx; ++i) {
            point* p = &(*pout)[ii];

            p->x = xx;
            p->y = yy;
            xx += stepx;
            ii++;
        }
        yy += stepy;
    }
}

static int str2double(char* token, double* value)
{
    char* end = NULL;

    if (token == NULL) {
        *value = NaN;
        return 0;
    }

    *value = strtod(token, &end);

    if (end == token) {
        *value = NaN;
        return 0;
    }

    return 1;
}

#define NALLOCATED_START 1024

/* Reads array of points from a columnar file.
 *
 * @param fname File name (can be "stdin" or "-" for standard input)
 * @param dim Number of dimensions (must be 2 or 3)
 * @param n Pointer to number of points (output)
 * @param points Pointer to array of points [*n] (output) (to be freed)
 * @param std Pointer to array of data std (to be freed)
 */
static void points_read(char* fname, int dim, int* n, point** points, double** std)
{
    FILE* f = NULL;
    int nallocated = NALLOCATED_START;
    char buf[BUFSIZE];
    char seps[] = " ,;\t";
    char* token;
    double stdval = NaN;

    if (dim < 2 || dim > 3) {
        *n = 0;
        *points = NULL;
        return;
    }

    if (fname == NULL)
        f = stdin;
    else {
        if (strcmp(fname, "stdin") == 0 || strcmp(fname, "-") == 0)
            f = stdin;
        else {
            f = fopen(fname, "r");
            if (f == NULL)
                quit("%s: %s", fname, strerror(errno));
        }
    }

    *points = malloc(nallocated * sizeof(point));
    *n = 0;
    while (fgets(buf, BUFSIZE, f) != NULL) {
        point* p;

        if (*n == nallocated) {
            nallocated *= 2;
            *points = realloc(*points, nallocated * sizeof(point));
            if (std != NULL && *std != NULL)
                *std = realloc(*std, nallocated * sizeof(double));
        }

        p = &(*points)[*n];

        if (buf[0] == '#')
            continue;
        if ((token = strtok(buf, seps)) == NULL)
            continue;
        if (!str2double(token, &p->x))
            continue;
        if ((token = strtok(NULL, seps)) == NULL)
            continue;
        if (!str2double(token, &p->y))
            continue;
        if (dim == 2)
            p->z = NaN;
        else {
            /*
             * z
             */
            if ((token = strtok(NULL, seps)) == NULL)
                continue;
            if (!str2double(token, &p->z))
                continue;
            /*
             * std
             */
            if (std == NULL)
                continue;

            if (*n == 0) {
                if ((token = strtok(NULL, seps)) != NULL)
                    *std = malloc(nallocated * sizeof(double));
                if (csa_verbose)
                    fprintf(stderr, "  %s stdin data\n", (token != NULL) ? "found" : "no");
            }

            if (*std != NULL) {
                if (*n != 0)
                    token = strtok(NULL, seps);
                if (token != NULL && !str2double(token, &stdval))
                    quit("  %s: could not convert \"%s\" to double", fname, token);
                (*std)[*n] = stdval;
            }
        }
        (*n)++;
    }

    if (*n == 0) {
        free(*points);
        *points = NULL;
    } else
        *points = realloc(*points, *n * sizeof(point));
    if (std != NULL && *std != NULL)
        *std = realloc(*std, *n * sizeof(point));

    if (f != stdin)
        if (fclose(f) != 0)
            quit("%s: %s", fname, strerror(errno));
}

static void points_write(int n, point* points)
{
    int i;

    for (i = 0; i < n; ++i) {
        point* p = &points[i];

        printf("%.15g %.15g %.15g\n", p->x, p->y, p->z);
    }
}

static double points_scaletosquare(int n, point* points)
{
    double xmin, ymin, xmax, ymax;
    double k;
    int i;

    if (n <= 0)
        return NaN;

    xmin = xmax = points[0].x;
    ymin = ymax = points[0].y;

    for (i = 1; i < n; ++i) {
        point* p = &points[i];

        if (p->x < xmin)
            xmin = p->x;
        else if (p->x > xmax)
            xmax = p->x;
        if (p->y < ymin)
            ymin = p->y;
        else if (p->y > ymax)
            ymax = p->y;
    }

    if (xmin == xmax || ymin == ymax)
        return NaN;
    else
        k = (ymax - ymin) / (xmax - xmin);

    for (i = 0; i < n; ++i)
        points[i].y /= k;

    return k;
}

static void points_scale(int n, point* points, double k)
{
    int i;

    for (i = 0; i < n; ++i)
        points[i].y /= k;
}

static void usage()
{
    printf("Usage: csabathy -i <XYZ file> {-o <XY file>|-n <nx>x<ny> [-c|-s] [-z <zoom>]}\n");
    printf("       [-v|-V] [-P nppc=<value>] [-P k=<value>]\n");
    printf("Options:\n");
    printf("  -c              -- scale internally so that the minimal ellipse turns into a\n");
    printf("                     circle\n");
    printf("  -i <XYZ file>   -- three-column file with points to approximate from (use\n");
    printf("                     \"-i stdin\" or \"-i -\" for standard input) (an optional\n");
    printf("                     fourth column with estimated sqrt(variance) is allowed;\n");
    printf("                     this data will be taken into account at spline fitting\n");
    printf("                     stage)\n");
    printf("  -n <nx>x<ny>    -- generate <nx>x<ny> output rectangular grid\n");
    printf("  -o <XY file>    -- two-column file with points to approximate in (use\n");
    printf("                     \"-o stdin\" or \"-o -\" for standard input)\n");
    printf("  -s              -- scale internally so that Xmax - Xmin = Ymax - Ymin\n");
    printf("  -v              -- verbose / version\n");
    printf("  -z <zoom>       -- zoom in (if <zoom> < 1) or out (<zoom> > 1) (activated\n");
    printf("                     only when used in conjunction with -n)\n");
    printf("  -P nppc=<value> -- set the average number of points per cell (default = 5,\n");
    printf("                     works best for uniform data. Decrease to get smaller\n");
    printf("                     cells or increase to get larger cells)\n");
    printf("  -P k=<value>    -- set the spline sensitivity (default = 140, reduce to get\n");
    printf("                     smoother results)\n");
    printf("  -V              -- very verbose / version\n");
    printf("Description:\n");
    printf("  `csabathy' approximates irregular scalar 2D data in specified points using\n");
    printf("  C1-continuous bivariate cubic spline. The calculated values are written to\n");
    printf("  standard output.\n");

    exit(0);
}

static void version()
{
    printf("csa version %s\n", csa_version);
    exit(0);
}

static void parse_commandline(int argc, char* argv[], char** fdata, char** fpoints, int* invariant, int* square, int* generate_points, int* nx, int* ny, int* nppc, int* k, double* zoom)
{
    int i;

    if (argc < 2)
        usage();

    i = 1;
    while (i < argc) {
        if (argv[i][0] != '-')
            usage();

        switch (argv[i][1]) {
        case 'c':
            i++;
            *invariant = 1;
            *square = 0;

            break;
        case 'i':
            i++;
            if (i >= argc)
                quit("no file name found after -i");
            *fdata = argv[i];
            i++;
            break;
        case 'n':
            i++;
            *fpoints = NULL;
            *generate_points = 1;
            if (i >= argc)
                quit("no grid dimensions found after -n");
            if (sscanf(argv[i], "%dx%d", nx, ny) != 2)
                quit("could not read grid dimensions after \"-n\"");
            if (*nx <= 0 || *nx > NIMAX || *ny <= 0 || *ny > NIMAX)
                quit("invalid size for output grid");
            i++;
            break;
        case 'o':
            i++;
            if (i >= argc)
                quit("no file name found after -o");
            *fpoints = argv[i];
            i++;
            break;
        case 's':
            i++;
            *square = 1;

            *invariant = 0;
            break;
        case 'v':
            i++;
            csa_verbose = 1;
            break;
        case 'z':
            i++;
            if (i >= argc)
                quit("no zoom value found after -z");
            *zoom = atof(argv[i]);
            i++;
            break;
        case 'P':{
                char delim[] = "=";
                char prmstr[STRBUFSIZE] = "";
                char* token;

                i++;
                if (i >= argc)
                    quit("no input found after -P");

                if (strlen(argv[i]) >= STRBUFSIZE)
                    quit("could not interpret \"%s\" after -P option", argv[i]);

                strcpy(prmstr, argv[i]);
                token = strtok(prmstr, delim);
                if (token == NULL)
                    quit("could not interpret \"%s\" after -P option", argv[i]);

                if (strcmp(token, "nppc") == 0) {
                    long int n;

                    token = strtok(NULL, delim);
                    if (token == NULL)
                        quit("could not interpret \"%s\" after -P option", argv[i]);

                    n = strtol(token, NULL, 10);
                    if (n == LONG_MIN || n == LONG_MAX)
                        quit("could not interpret \"%s\" after -P option", argv[i]);
                    else if (n <= 0)
                        quit("non-sensible value for \"nppc\" parameter");
                    *nppc = (int) n;
                } else if (strcmp(token, "k") == 0) {
                    long int n;

                    token = strtok(NULL, delim);
                    if (token == NULL)
                        quit("could not interpret \"%s\" after -P option", argv[i]);

                    n = strtol(token, NULL, 10);
                    if (n == LONG_MIN || n == LONG_MAX)
                        quit("could not interpret \"%s\" after -P option", argv[i]);
                    else if (n <= 0)
                        quit("non-sensible value for \"k\" parameter");
                    *k = (int) n;
                } else
                    usage();

                i++;
                break;
            }
        case 'V':
            i++;
            csa_verbose = 2;
            break;
        default:
            usage();
            break;
        }
    }

    if (csa_verbose && argc == 2)
        version();
}

int main(int argc, char* argv[])
{
    char* fdata = NULL;
    char* fpoints = NULL;
    int nin = 0;
    point* pin = NULL;
    double* std = NULL;
    int invariant = 0;
    minell* me = NULL;
    int square = 0;
    int nout = 0;
    int generate_points = 0;
    point* pout = NULL;
    int nx = -1;
    int ny = -1;
    csa* a = NULL;
    int nppc = -1;
    int k = -1;
    double ks = NaN;
    double zoom = NaN;

    parse_commandline(argc, argv, &fdata, &fpoints, &invariant, &square, &generate_points, &nx, &ny, &nppc, &k, &zoom);

    if (fdata == NULL)
        quit("no input data");

    if (!generate_points && fpoints == NULL)
        quit("no output grid specified");

    points_read(fdata, 3, &nin, &pin, &std);

    if (nin < 3)
        return 0;

    if (invariant) {
        me = minell_build(nin, pin);
        minell_scalepoints(me, nin, pin);
    } else if (square)
        ks = points_scaletosquare(nin, pin);

    a = csa_create();
    csa_addpoints(a, nin, pin);
    csa_addstd(a, nin, std);
    if (nppc > 0)
        csa_setnppc(a, nppc);
    if (k > 0)
        csa_setk(a, k);
    csa_calculatespline(a);

    /*
     * after calculating the spline, there is no more need in the input data
     */
    if (pin != NULL)
        free(pin);
    pin = NULL;
    if (std != NULL)
        free(std);
    std = NULL;

    if (generate_points) {
        if (isnan(zoom))
            points_generate(a->xmin - a->h, a->xmax + a->h, a->ymin - a->h, a->ymax + a->h, nx, ny, &nout, &pout);
        else {
            double xdiff2 = (a->xmax - a->xmin) / 2.0;
            double ydiff2 = (a->ymax - a->ymin) / 2.0;
            double xav = (a->xmax + a->xmin) / 2.0;
            double yav = (a->ymax + a->ymin) / 2.0;

            points_generate(xav - xdiff2 * zoom, xav + xdiff2 * zoom, yav - ydiff2 * zoom, yav + ydiff2 * zoom, nx, ny, &nout, &pout);
        }
    } else {
        points_read(fpoints, 2, &nout, &pout, NULL);
        if (invariant)
            minell_scalepoints(me, nout, pout);
        else if (square)
            points_scale(nout, pout, ks);
    }

    csa_approximatepoints(a, nout, pout);
    csa_destroy(a);
    a = NULL;

    if (invariant)
        minell_rescalepoints(me, nout, pout);
    else if (square)
        points_scale(nout, pout, 1.0 / ks);

    if (me != NULL) {
        minell_destroy(me);
        me = NULL;
    }

    points_write(nout, pout);

    free(pout);

    return 0;
}

#endif                          /* STANDALONE */
