/******************************************************************************
 *
 * File:           csa.h
 *
 * Created:        16/10/2002
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        A header for csa library (2D data approximation with
 *                 bivariate C1 cubic spline)
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_CSA_H)
#define _CSA_H

#if !defined(_STRUCT_POINT)
#define _STRUCT_POINT
typedef struct {
    double x;
    double y;
    double z;
} point;
#endif

extern int csa_verbose;
extern char* csa_version;

struct csa;
typedef struct csa csa;

csa* csa_create();
void csa_destroy(csa* a);
void csa_addpoints(csa* a, int n, point points[]);
void csa_addstd(csa* a, int n, double variance[]);
void csa_calculatespline(csa* a);
void csa_approximatepoint(csa* a, point* p);
void csa_approximatepoints(csa* a, int n, point* points);

void csa_setnpmin(csa* a, int npmin);
void csa_setnpmax(csa* a, int npmax);
void csa_setk(csa* a, int k);
void csa_setnppc(csa* a, int nppc);

double* csa_approximatepoints2(int nin, double xin[], double yin[], double zin[], double sigma[], int nout, double xout[], double yout[], int npmin, int npmax, int k, int nppc);

#endif
