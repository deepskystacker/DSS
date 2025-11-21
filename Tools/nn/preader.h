/******************************************************************************
 *
 * File:           preader.h
 *
 * Created:        29/05/2006
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        A header file with preader.c
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_PREADER_H)
#define _PREADER_H

struct preader;
typedef struct preader preader;

preader* preader_create1(double xmin, double xmax, double ymin, double ymax, int nx, int ny, int j1, int j2);
preader* preader_create2(char* fname);
point* preader_getpoint(preader* pr);
void preader_destroy(preader* pr);
int preader_istype1(preader* pr);

#endif
