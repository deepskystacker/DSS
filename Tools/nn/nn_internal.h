/******************************************************************************
 *
 * File:           nn_internal.h
 *
 * Created:        11/03/2005
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Header for internal stuff in the nn library
 *
 * Description:    None
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_NN_INTERNAL_H)
#define _NN_INTERNAL_H

/*
 * nnpi.c
 */
void nnpi_calculate_weights(nnpi* nn, point* p);
int nnpi_get_nvertices(nnpi* nn);
int* nnpi_get_vertices(nnpi* nn);
double* nnpi_get_weights(nnpi* nn);

/*
 * nncommon.c, nncommon-vulnerable.c
 */
int circle_build1(circle* c, point* p0, point* p1, point* p2);
int circle_build2(circle* c, point* p0, point* p1, point* p2);
int circle_contains(circle* c, point* p);
void nn_quit(char* format, ...);
int str2double(char* token, double* value);

#endif
