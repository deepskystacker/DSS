/******************************************************************************
 *
 * File:           delaunay.c
 *
 * Created:        04/08/2000
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Delaunay triangulation - a wrapper to triangulate()
 *
 * Description:    None
 *
 * Revisions:      10/06/2003 PS: delaunay_build(); delaunay_destroy();
 *                   struct delaunay: from now on, only shallow copy of the
 *                   input data is contained in struct delaunay. This saves
 *                   memory and is consistent with libcsa.
 *                 30/10/2007 PS: added delaunay_addflag() and
 *                   delaunay_resetflags(); modified delaunay_circles_find()
 *                   to reset the flags to 0 on return. This is very important
 *                   for large datasets, many thanks to John Gerschwitz,
 *                   Petroleum Geo-Services, for identifying the problem.
 *                 05/05/2021 PS: added MPI code.
 *
 *****************************************************************************/

#define ANSI_DECLARATORS        /* for triangle.h */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include "triangle.h"
#include "istack_internal.h"
#include "nn.h"
#include "nncommon.h"
#include "delaunay_internal.h"

#if defined(MPI)
#include <mpi.h>

int nprocesses = 1;
int rank = 0;

#if defined(USE_SHMEM)
/*
 * "sm" stands for "shared memory"
 */
MPI_Comm sm_comm = MPI_COMM_NULL;
int sm_comm_rank = -1;
int sm_comm_size = 0;
#endif
extern int make_iso_compilers_happy;
#endif                          /* MPI */

/*
 * This parameter is used in search of tricircles containing a given point:
 *   if there are no more triangles than N_SEARCH_TURNON
 *     do linear search
 *   else
 *     do more complicated stuff
 */
#define N_SEARCH_TURNON 20
#define N_FLAGS_TURNON 1000
#define N_FLAGS_INC 100

static void tio_destroy(struct triangulateio* tio)
{
    if (tio->pointlist != NULL)
        free(tio->pointlist);
    if (tio->pointattributelist != NULL)
        free(tio->pointattributelist);
    if (tio->pointmarkerlist != NULL)
        free(tio->pointmarkerlist);
    if (tio->trianglelist != NULL)
        free(tio->trianglelist);
    if (tio->triangleattributelist != NULL)
        free(tio->triangleattributelist);
    if (tio->trianglearealist != NULL)
        free(tio->trianglearealist);
    if (tio->neighborlist != NULL)
        free(tio->neighborlist);
    if (tio->segmentlist != NULL)
        free(tio->segmentlist);
    if (tio->segmentmarkerlist != NULL)
        free(tio->segmentmarkerlist);
    if (tio->holelist != NULL)
        free(tio->holelist);
    if (tio->regionlist != NULL)
        free(tio->regionlist);
    if (tio->edgelist != NULL)
        free(tio->edgelist);
    if (tio->edgemarkerlist != NULL)
        free(tio->edgemarkerlist);
    if (tio->normlist != NULL)
        free(tio->normlist);
}

static delaunay* delaunay_create()
{
    delaunay* d = calloc(1, sizeof(delaunay));

    d->xmin = DBL_MAX;
    d->xmax = -DBL_MAX;
    d->ymin = DBL_MAX;
    d->ymax = -DBL_MAX;

    return d;
}

static void tio2delaunay(struct triangulateio* tio, delaunay* d, void* data)
{
    int i, j;

    if (tio != NULL) {

        /*
         * I assume that all input points appear in tio in the same order
         * as they were written to tio_in. I have seen no exceptions so far,
         * even if duplicate points were presented. Just in case, let us make
         * a couple of checks. 
         */
        assert(tio->numberofpoints == d->npoints);
        assert(tio->pointlist[2 * d->npoints - 2] == d->points[d->npoints - 1].x && tio->pointlist[2 * d->npoints - 1] == d->points[d->npoints - 1].y);

        d->ntriangles = tio->numberoftriangles;
    }
#if defined(USE_SHMEM)
    MPI_Bcast(&d->ntriangles, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif

    d->triangles = data;
    d->neighbours = (triangle_neighbours*) &d->triangles[d->ntriangles];
    d->circles = (circle*) &d->neighbours[d->ntriangles];
    d->n_point_triangles = (int*) &d->circles[d->ntriangles];
    d->point_triangles_offset = (int*) &d->n_point_triangles[d->npoints];
    d->point_triangles = (int*) &d->point_triangles_offset[d->npoints];

    if (tio != NULL) {
        if (nn_verbose)
            fprintf(stderr, "triangles:\n");
        for (i = 0; i < d->ntriangles; ++i) {
            int offset = i * 3;
            triangle* t = &d->triangles[i];
            triangle_neighbours* n = &d->neighbours[i];
            circle* c = &d->circles[i];
            int status;

            t->vids[0] = tio->trianglelist[offset];
            t->vids[1] = tio->trianglelist[offset + 1];
            t->vids[2] = tio->trianglelist[offset + 2];

            n->tids[0] = tio->neighborlist[offset];
            n->tids[1] = tio->neighborlist[offset + 1];
            n->tids[2] = tio->neighborlist[offset + 2];

            status = circle_build1(c, &d->points[t->vids[0]], &d->points[t->vids[1]], &d->points[t->vids[2]]);
            assert(status);

            if (nn_verbose)
                fprintf(stderr, "  %d: (%d,%d,%d)\n", i, t->vids[0], t->vids[1], t->vids[2]);
        }

        for (i = 0; i < d->ntriangles; ++i) {
            triangle* t = &d->triangles[i];

            for (j = 0; j < 3; ++j)
                d->n_point_triangles[t->vids[j]]++;
        }
        for (i = 1; i < d->npoints; ++i)
            d->point_triangles_offset[i] = d->point_triangles_offset[i - 1] + d->n_point_triangles[i - 1];
        memset(d->n_point_triangles, 0, d->npoints * sizeof(int));

        for (i = 0; i < d->ntriangles; ++i) {
            triangle* t = &d->triangles[i];

            for (j = 0; j < 3; ++j) {
                int vid = t->vids[j];

                d->point_triangles[d->point_triangles_offset[vid] + d->n_point_triangles[vid]] = i;
                d->n_point_triangles[vid]++;
            }
        }
    }
#if defined(USE_SHMEM)
    MPI_Win_fence(0, d->sm_win_delaunaydata);
    MPI_Barrier(sm_comm);
#endif
}

static size_t delaunay_getdatasize(struct triangulateio* tio)
{
    return tio->numberoftriangles * (sizeof(triangle) + sizeof(triangle_neighbours) + sizeof(circle) + sizeof(int) * 3) + tio->numberofpoints * sizeof(int) * 2;
}

/* Builds Delaunay triangulation of the given array of points.
 *
 * @param np Number of points
 * @param points Array of points [np] (input)
 * @param ns Number of forced segments
 * @param segments Array of (forced) segment endpoint indices [2*ns]
 * @param nh Number of holes
 * @param holes Array of hole (x,y) coordinates [2*nh]
 * @return Delaunay triangulation structure with triangulation results
 */
delaunay* delaunay_build(int np, point points[], int ns, int segments[], int nh, double holes[])
{
    delaunay* d = NULL;
    struct triangulateio tio_in;
    struct triangulateio tio_out;
    int i, j;

#if defined(USE_SHMEM)
    int ntriangles;

    if (sm_comm_rank == 0) {
#endif
        char cmd[64] = "eznC";

        if (np == 0)
            return NULL;

        assert(sizeof(REAL) == sizeof(double));

        memset(&tio_in, 0, sizeof(struct triangulateio));
        memset(&tio_out, 0, sizeof(struct triangulateio));

        tio_in.pointlist = malloc(np * 2 * sizeof(double));
        tio_in.numberofpoints = np;
        for (i = 0, j = 0; i < np; ++i) {
            tio_in.pointlist[j++] = points[i].x;
            tio_in.pointlist[j++] = points[i].y;
        }

        if (ns > 0) {
            tio_in.segmentlist = malloc(ns * 2 * sizeof(int));
            tio_in.numberofsegments = ns;
            memcpy(tio_in.segmentlist, segments, ns * 2 * sizeof(int));
        }

        if (nh > 0) {
            tio_in.holelist = malloc(nh * 2 * sizeof(double));
            tio_in.numberofholes = nh;
            memcpy(tio_in.holelist, holes, nh * 2 * sizeof(double));
        }

        if (!nn_verbose)
            strcat(cmd, "Q");
        else if (nn_verbose > 1)
            strcat(cmd, "VV");
        if (ns != 0)
            strcat(cmd, "p");

        if (nn_verbose)
            fflush(stderr);

        /*
         * climax 
         */
        triangulate(cmd, &tio_in, &tio_out, NULL);

        if (nn_verbose)
            fflush(stderr);

#if defined(USE_SHMEM)
        ntriangles = tio_out.numberoftriangles;
    }
    (void) MPI_Bcast(&ntriangles, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (ntriangles == 0)
        goto finish;
#endif

#if !defined(USE_SHMEM)
    if (tio_out.numberoftriangles == 0)
        goto finish;
#endif

    d = delaunay_create();
    d->npoints = np;
    d->points = points;         /* (shallow copy) */
    for (i = 0, j = 0; i < np; ++i) {
        point* p = &points[i];

        if (p->x < d->xmin)
            d->xmin = p->x;
        if (p->x > d->xmax)
            d->xmax = p->x;
        if (p->y < d->ymin)
            d->ymin = p->y;
        if (p->y > d->ymax)
            d->ymax = p->y;
    }

#if defined(USE_SHMEM)
    if (sm_comm_rank == 0) {
#endif
        if (nn_verbose) {
            fprintf(stderr, "input:\n");
            for (i = 0, j = 0; i < d->npoints; ++i) {
                point* p = &d->points[i];

                fprintf(stderr, "  %d: %15.7g %15.7g %15.7g\n", i, p->x, p->y, p->z);
            }
        }
#if defined(USE_SHMEM)
    }
#endif

    {
        size_t size = 0;
        void* data = NULL;

#if !defined(USE_SHMEM)
        size = delaunay_getdatasize(&tio_out);
        data = calloc(1, size);
#else
        if (rank == 0) {
            size = delaunay_getdatasize(&tio_out);
            assert(sizeof(size_t) == sizeof(MPI_UNSIGNED_LONG));
        }
        (void) MPI_Bcast(&size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

        (void) MPI_Win_allocate_shared((sm_comm_rank == 0) ? size : 0, sizeof(char), MPI_INFO_NULL, sm_comm, &data, &d->sm_win_delaunaydata);
        if (sm_comm_rank == 0) {
            memset(data, 0, size);
            if (nn_verbose)
                fprintf(stderr, "  MPI: put %u bytes of triangulation data into shared memory\n", (unsigned int) size);
        } else {
            int disp_unit;
            MPI_Aint my_size;

            MPI_Win_shared_query(d->sm_win_delaunaydata, 0, &my_size, &disp_unit, &data);
            assert(my_size == size);
            assert(disp_unit == sizeof(char));
            assert(data != NULL);
        }
        MPI_Win_fence(0, d->sm_win_delaunaydata);
        MPI_Barrier(sm_comm);
#endif

#if defined(USE_SHMEM)
        if (sm_comm_rank == 0)
#endif
            tio2delaunay(&tio_out, d, data);
#if defined(USE_SHMEM)
        else
            tio2delaunay(NULL, d, data);
#endif
    }

  finish:
#if defined(USE_SHMEM)
    if (sm_comm_rank == 0) {
#endif
        tio_destroy(&tio_in);
        tio_destroy(&tio_out);
#if defined(USE_SHMEM)
    }
#endif

    return d;
}

/** Destroys Delaunay triangulation.
 *
 * @param d Structure to be destroyed
 */
void delaunay_destroy(delaunay* d)
{
    if (d == NULL)
        return;

#if !defined(USE_SHMEM)
    if (d->triangles != NULL)
        free(d->triangles);
#else
    MPI_Win_free(&d->sm_win_delaunaydata);
#endif
    free(d);
}

/**
 */
dsearch* dsearch_build(delaunay* d)
{
    dsearch* ds = calloc(1, sizeof(dsearch));

    ds->d = d;
    ds->first_id = -1;
    if (d->ntriangles > 0)
        ds->flags = calloc(d->ntriangles, sizeof(int));

    return ds;
}

/**
 */
void dsearch_destroy(dsearch* ds)
{
    if (ds->flags != NULL)
        free(ds->flags);
    if (ds->t_in != NULL)
        istack_destroy(ds->t_in);
    if (ds->t_out != NULL)
        istack_destroy(ds->t_out);
    if (ds->flagids != NULL)
        free(ds->flagids);
    free(ds);
}

/* Returns whether the point p is on the right side of the vector (p0, p1).
 */
static int onrightside(point* p, point* p0, point* p1)
{
    return (p1->x - p->x) * (p0->y - p->y) > (p0->x - p->x) * (p1->y - p->y);
}

/* Finds triangle specified point belongs to (if any).
 *
 * @param d Delaunay triangulation
 * @param p Point to be mapped
 * @param seed Triangle index to start with
 * @return Triangle id if successful, -1 otherwhile
 */
int delaunay_xytoi(delaunay* d, point* p, int id)
{
    triangle* t;
    int i;

    if (p->x < d->xmin || p->x > d->xmax || p->y < d->ymin || p->y > d->ymax)
        return -1;

    if (id < 0 || id > d->ntriangles)
        id = 0;
    t = &d->triangles[id];
    do {
        for (i = 0; i < 3; ++i) {
            int i1 = (i + 1) % 3;

            if (onrightside(p, &d->points[t->vids[i]], &d->points[t->vids[i1]])) {
                id = d->neighbours[id].tids[(i + 2) % 3];
                if (id < 0)
                    return id;
                t = &d->triangles[id];
                break;
            }
        }
    } while (i < 3);

    return id;
}

static void dsearch_addflag(dsearch* ds, int i)
{
    if (ds->nflags == ds->nflagsallocated) {
        ds->nflagsallocated += N_FLAGS_INC;
        ds->flagids = realloc(ds->flagids, ds->nflagsallocated * sizeof(int));
    }
    ds->flagids[ds->nflags] = i;
    ds->nflags++;
}

static void dsearch_resetflags(dsearch* ds)
{
    int i;

    for (i = 0; i < ds->nflags; ++i)
        ds->flags[ds->flagids[i]] = 0;
    ds->nflags = 0;
}

/** Find all tricircles specified point belongs to.
 *
 * @param ds `dsearch' structure
 * @param p Point to be mapped
 * @param n Pointer to the number of tricircles within `d' containing `p'
 *          (output)
 * @param out Pointer to an array of indices of the corresponding triangles 
 *            [n] (output)
 *
 * There is a standard search procedure involving search through triangle
 * neighbours (not through vertex neighbours). It must be a bit faster due to
 * the smaller number of triangle neighbours (3 per triangle) but may fail
 * for a point outside convex hall.
 *
 * We may wish to modify this procedure in future: first check if the point
 * is inside the convex hall, and depending on that use one of the two
 * search algorithms. It not 100% clear though whether this will lead to a
 * substantial speed gains because of the check on convex hall involved.
 */
void dsearch_circles_find(dsearch* ds, point* p, int* n, int** out)
{
    delaunay* d = ds->d;

    /*
     * This flag was introduced as a hack to handle some degenerate cases. It 
     * is set to 1 only if the triangle associated with the first circle is
     * already known to contain the point. In this case the circle is assumed 
     * to contain the point without a check. In my practice this turned
     * useful in some cases when point p coincided with one of the vertices
     * of a thin triangle. 
     */
    int contains = 0;
    int i;

    if (ds->t_in == NULL) {
        ds->t_in = istack_create();
        ds->t_out = istack_create();
    }

    /*
     * if there are only a few data points, do linear search
     */
    if (d->ntriangles <= N_SEARCH_TURNON) {
        istack_reset(ds->t_out);

        for (i = 0; i < d->ntriangles; ++i) {
            if (circle_contains(&d->circles[i], p)) {
                istack_push(ds->t_out, i);
            }
        }

        *n = ds->t_out->n;
        *out = ds->t_out->v;

        return;
    }
    /*
     * otherwise, do a more complicated stuff
     */

    /*
     * It is important to have a reasonable seed here. If the last search
     * was successful -- start with the last found tricircle, otherwhile (i) 
     * try to find a triangle containing p; if fails then (ii) check
     * tricircles from the last search; if fails then (iii) make linear
     * search through all tricircles 
     */
    if (ds->first_id < 0 || !circle_contains(&d->circles[ds->first_id], p)) {
        /*
         * if any triangle contains p -- start with this triangle 
         */
        ds->first_id = delaunay_xytoi(d, p, ds->first_id);
        contains = (ds->first_id >= 0);

        /*
         * if no triangle contains p, there still is a chance that it is
         * inside some of circumcircles 
         */
        if (ds->first_id < 0) {
            int nn = ds->t_out->n;
            int tid = -1;

            /*
             * first check results of the last search 
             */
            for (i = 0; i < nn; ++i) {
                tid = ds->t_out->v[i];
                if (circle_contains(&d->circles[tid], p))
                    break;
            }
            /*
             * if unsuccessful, search through all circles 
             */
            if (tid < 0 || i == nn) {
                double nt = d->ntriangles;

                for (tid = 0; tid < nt; ++tid) {
                    if (circle_contains(&d->circles[tid], p))
                        break;
                }
                if (tid == nt) {
                    istack_reset(ds->t_out);
                    *n = 0;
                    *out = NULL;
                    return;     /* failed */
                }
            }
            ds->first_id = tid;
        }
    }

    istack_reset(ds->t_in);
    istack_reset(ds->t_out);

    istack_push(ds->t_in, ds->first_id);
    ds->flags[ds->first_id] = 1;
    dsearch_addflag(ds, ds->first_id);

    /*
     * main cycle 
     */
    while (ds->t_in->n > 0) {
        int tid = istack_pop(ds->t_in);
        triangle* t = &d->triangles[tid];

        if (contains || circle_contains(&d->circles[tid], p)) {
            istack_push(ds->t_out, tid);
            for (i = 0; i < 3; ++i) {
                int vid = t->vids[i];
                int nt = d->n_point_triangles[vid];
                int j;

                for (j = 0; j < nt; ++j) {
                    int ntid = d->point_triangles[d->point_triangles_offset[vid] + j];

                    if (ds->flags[ntid] == 0) {
                        istack_push(ds->t_in, ntid);
                        ds->flags[ntid] = 1;
                        dsearch_addflag(ds, ntid);
                    }
                }
            }
        }
        contains = 0;
    }

    *n = ds->t_out->n;
    *out = ds->t_out->v;
    dsearch_resetflags(ds);
}
