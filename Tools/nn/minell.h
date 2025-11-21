/******************************************************************************
 *
 * File:           minell.h
 *
 * Created:        24/02/2003
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        A header for the minimal ellipse stuff
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_MINELL_H)
#define _MINELL_H

#if !defined(_STRUCT_POINT)
#define _STRUCT_POINT
typedef struct {
    double x;
    double y;
    double z;
} point;
#endif

#if !defined(_MINELL_STRUCT)
#define _MINELL_STRUCT
struct minell;
typedef struct minell minell;
#endif

/* Note that minell_build() shuffles the input point array */
minell* minell_build(int n, point p[]);
void minell_destroy(minell* me);
void minell_scalepoints(minell* me, int n, point p[]);
void minell_rescalepoints(minell* me, int n, point p[]);

#endif
