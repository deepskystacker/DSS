/******************************************************************************
 *
 * File:           delaunay.h
 *
 * Created:        04/08/2000
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Header for delaunay triangulation wrapper
 *
 * Description:    None
 *
 * Revisions:      30/10/2007 PS: Added fields nflags, nflagsallocated and
 *                   flagids for flag accounting, to make it possible to reset
 *                   only engaged flags rather than the whole array.
 *
 *****************************************************************************/

#if !defined(_DELAUNAY_H)
#define _DELAUNAY_H

typedef struct {
    int vids[3];
} triangle;

typedef struct {
    int tids[3];
} triangle_neighbours;

#if !defined(_STRUCT_DELAUNAY)
#define _STRUCT_DELAUNAY
struct delaunay;
typedef struct delaunay delaunay;
#endif

delaunay* delaunay_build(int np, point points[], int ns, int segments[], int nh, double holes[]);
void delaunay_destroy(delaunay* d);

#if defined(MPI)
#include<mpi.h>

extern int nprocesses;
extern int rank;

#if defined(USE_SHMEM)
/*
 * "sm" stands for "shared memory"
 */
extern MPI_Comm sm_comm;
extern int sm_comm_rank;
extern int sm_comm_size;
#endif
#endif

#endif
