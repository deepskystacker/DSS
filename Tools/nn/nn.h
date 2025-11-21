/******************************************************************************
 *
 * File:           nn.h
 *
 * Created:        04/08/2000
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Header file for nn library
 *
 * Description:    None
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_NN_H)
#define _NN_H

/* Contains version string for the nn package.
 */
extern char* nn_version;

/* Sets the verbosity level within nn package.
 * 0 (default) - silent
 * 1 - verbose
 * 2 - very verbose
 */
extern int nn_verbose;

/* Switches between different formulations for NN weights.
 * SIBSON -- classic formulation by Sibson
 * NON_SIBSONIAN -- alternative formulation by Belikov & Semenov
 *                  
 */
typedef enum { SIBSON, NON_SIBSONIAN } NN_RULE;
extern NN_RULE nn_rule;

/* Limits verbose information to a particular vertex (used mainly for
 * debugging purposes).
 */
extern int nn_test_vertice;

/* "point" is a basic data structure in this package.
 */
#if !defined(_STRUCT_POINT)
#define _STRUCT_POINT
typedef struct {
    double x;
    double y;
    double z;
} point;
#endif

#if !defined(_STRUCT_DELAUNAY)
#define _STRUCT_DELAUNAY
struct delaunay;
typedef struct delaunay delaunay;
#endif

/** Smoothes the input point array by averaging the input x,y and z values
 ** for each cell within virtual rectangular nx by ny grid. The corners of the
 ** grid are created from min and max values of the input array. It also frees
 ** the original array and returns results and new dimension via original
 ** data and size pointers.
 *
 * @param n Pointer to number of points (input/output)
 * @param p Pointer to array of points (input/output) [*n]
 * @param nx Number of x nodes in decimation
 * @param ny Number of y nodes in decimation
 */
void points_thingrid(int* n, point** p, int nx, int ny);

/** Smoothes the input point array by averaging the input data (X,Y and Z
 ** values) until the sum of the distances between points does not exceed the
 ** specified maximum value. It also frees the original array and returns
 ** results and new dimension via original data and size pointers. 
 *
 * @param n Pointer to number of points (input/output)
 * @param p Pointer to array of points (input/output) [*n]
 * @param rmax Maximum allowed accumulated distance
 */
void points_thinlin(int* n, point** p, double rmax);

/* Calculates X and/or Y ranges of the input array of points. If necessary,
 * adjusts the range according to the zoom value.
 *
 * @param n Number of points
 * @param points Array of points
 * @param xmin Min X value if *xmin = NaN on input, not changed otherwise
 * @param xmax Max X value if *xmax = NaN on input, not changed otherwise
 * @param ymin Min Y value if *ymin = NaN on input, not changed otherwise
 * @param ymax Max Y value if *ymax = NaN on input, not changed otherwise
 */
void points_getrange(int n, point points[], double zoom, double* xmin, double* xmax, double* ymin, double* ymax);

/** Generates rectangular grid nx by ny using specified min and max x and y 
 ** values. Allocates space for the output point array, be sure to free it
 ** when necessary!
 *
 * @param xmin Min x value
 * @param xmax Max x value
 * @param ymin Min y value
 * @param ymax Max y value
 * @param nx Number of x nodes
 * @param ny Number of y nodes
 * @param zoom Zoom coefficient
 * @param nout Pointer to number of output points
 * @param pout Pointer to array of output points [*nout]
 */
void points_generate(double xmin, double xmax, double ymin, double ymax, int nx, int ny, int* nout, point** pout);

/** Reads array of points from a columnar file.
 *
 * @param fname File name (can be "stdin" dor stndard input)
 * @param dim Number of dimensions (must be 2 or 3)
 * @param n Pointer to number of points (output)
 * @param points Pointer to array of points [*n] (output)
 */
void points_read(char* fname, int dim, int* n, point** points);

/** Scales Y coordinate so that the resulting set fits into square:
 ** xmax - xmin = ymax - ymin
 *
 * @param n Number of points
 * @param points The points to scale
 * @return Y axis compression coefficient
 */
double points_scaletosquare(int n, point* points);

/** Compresses Y domain by a given multiple.
 *
 * @param n Number of points
 * @param points The points to scale
 * @param Y axis compression coefficient as returned by points_scaletosquare()
 */
void points_scale(int n, point* points, double k);

/** `lpi' -- "Linear Point Interpolator" is a structure for linear
 ** interpolation of data on a "point-to-point" basis.
 *
 * `lpi' interpolates linearly within each triangle resulted from the Delaunay
 * triangluation of the input data. `lpi' is much faster than all Natural
 * Neighbours interpolators below.
 */
struct lpi;
typedef struct lpi lpi;

/** Builds linear interpolator.
 *
 * @param d Delaunay triangulation
 * @return Linear interpolator
 */
lpi* lpi_build(delaunay* d);

/** Destroys linear interpolator.
 *
 * @param l Structure to be destroyed
 */
void lpi_destroy(lpi* l);

/** Finds linearly interpolated value in a point.
 *
 * @param l Linear point interpolator
 * @param p Point to be interpolated (p->x, p->y -- input; p->z -- output)
 */
void lpi_interpolate_point(lpi* l, point* p);

/** Linearly interpolates data in an array of points.
 *
 * @param nin Number of input points
 * @param pin Array of input points [pin]
 * @param nout Number of ouput points
 * @param pout Array of output points [nout]
 */
void lpi_interpolate_points(delaunay* d, int nout, point pout[]);

/** `nnpi' -- "Natural Neighbours Point Interpolator" is a structure for
 ** Natural Neighbours interpolation of data on a "point-to-point" basis.
 *
 * Because it involves weight calculation for each output point, it is not
 * designed to take advantage of consequitive interpolations on the same 
 * sets of input and output points -- use `nnhpi' or `nnai' in these cases.
 */
struct nnpi;
typedef struct nnpi nnpi;

/** Creates Natural Neighbours point interpolator.
 *
 * @param d Delaunay triangulation
 * @return Natural Neighbours interpolation
 */
nnpi* nnpi_create(delaunay* d);

/** Destroys Natural Neighbours point interpolation.
 *
 * @param nn Structure to be destroyed
 */
void nnpi_destroy(nnpi* nn);

/** Performs Natural Neighbours interpolation in a point.
 *
 * @param nn NN point interpolator
 * @param p Point to be interpolated (p->x, p->y -- input; p->z -- output)
 */
void nnpi_interpolate_point(nnpi* nn, point* p);

/** Performs Natural Neighbours interpolation in an array of points.
 *
 * @param nin Number of input points
 * @param pin Array of input points [pin]
 * @param wmin Minimal allowed weight
 * @param nout Number of output points
 * @param pout Array of output points [nout]
 */
void nnpi_interpolate_points(delaunay* d, double wmin, int nout, point pout[]);

/** Sets minimal allowed weight for Natural Neighbours interpolation.
 *
 * For Sibson interpolation, setting wmin = 0 is equivalent to interpolating
 * inside convex hall of the data only (returning NaNs otherwise).
 *
 * @param nn Natural Neighbours point interpolator
 * @param wmin Minimal allowed weight
 */
void nnpi_setwmin(nnpi* nn, double wmin);

/** `nnhpi' -- "Natural Neighbours Hashing Point Interpolator" -- is a
 ** structure for conducting consequitive Natural Neighbours interpolations
 ** from the same set of observation points, designed to take advantage of
 ** repeated interpolations in the same point. It allows to modify Z
 ** coordinate of observed data between interpolations (because this does not
 ** affect the interpolant weights).
 */
struct nnhpi;
typedef struct nnhpi nnhpi;

/** Creates Natural Neighbours hashing point interpolator.
 *
 * @param d Delaunay triangulation
 * @param size Hash table size (should be of order of number of output points)
 * @return Natural Neighbours interpolation
 */
nnhpi* nnhpi_create(delaunay* d, int size);

/** Destroys Natural Neighbours hashing point interpolation.
 *
 * @param nn Structure to be destroyed
 */
void nnhpi_destroy(nnhpi* nn);

/** Performs Natural Neighbours interpolation in a point.
 *
 * @param nnhpi NN hashing point interpolator
 * @param p Point to be interpolated (p->x, p->y -- input; p->z -- output)
 */
void nnhpi_interpolate(nnhpi* nn, point* p);

/** Modifies interpolated data.
 *
 * Finds point* pd in the underlying Delaunay triangulation such that
 * pd->x = p->x and pd->y = p->y, and copies p->z to pd->z. Exits with error
 * if the point is not found.
 *
 * @param nn Natural Neighbours hashing point interpolator
 * @param p New data
 */
void nnhpi_modify_data(nnhpi* nn, point* p);

/** Sets minimal allowed weight for Natural Neighbours interpolation.
 *
 * For Sibson interpolation, setting wmin = 0 is equivalent to interpolating
 * inside convex hall of the data only (returning NaNs otherwise).
 *
 * @param nn Natural Neighbours point hashing interpolator
 * @param wmin Minimal allowed weight
 */
void nnhpi_setwmin(nnhpi* nn, double wmin);

/** `nnai' -- "Natural Neighbours Array Interpolator" is a structure for
 ** conducting consequitive Natural Neighbours interpolations from the same
 ** set of observation points in the same set of points. It allows to modify Z
 ** coordinate of data between interpolations (because this does not
 ** affect the interpolant weights).
 *
 * `nnai' is the fastest of the three Natural Neighbours interpolators in `nn'
 * library.
 */
struct nnai;
typedef struct nnai nnai;

/** Builds Natural Neighbours array interpolator.
 *
 * This includes calculation of weights used in nnai_interpolate().
 *
 * @param d Delaunay triangulation
 * @return Natural Neighbours interpolation
 */
nnai* nnai_build(delaunay* d, int n, double* x, double* y);

/** Destroys Natural Neighbours array interpolator.
 *
 * @param nn Structure to be destroyed
 */
void nnai_destroy(nnai* nn);

/** Conducts NN interpolation in a fixed array of output points using 
 ** data specified in a fixed array of input points. Uses pre-calculated
 ** weights.
 *
 * @param nn NN array interpolator
 * @param zin input data [nn->d->npoints]
 * @param zout output data [nn->n]. Must be pre-allocated!
 */
void nnai_interpolate(nnai* nn, double* zin, double* zout);

/** Sets minimal allowed weight for Natural Neighbours interpolation.
 *
 * For Sibson interpolation, setting wmin = 0 is equivalent to interpolating
 * inside convex hall of the input data only (returning NaNs otherwise).
 *
 * @param nn Natural Neighbours array interpolator
 * @param wmin Minimal allowed weight
 */
void nnai_setwmin(nnai* nn, double wmin);

#endif                          /* _NN_H */
