/******************************************************************************
 *
 * File:           delaunay_internal.h
 *
 * Created:        05/05/2021
 *
 * Author:         Pavel Sakov
 *                 BoM
 *
 * Purpose:        Internal header for delaunay triangulation.
 *
 * Description:    Internal header for delaunay triangulation.
 * Revisions:      
 *
 *****************************************************************************/

#if !defined(_DELAUNAY_INTERNAL_H)
#define _DELAUNAY_INTERNAL_H

#if defined(MPI)
#include <mpi.h>
#endif
#include "istack.h"
#include "delaunay.h"

/** Structure to perform the Delaunay triangulation of a given array of points.
 *
 * Contains a deep copy of the input array of points.
 * Contains triangles, circles and edges resulted from the triangulation.
 * Contains neighbour triangles for each triangle.
 * Contains point to triangle map.
 */
struct delaunay {
    int npoints;
    point* points;
    double xmin;
    double xmax;
    double ymin;
    double ymax;

    int ntriangles;

    triangle* triangles;
    circle* circles;
    triangle_neighbours* neighbours;    /* for delaunay_xytoi() */
    int* n_point_triangles;     /* n_point_triangles[i] is number of
                                 * triangles i-th point belongs to */
    int* point_triangles_offset;
    int* point_triangles;       /* point_triangles[point_triangles_offset[i]
                                 * + j] is the index of j-th triangle i-th
                                 * point belongs to */

#if defined(USE_SHMEM)
    MPI_Win sm_win_delaunaydata;
#endif
};

typedef struct {
    delaunay* d;

    int ntriangles;
    int* flags;
    int first_id;               /* last search result, used in start up of a
                                 * new search */
    istack* t_in;
    istack* t_out;

    /*
     * to keep track of flags set to 1 in the case of very large data sets
     */
    int nflags;
    int nflagsallocated;
    int* flagids;
} dsearch;

int delaunay_xytoi(delaunay* d, point* p, int seed);

dsearch* dsearch_build(delaunay* d);
void dsearch_destroy(dsearch* ds);
void dsearch_circles_find(dsearch* d, point* p, int* n, int** out);

#endif                          /* _DELAUNAY_INTERNAL_H */
