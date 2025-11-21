/******************************************************************************
 *
 * File:        minell.c
 *
 * Created:     24/02/2003
 *
 * Author:      Pavel Sakov
 *              CSIRO Marine Research
 *              NERSC
 *
 * Description: Minimal ellipse stuff: construction of an ellipse of minimal
 *              area containing a given set of points on a plane. Affine
 *              transformation that would transform it into a unit circle with
 *              center in (0,0) and back.
 *
 *              Minimal ellipse is a handy object due to its uniqueness for a
 *              given set of points (unlike, say, minimal rectangle) and due
 *              to the speed of construction algorithms.
 *
 *              The minimal ellipse construction is based on Welzl's algorithm,
 *
 *              E. Welzl. Smallest Enclosing Disks (Balls and Ellipsoids).
 *              LNCS, 555:359--370, 1991. 32
 *              http://www.inf.ethz.ch/personal/emo/ps-files/
 *                     SmallEnclDisks-LNCS555.ps
 *
 *              The original Welzl's algorithm is purely recursive. To improve
 *              performance, we developed a modified algorithm, when Welzl's
 *              procedure is called for maximum 6 points at any one time. For
 *              big datasets, the modified algorithm is 3 to 5 times faster
 *              than the original one.
 *
 *              The numerics is described in
 *
 *              B. Gartner and S. Schonherr. Smallest enclosing ellipses - fast
 *              and exact. Serie B - Informatik B 97-03, Freie Universitat
 *              Berlin, Germany, June 1997.
 *              http://www.inf.fu-berlin.de/~sven/own_work/
 *                     smell2exact_tr-b-97-03.ps.gz
 *              ftp://ftp.inf.fu-berlin.de/pub/reports/tr-b-97-03.ps.gz
 *
 *              or
 *
 *              B. Gartner and S. Schonherr. Exact primitives for smallest
 *              enclosing ellipses. In Proc. 13th Annu. ACM Symp. on
 *              Computational Geometry, pages 430-432, 1997.  [GS97b]
 *              http://www.inf.fu-berlin.de/~sven/own_work/
 *                     smell2exact_socg97.ps.gz
 * 
 *              See also
 *              http://geometryalgorithms.com/Archive/algorithm_0107
 *
 * Revisions: PS 27/02/2003 -- improved performance by modifying the algorithm
 *
 *            PS 04/03/2003 -- fixed code in regard to the wrong assumption
 *                             that five support points may originate only
 *                             imediately after test for the fifth point being
 *                             in ellipse defined by four support points
 *
 *            PS 24/09/2008 -- fixed deficiencies in minell_calcprm() - thanks
 *                             to Glen Low for the bug report.
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <errno.h>
#include "config.h"
#include "nan.h"
#include "minell.h"

#define BIGNUMBER 1.0e+100
#define SECANT_COUNT_MAX 30
#define SECANT_EPS 5.0e-11
#define EPS 1.0e-14

static int me_seed = 1;
static int me_classic = 0;
static int me_verbose = 0;

struct minell {
    /*
     * support points
     */
    int n;
    int nprev;
    point* points[5];

    /*
     * Center form representation. A point p belongs to minimal ellipse if 
     *
     * (p - c)^T M (p - c) - 1 <= 0,
     *
     * M = || r t ||
     *     || t s || 
     */
    point c;                    /* ellipsoid center */
    double r;
    double t;
    double s;

    /*
     * Some other ellipse parameters (calculated after the ellipse is built)
     */
    double theta;               /* inclination */
    double a;                   /* semi-axis length */
    double b;                   /* semi-axis length */

    /*
     * two conics -- for n = 4 and n = 5
     */
    double dx12;
    double dy12;
    double d12;
    double dx23;
    double dy23;
    double d23;
    double dx34;
    double dy34;
    double d34;
    double dx41;
    double dy41;
    double d41;
    double r1;
    double r2;
    double s1;
    double s2;
    double t1;
    double t2;
    double u1;
    double u2;
    double v1;
    double v2;
    double w1;
    double w2;
    /*
     * the two conics are given by:
     *
     * C1(x,y) = (dy12 * x - dx12 * y + d12) * (dy34 * x - dx34 * y + d34)
     * C2(x,y) = (dy23 * x - dx23 * y + d23) * (dy41 * x - dx41 * y + d41)
     *
     * or
     *
     * C_i(x,y) = r_i x^2 + s_i y^2 + 2 t_i x y + 2 u_i x + 2 v_i y + w_i
     */

    /*
     * some coefficients used in in-ellipse tests
     */
    double alpha;
    double beta;
    double gamma;

    /*
     * n = 5: conic coefficients
     */
    double lambda0;
    double mu0;

    /*
     * stats
     */
    int minell_npoints;
    int minell_calc_count;
    int minell_calc3_count;
    int minell_calc4_count;
    int minell_calc5_count;
    int minell_eval3_count;
    int minell_eval4_count;
    int minell_eval5_count;
};

/* For a minimal ellipse with 4 support points, finds whether the conic
 * through these points and a given fifth point is an ellipse.
 */
static int isellipse(minell* me, point* p)
{
    double x = p->x;
    double y = p->y;

    me->mu0 = -(me->dy12 * x - me->dx12 * y + me->d12) * (me->dy34 * x - me->dx34 * y + me->d34);
    me->lambda0 = (me->dy23 * x - me->dx23 * y + me->d23) * (me->dy41 * x - me->dx41 * y + me->d41);

    return (me->alpha * me->lambda0 + me->beta * me->mu0) * me->lambda0 + me->gamma * me->mu0 * me->mu0 > 0.0;
}

/* For a given point, calculates a value correlated with the distance to the 
 * minimal ellipse boundary. Applicable in the case if there are 4 support
 * points AND the conic through these and the tested point is an ellipse only.
 * The fifth point is inside the minimal ellipse if the calculated value <= 0.
 */
static double minell_calcinsideeval(minell* me)
{
    double r = me->lambda0 * me->r1 + me->mu0 * me->r2;
    double s = me->lambda0 * me->s1 + me->mu0 * me->s2;
    double t = me->lambda0 * me->t1 + me->mu0 * me->t2;
    double u = me->lambda0 * me->u1 + me->mu0 * me->u2;
    double v = me->lambda0 * me->v1 + me->mu0 * me->v2;
    double w = me->lambda0 * me->w1 + me->mu0 * me->w2;
    double d = r * s - t * t;
    double Ds = me->r1 * me->s2 - me->r2 * me->s1;
    double Dt = me->r1 * me->t2 - me->r2 * me->t1;
    double Du = me->r1 * me->u2 - me->r2 * me->u1;
    double Dv = me->r1 * me->v2 - me->r2 * me->v1;
    double Dw = me->r1 * me->w2 - me->r2 * me->w1;
    double Dd = r * Ds - 2.0 * t * Dt;
    double Z = (u * s - 2.0 * v * t) * u + v * v * r;
    double DZ = 2.0 * (u * Du * s + v * Dv * r - Du * v * t - u * Dv * t - u * v * Dt) + u * u * Ds;
    double delta = 3.0 * Dd * Z + d * (2.0 * d * Dw - Dd * w - 2.0 * DZ);
    double ballpark = fabs(3.0 * Dd * Z) + fabs(d) * (fabs(2.0 * d * Dw) + fabs(Dd * w) + fabs(2.0 * DZ));

    return r * delta / ballpark;
}

static int minell_containspoint(minell* me, point* p)
{
    int i;

    for (i = 0; i < me->n; ++i)
        if (me->points[i]->x == p->x && me->points[i]->y == p->y)
            return 1;

    if (me->n < 3)
        return 0;
    else if (me->n == 3) {
        point* c = &me->c;
        double dx = p->x - c->x;
        double dy = p->y - c->y;

        me->minell_eval3_count++;

        return (me->r * dx + 2.0 * me->t * dy) * dx + me->s * dy * dy - 1.0 <= EPS;
    } else if (me->n == 4) {
        me->minell_eval4_count++;

        if (!isellipse(me, p)) {
            double lambda = 2.0 * me->gamma - me->beta;
            double mu = 2.0 * me->alpha - me->beta;
            double ballpark = fabs(mu * me->lambda0) + fabs(lambda * me->mu0);

            return mu * me->lambda0 - lambda * me->mu0 <= EPS * ballpark;
        } else
            return minell_calcinsideeval(me) <= EPS;
    } else {                    /* me->n == 5 */
        double x = p->x;
        double y = p->y;
        double C1 = (me->dy12 * x - me->dx12 * y + me->d12) * (me->dy34 * x - me->dx34 * y + me->d34) * me->lambda0;
        double C2 = (me->dy23 * x - me->dx23 * y + me->d23) * (me->dy41 * x - me->dx41 * y + me->d41) * me->mu0;
        double r = me->lambda0 * me->r1 + me->mu0 * me->r2;
        double ballpark = (fabs(me->dy12 * x) + fabs(me->dx12 * y) + fabs(me->d12)) * (fabs(me->dy34 * x) + fabs(me->dx34 * y) + fabs(me->d34)) * fabs(me->lambda0) + (fabs(me->dy23 * x) + fabs(me->dx23 * y) + fabs(me->d23)) * (fabs(me->dy41 * x) + fabs(me->dx41 * y) + fabs(me->d41)) * fabs(me->mu0);

        me->minell_eval5_count++;

        if (r > 0.0)
            return (C1 + C2) / ballpark <= EPS;
        else
            return (C1 + C2) / ballpark >= -EPS;
    }
}

static point** points_shuffle(int n, point* p)
{
    int* numbers = malloc(n * sizeof(int));
    point** pp = malloc(n * sizeof(point*));
    int i;

    for (i = 0; i < n; ++i)
        numbers[i] = i;

    srand(me_seed);

    for (i = 0; i < n; ++i) {
        int nn = (int) ((double) n * (double) rand() / ((double) RAND_MAX + 1.0));
        int tmp = numbers[i];

        numbers[i] = numbers[nn];
        numbers[nn] = tmp;
    }

    for (i = 0; i < n; ++i)
        pp[i] = &p[numbers[i]];

    free(numbers);

    return pp;
}

static minell* minell_create(void)
{
    minell* me = malloc(sizeof(minell));

    me->n = 0;
    me->nprev = -1;
    me->minell_npoints = 0;
    me->minell_calc_count = 0;
    me->minell_calc3_count = 0;
    me->minell_calc4_count = 0;
    me->minell_calc5_count = 0;
    me->minell_eval3_count = 0;
    me->minell_eval4_count = 0;
    me->minell_eval5_count = 0;

    return me;
}

void minell_destroy(minell* me)
{
    if (me != NULL)
        free(me);
}

/* Calculates minimal ellipse by 3 boundary points.
 */
static void minell_calc3(minell* me)
{
    point** points = me->points;
    point* c = &me->c;
    int i;

    me->minell_calc3_count++;

    /*
     * find center
     */
    c->x = 0.0;
    c->y = 0.0;
    c->z = NaN;
    for (i = 0; i < 3; ++i) {
        c->x += points[i]->x;
        c->y += points[i]->y;
    }
    c->x /= 3.0;
    c->y /= 3.0;

    {
        double a = 0.0, b = 0.0, d = 0.0;
        double D;

        for (i = 0; i < 3; ++i) {
            point* p = points[i];
            double dx = p->x - c->x;
            double dy = p->y - c->y;

            a += dx * dx;
            b += dx * dy;
            d += dy * dy;
        }

        D = (a * d - b * b) / 1.5;

        if (D == 0.0) {
            me->r = d * BIGNUMBER;
            me->t = -b * BIGNUMBER;
            me->s = a * BIGNUMBER;
        } else {
            me->r = d / D;
            me->t = -b / D;
            me->s = a / D;
        }
    }
}

/* Calculates whether the point p is on the left side of vector [p0, p1].
 * Returns 1 if on the left side, -1 if on the right side, 0 if on the line.
 */
static int onleftside(point* p, point* p0, point* p1)
{
    double tmp = (p0->x - p->x) * (p1->y - p->y) - (p1->x - p->x) * (p0->y - p->y);

    if (tmp > 0.0)
        return 1;
    else if (tmp < 0.0)
        return -1;
    else
        return 0;
}

/* Moves points within in an array (of up to 5 points) so that they become
 * aranged in counterclockwise direction.
 */
static void points_makeccw(int n, point* points[])
{
    point* p;
    int i;

    if (n < 3)
        return;

    if (onleftside(points[0], points[1], points[2]) < 0) {
        p = points[1];
        points[1] = points[2];
        points[2] = p;
    }

    if (n == 3)
        return;

    for (i = 0; i < 3; ++i) {
        int i1 = (i + 1) % 3;

        if (onleftside(points[3], points[i], points[i1]) < 0)
            break;
    }

    if (i == 3) {
        for (i = 0; i < 3; ++i) {
            int i1 = (i + 1) % 3;

            if (onleftside(points[3], points[i], points[i1]) <= 0)
                break;
        }
    }

    assert(i < 3);

    if (i == 1) {
        p = points[3];
        points[3] = points[2];
        points[2] = p;
    } else if (i == 0) {
        p = points[3];
        points[3] = points[2];
        points[2] = points[1];
        points[1] = p;
    }

    if (n == 4)
        return;

    for (i = 0; i < 4; ++i) {
        int i1 = (i + 1) % 4;

        if (onleftside(points[4], points[i], points[i1]) < 0)
            break;
    }

    if (i == 4) {
        for (i = 0; i < 4; ++i) {
            int i1 = (i + 1) % 4;

            if (onleftside(points[4], points[i], points[i1]) <= 0)
                break;
        }
    }

    assert(i < 4);

    if (i == 2) {
        p = points[4];
        points[4] = points[3];
        points[3] = p;
    } else if (i == 1) {
        p = points[4];
        points[4] = points[3];
        points[3] = points[2];
        points[2] = p;
    } else if (i == 0) {
        p = points[4];
        points[4] = points[3];
        points[3] = points[2];
        points[2] = points[1];
        points[1] = p;
    }
}

/* Makes some preliminary calculations for a minimal ellipse defined by 4
 * boundary points.
 */
static void minell_calc4(minell* me)
{
    point** points = me->points;

    me->minell_calc4_count++;

    /*
     * make sure the 4 points are in counterclockwise order
     */
    points_makeccw(me->n, me->points);

    me->dx12 = points[0]->x - points[1]->x;
    me->dy12 = points[0]->y - points[1]->y;
    me->d12 = points[0]->x * points[1]->y - points[1]->x * points[0]->y;

    me->dx23 = points[1]->x - points[2]->x;
    me->dy23 = points[1]->y - points[2]->y;
    me->d23 = points[1]->x * points[2]->y - points[2]->x * points[1]->y;

    me->dx34 = points[2]->x - points[3]->x;
    me->dy34 = points[2]->y - points[3]->y;
    me->d34 = points[2]->x * points[3]->y - points[3]->x * points[2]->y;

    me->dx41 = points[3]->x - points[0]->x;
    me->dy41 = points[3]->y - points[0]->y;
    me->d41 = points[3]->x * points[0]->y - points[0]->x * points[3]->y;

    me->r1 = me->dy12 * me->dy34;
    me->s1 = me->dx12 * me->dx34;
    me->t1 = -(me->dx12 * me->dy34 + me->dx34 * me->dy12) / 2.0;
    me->u1 = (me->dy12 * me->d34 + me->dy34 * me->d12) / 2.0;
    me->v1 = -(me->dx12 * me->d34 + me->dx34 * me->d12) / 2.0;
    me->w1 = me->d12 * me->d34;

    me->r2 = me->dy23 * me->dy41;
    me->s2 = me->dx23 * me->dx41;
    me->t2 = -(me->dx23 * me->dy41 + me->dx41 * me->dy23) / 2.0;
    me->u2 = (me->dy23 * me->d41 + me->dy41 * me->d23) / 2.0;
    me->v2 = -(me->dx23 * me->d41 + me->dx41 * me->d23) / 2.0;
    me->w2 = me->d23 * me->d41;

    me->alpha = me->r1 * me->s1 - me->t1 * me->t1;
    me->beta = me->r1 * me->s2 + me->r2 * me->s1 - 2.0 * me->t1 * me->t2;
    me->gamma = me->r2 * me->s2 - me->t2 * me->t2;
}

static void minell_calcb(minell* me, int nb, point* pointsb[])
{
    int i;

    me->nprev = me->n;
    me->n = nb;
    for (i = 0; i < nb; ++i)
        me->points[i] = pointsb[i];

    if (nb < 3)
        return;
    else if (nb == 3)
        minell_calc3(me);
    else if (nb == 4)
        minell_calc4(me);
    else if (me->nprev == 5) {  /* nb == 5 */
        /*
         * (should never happen according to B. Gartner  and S. Schonherr,
         * "Smallest enclosing ellipses - fast and exact", but life is not
         * always like this...)
         */
        me->minell_calc5_count++;
        minell_calcb(me, 4, pointsb);
        (void) minell_containspoint(me, pointsb[4]);
        me->n = 5;
        me->nprev = 4;
    }
}

/* Moves the last point to the front of the point pointer array.
 */
static void points_movelasttofront(point** points, int n)
{
    point* p = points[n - 1];

    memmove(&points[1], points, (n - 1) * sizeof(point*));
    points[0] = p;
}

static double distance(point* p1, point* p2)
{
    return hypot(p1->x - p2->x, p1->y - p2->y);
}

/* For a minimal ellipse defined by 4 boundary points, finds a fifth point
 * on the boundary.
 * Note: may need some tuning.
 */
static void minell_findfifthpoint(minell* me)
{
    point** ps = me->points;
    int factor = 5;
    int offset = 0;
    double x1, y1, x2, y2;
    double dx21, dy21;
    point p;
    point p1, p2;
    double f1, f2;
    double xprev = NaN;
    int count;
    int i;

    /*
     * for the quadrilateral formed by four boundary points, choose a pair of
     * opposite edges that does not include the edge of minimal length to
     * reduce loss of precision in the case if there is an edge of nearly zero
     * length
     */
    {
        double mindist = DBL_MAX;

        for (i = 0; i < 4; ++i) {
            int i1 = (i + 1) % 4;
            double dist = distance(ps[i], ps[i1]);

            if (dist < mindist) {
                mindist = dist;
                offset = i1 % 2;
            }
        }
    }

    /*
     * take middle points of the chosen edges
     */
    if (offset == 0) {
        x1 = (ps[2]->x + ps[3]->x) / 2.0;
        y1 = (ps[2]->y + ps[3]->y) / 2.0;
        x2 = (ps[0]->x + ps[1]->x) / 2.0;
        y2 = (ps[0]->y + ps[1]->y) / 2.0;
    } else {
        x1 = (ps[3]->x + ps[0]->x) / 2.0;
        y1 = (ps[3]->y + ps[0]->y) / 2.0;
        x2 = (ps[1]->x + ps[2]->x) / 2.0;
        y2 = (ps[1]->y + ps[2]->y) / 2.0;
    }
    dx21 = x2 - x1;
    dy21 = y2 - y1;

    /*
     * on the line connecting the middle points, find a point p1 such that the
     * conic through it + four boundary points is an ellipse
     */
    do {
        double dx, dy;

        p1.x = x2;
        p1.y = y2;

        factor *= 2;
        dx = dx21 / (double) factor;
        dy = dy21 / (double) factor;

        for (i = 0; i < factor; ++i) {
            p1.x += dx;
            p1.y += dy;
            if (isellipse(me, &p1))
                break;
        }
    } while (i == factor);

    f1 = minell_calcinsideeval(me);
    f2 = f1;

    /*
     * find a point p2 such that p1 and p2 are on different sides of the
     * minimal ellipse boundary
     */
    do {
        int inside = (f1 < 0.0) ? 1 : -1;
        double dx, dy;

        p2.x = p1.x;
        p2.y = p1.y;

        factor *= 2;
        dx = dx21 * (double) inside / (double) factor;
        dy = dy21 * (double) inside / (double) factor;

        for (i = 0; i < factor; ++i) {
            p2.x += dx;
            p2.y += dy;
            if (!isellipse(me, &p2))
                break;
            f2 = minell_calcinsideeval(me);
            if (f1 * f2 <= 0.0)
                break;
        }
    } while (f1 * f2 > 0.0);

    /*
     * OK, now squeeze it to death
     */
    x2 = 1.0;
    x1 = 0.0;
    xprev = NaN;
    count = 0;
    while (count < SECANT_COUNT_MAX) {
        double x = (f2 * x1 - f1 * x2) / (f2 - f1);
        double f;
        int ok;

        p.x = p2.x * x + p1.x * (1.0 - x);
        p.y = p2.y * x + p1.y * (1.0 - x);

        ok = isellipse(me, &p);
        assert(ok);

        f = minell_calcinsideeval(me);

        if (f == 0.0 || fabs(x - xprev) < SECANT_EPS)
            break;

        if (f * f1 > 0.0) {
            x1 = x2;
            f1 = f2;
            x2 = x;
            f2 = f;
        } else {
            x2 = x1;
            f2 = f1;
            x1 = x;
            f1 = f;
        }

        xprev = x;
        count++;
    }
    if (me_verbose > 1)
        fprintf(stderr, "minimal ellipse: find fifth point: count = %d\n", count);
}

static void minell_center2human(minell* me)
{
    double sum = me->r + me->s;
    double diff = me->r - me->s;
    double sqr = hypot(diff, 2.0 * me->t);
    double a = (sum - sqr) / 2.0;
    double b = (sum + sqr) / 2.0;

    me->a = sqrt(1.0 / a);
    me->b = sqrt(1.0 / b);

    if (a == b) {
        me->theta = 0.0;
        return;
    }

    me->theta = -asin(2.0 * me->t / sqr) / 2.0;
    if (me->s < me->r) {        /* cos(2 theta) < 0 */
        if (me->theta > 0)
            me->theta = M_PI / 2.0 - me->theta;
        else
            me->theta = -M_PI / 2.0 - me->theta;
    }
}

/* Calculates human-form parameters of the minimal ellipse from whatever is
 * available.
 */
static void minell_calcprm(minell* me)
{
    point** ps = me->points;

    me->c.z = NaN;
    if (me->n == 0) {
        me->c.x = NaN;
        me->c.y = NaN;
        me->a = NaN;
        me->b = NaN;
        me->theta = NaN;
    } else if (me->n == 1) {
        me->c.x = ps[0]->x;
        me->c.y = ps[0]->y;
        me->a = 0.0;
        me->b = 0.0;
        me->theta = 0.0;
    } else if (me->n == 2) {
        me->c.x = (ps[0]->x + ps[1]->x) / 2.0;
        me->c.y = (ps[0]->y + ps[1]->y) / 2.0;
        me->a = hypot(me->c.x - ps[0]->x, me->c.y - ps[0]->y);
        me->b = 0.0;
        me->theta = atan((ps[0]->y - ps[1]->y) / (ps[0]->x - ps[1]->x));
    } else if (me->n == 3) {
        minell_center2human(me);
    } else {
        if (me->n == 4)
            minell_findfifthpoint(me);
        /*
         * We have 4 explicit points on the boundary of the minimal ellipse.
         * Because the last tested point is on the boundary, lambda0 and mu0
         * give us the coefficients for the minimal ellipse conic.
         */
        {
            double r = me->lambda0 * me->r1 + me->mu0 * me->r2;
            double s = me->lambda0 * me->s1 + me->mu0 * me->s2;
            double t = me->lambda0 * me->t1 + me->mu0 * me->t2;
            double u = me->lambda0 * me->u1 + me->mu0 * me->u2;
            double v = me->lambda0 * me->v1 + me->mu0 * me->v2;
            double w = me->lambda0 * me->w1 + me->mu0 * me->w2;
            double d = r * s - t * t;
            double z;

            me->c.x = (t * v - s * u) / d;
            me->c.y = (t * u - r * v) / d;

            z = (u * (u * s - 2.0 * v * t) + v * v * r) / d - w;

            me->r = r / z;
            me->s = s / z;
            me->t = t / z;

            assert(me->r > 0.0);
            assert(me->s > 0.0);
        }
        minell_center2human(me);
    }
}

static void minell_info(minell* me, FILE* f)
{
    int i;

    fprintf(f, "minimal ellipse: info:\n");
    fprintf(f, "  defined by %d points:\n", me->n);
    for (i = 0; i < me->n; ++i) {
        point* p = me->points[i];

        fprintf(f, "    point %d: (%.15g, %.15g)\n", i, p->x, p->y);
    }
    fprintf(f, "  center = (%.15g, %.15g)\n", me->c.x, me->c.y);
    fprintf(f, "  semiaxis a = %.15g (inclination = %.15g deg)\n", me->a, me->theta * 180.0 / M_PI);
    fprintf(f, "  semiaxis b = %.15g\n", me->b);
}

/* Recursively calculates minimal ellipse by Welzl's algorithm.
 */
static void minell_calc_welzl(minell* me, int n, point* points[], int nb, point* pointsb[])
{
    point* p;
    int count;

    me->minell_calc_count++;
    count = me->minell_calc_count;

    if (me_verbose > 1) {
        int i;

        fprintf(stderr, "%d: %d | %d\n", count, n, nb);
        fprintf(stderr, "  pointsb:\n");
        fprintf(stderr, "    x = [");
        for (i = 0; i < nb; ++i)
            fprintf(stderr, "%.4f ", pointsb[i]->x);
        fprintf(stderr, "];\n");
        fprintf(stderr, "    y = [");
        for (i = 0; i < nb; ++i)
            fprintf(stderr, "%.4f ", pointsb[i]->y);
        fprintf(stderr, "];\n");

        fflush(stderr);
    }

    if (n == 0 || nb == 5) {
        /*
         * This is the only location where the ellipse (`me') gets
         * calculated. It then is passed up the stack of resursive
         * procedures to check that all data points do belong to it. 
         */
        minell_calcb(me, nb, pointsb);

        if (me_verbose > 2) {
            minell_calcprm(me);
            minell_info(me, stderr);
        }

        return;
    }

    p = points[n - 1];
    minell_calc_welzl(me, n - 1, points, nb, pointsb);

    if (me_verbose > 1) {
        int i;

        fprintf(stderr, "%d: %d | %d\n", count, n, nb);
        fprintf(stderr, "  me->points:\n");
        fprintf(stderr, "    x = [");
        for (i = 0; i < me->n; ++i)
            fprintf(stderr, "%.4f ", me->points[i]->x);
        fprintf(stderr, "];\n");
        fprintf(stderr, "    y = [");
        for (i = 0; i < me->n; ++i)
            fprintf(stderr, "%.4f ", me->points[i]->y);
        fprintf(stderr, "];\n");
    }

    if (minell_containspoint(me, p)) {
        if (me_verbose > 1) {
            fprintf(stderr, "  p = (%.4f, %.4f) : IN\n", p->x, p->y);
            fflush(stderr);
        }
        return;
    } else {
        if (me_verbose > 1) {
            fprintf(stderr, "  p = (%.4f, %.4f) : OUT\n", p->x, p->y);
            fflush(stderr);
        }

        /*
         * OK, the point does not belong to the ellipse. Add it to the list
         * of boundary points and recalculate the ellipse. 
         */

        pointsb[nb] = p;

        minell_calc_welzl(me, n - 1, points, nb + 1, pointsb);

        /*
         * This operation by Welzl greatly reduces (by 5-10 times) the number
         * of recursive calls and in-ellipse tests (although it has n^2
         * flavour).
         */
        if (me_classic)
            points_movelasttofront(points, n);
    }
}

/* This improvement to the direct call of the Welzl's recursive algorithm
 * is based on the observation that when an in-ellipse test fails, almost
 * always the new ellipse's support points will include the new point and
 * some of the former support points only. It therefore avoids making
 * in-ellipse check for all processed points each time a new point is added to
 * the boundary. Instead, it carries on until all points are verified to be
 * inside the calculated ellipse.
 */
static void minell_calc(minell* me, int n, point* points[])
{
    int nin;
    int i;

    for (i = 0, nin = 0; nin < n; i = (i + 1) % n) {
        point* pointstocheck[5];
        point* pointsonboundary[5];
        int j;

        if (minell_containspoint(me, points[i])) {
            nin++;
            continue;
        }

        if (me_verbose > 1)
            fprintf(stderr, "*** calc --> welzl ***\n");

        nin = 0;

        pointsonboundary[0] = points[i];
        for (j = 0; j < me->n; ++j)
            pointstocheck[j] = me->points[j];

        minell_calc_welzl(me, me->n, pointstocheck, 1, pointsonboundary);
    }
}

minell* minell_build(int n, point p[])
{
    point** pp = points_shuffle(n, p);
    minell* me = minell_create();

    me->minell_npoints = n;

    if (me_classic) {
        point* pointsb[5];

        minell_calc_welzl(me, n, pp, 0, pointsb);
    } else
        minell_calc(me, n, pp);

    free(pp);

    minell_calcprm(me);

    return me;
}

/* For given points, applies an affine transformation that would transfer a
 * given ellipse into a unit circle with center in (0,0).
 */
void minell_scalepoints(minell* me, int n, point points[])
{
    double _cos = cos(me->theta);
    double _sin = sin(me->theta);
    double a = me->a;
    double b = me->b;
    double x0 = me->c.x;
    double y0 = me->c.y;
    int i;

    for (i = 0; i < n; ++i) {
        point* p = &points[i];
        double x = p->x - x0;
        double y = p->y - y0;

        p->x = (x * _cos - y * _sin) / a;
        p->y = (x * _sin + y * _cos) / b;
    }
}

/* For given points, applies an affine transformation inverse to the previous
 * one.
 */
void minell_rescalepoints(minell* me, int n, point points[])
{
    double _cos = cos(me->theta);
    double _sin = sin(me->theta);
    double a = me->a;
    double b = me->b;
    double x0 = me->c.x;
    double y0 = me->c.y;
    int i;

    for (i = 0; i < n; ++i) {
        point* p = &points[i];
        double x = p->x * a;
        double y = p->y * b;

        p->x = (x * _cos + y * _sin) + x0;
        p->y = (-x * _sin + y * _cos) + y0;
    }
}

#if defined(ME_STANDALONE)

#include <sys/time.h>

int userandomseed = 0;
int test = 0;

#define NTEST 1000

static void minell_quit(char* format, ...)
{
    va_list args;

    fflush(stdout);
    fprintf(stderr, "error: minimal ellipse: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    exit(1);
}

static void minell_stats(minell* me, FILE* f)
{
    fprintf(f, "minimal ellipse: stats:\n");
    fprintf(f, "  built over %d points\n", me->minell_npoints);
    fprintf(f, "  by %sWelzl's algorithm\n", (me_classic) ? "" : "modified ");
    if (me_verbose && me_seed != 1)
        fprintf(f, "  seed = %d\n", me_seed);
    fprintf(f, "  %d calls to Welzl's procedure\n", me->minell_calc_count);
    fprintf(f, "  computed by 3 points: %d times\n", me->minell_calc3_count);
    fprintf(f, "  computed by 4 points: %d times\n", me->minell_calc4_count);
    fprintf(f, "  computed by 5 points: %d times\n", me->minell_calc5_count);
    fprintf(f, "  in-ellipse test by 3 points: %d times\n", me->minell_eval3_count);
    fprintf(f, "  in-ellipse test by 4 points: %d times\n", me->minell_eval4_count);
    fprintf(f, "  in-ellipse test by 5 points: %d times\n", me->minell_eval5_count);
}

static void usage()
{
    printf("Usage: minell <file> [-c] [-r|-s <seed>] [-t] [-v|-V]\n");
    printf("  <file> -- data file with X and Y coords in the first two columns\n");
    printf("            (use \"stdin\" or \"-\" for standard input)\n");
    printf("Options:\n");
    printf("  -c -- use classic algorithm by Welzl\n");
    printf("  -r -- use random seed at shuffling the input points\n");
    printf("  -s <seed> -- use this seed at shuffling the input points\n");
    printf("  -t -- produce stats by running 1000 times with seed changing from\n");
    printf("        1 (unless explicitely specified) to 1000\n");
    printf("  -v -- verbose\n");
    printf("  -V -- very verbose\n");
    printf("  -VV -- even more verbose\n");
    printf("Description: `minell' calculates and prints parameters of the ellipse of\n");
    printf("  minimal area containing specified points.\n");

    exit(0);
}

static void parse_commandline(int argc, char* argv[], char** fname)
{
    int i;

    *fname = NULL;

    i = 1;
    while (i < argc) {
        if (argv[i][0] != '-') {
            if (*fname == NULL) {
                *fname = argv[i];
                i++;
            } else
                usage();
        } else {
            switch (argv[i][1]) {
            case 0:
                if (*fname == NULL) {
                    *fname = argv[i];
                    i++;
                } else
                    usage();
                break;
            case 'c':
                me_classic = 1;
                i++;
                break;
            case 'r':
                userandomseed = 1;
                i++;
                break;
            case 's':
                i++;
                if (i >= argc)
                    minell_quit("could not read random seed after \"-s\"\n");
                me_seed = atoi(argv[i]);
                userandomseed = 0;
                i++;
                break;
            case 't':
                test = 1;
                i++;
                break;
            case 'v':
                me_verbose = 1;
                i++;
                break;
            case 'V':
                if (argv[i][2] == 'V')
                    me_verbose = 3;
                else
                    me_verbose = 2;
                i++;
                break;
            default:
                usage();
                break;
            }
        }
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
#define BUFSIZE 10240

/* Reads array of points from a columnar file.
 *
 * @param fname File name (can be "stdin" or "-" for standard input)
 * @param dim Number of dimensions (must be 2 or 3)
 * @param n Pointer to number of points (output)
 * @param points Pointer to array of points [*n] (output) (to be freed)
 */
static void points_read(char* fname, int dim, int* n, point** points)
{
    FILE* f = NULL;
    int nallocated = NALLOCATED_START;
    char buf[BUFSIZE];
    char seps[] = " ,;\t";
    char* token;

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
                minell_quit("%s: %s\n", fname, strerror(errno));
        }
    }

    *points = malloc(nallocated * sizeof(point));
    *n = 0;
    while (fgets(buf, BUFSIZE, f) != NULL) {
        point* p;

        if (*n == nallocated) {
            nallocated *= 2;
            *points = realloc(*points, nallocated * sizeof(point));
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
            if ((token = strtok(NULL, seps)) == NULL)
                continue;
            if (!str2double(token, &p->z))
                continue;
        }
        (*n)++;
    }

    if (*n == 0) {
        free(*points);
        *points = NULL;
    } else
        *points = realloc(*points, *n * sizeof(point));

    if (f != stdin)
        if (fclose(f) != 0)
            minell_quit("%s: %s\n", fname, strerror(errno));
}

int main(int argc, char* argv[])
{
    char* fname = NULL;
    int npoints = 0;
    point* points = NULL;
    minell* me = NULL;
    struct timeval tv0, tv1;
    struct timezone tz;

    parse_commandline(argc, argv, &fname);

    if (fname == NULL) {
        fprintf(stderr, "error: minimal ellipse: no input data\n");
        usage();
    }

    points_read(fname, 2, &npoints, &points);

    gettimeofday(&tv0, &tz);

    if (userandomseed)
        me_seed = tv0.tv_usec % INT_MAX;

    if (!test) {
        me = minell_build(npoints, points);

        gettimeofday(&tv1, &tz);

        minell_info(me, stdout);
        if (me_verbose) {
            long dt = 1000000 * (tv1.tv_sec - tv0.tv_sec) + tv1.tv_usec - tv0.tv_usec;

            minell_stats(me, stdout);
            fprintf(stdout, "minimal ellipse: elapsed time = %ld us\n", dt);
        }

        minell_destroy(me);
    } else {
        int minell_calc_count = 0;
        int minell_calc3_count = 0;
        int minell_calc4_count = 0;
        int minell_calc5_count = 0;
        int minell_eval3_count = 0;
        int minell_eval4_count = 0;
        int minell_eval5_count = 0;
        long dt;
        int i;

        for (i = 0; i < NTEST; ++i, ++me_seed) {
            me = minell_build(npoints, points);

            minell_calc_count += me->minell_calc_count;
            minell_calc3_count += me->minell_calc3_count;
            minell_calc4_count += me->minell_calc4_count;
            minell_calc5_count += me->minell_calc5_count;
            minell_eval3_count += me->minell_eval3_count;
            minell_eval4_count += me->minell_eval4_count;
            minell_eval5_count += me->minell_eval5_count;

            minell_destroy(me);
        }

        gettimeofday(&tv1, &tz);
        dt = 1000000 * (tv1.tv_sec - tv0.tv_sec) + tv1.tv_usec - tv0.tv_usec;
        fprintf(stdout, "minimal ellipse:\n");
        fprintf(stdout, "  average build time = %ld us\n", dt / NTEST);
        fprintf(stdout, "  average number of calls to Welzl's procedure = %d\n", minell_calc_count / NTEST);
        fprintf(stdout, "  average number of ellipses built by 3 support points = %d\n", minell_calc3_count / NTEST);
        fprintf(stdout, "  average number of ellipses built by 4 support points = %d\n", minell_calc4_count / NTEST);
        fprintf(stdout, "  average number of in-ellipse test by 3 support points = %d\n", minell_eval3_count / NTEST);
        fprintf(stdout, "  average number of in-ellipse test by 4 support points = %d\n", minell_eval4_count / NTEST);
        fprintf(stdout, "  average number of in-ellipse test by 5 support points = %d\n", minell_eval5_count / NTEST);
    }

    if (points != NULL)
        free(points);

    return 0;
}

#endif                          /* ME_UTIL */
