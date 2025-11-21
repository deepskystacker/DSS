/******************************************************************************
 *
 * File:        distribute.h        
 *
 * Created:     12/2012
 *
 * Author:      Pavel Sakov
 *              Bureau of Meteorology
 *
 * Description:
 *
 * Revisions:
 *
 *****************************************************************************/

#if !defined(_DISTRIBUTE_H)

extern int nprocesses;
extern int rank;
extern int my_number_of_iterations;
extern int my_first_iteration;
extern int my_last_iteration;
extern int* number_of_iterations;
extern int* first_iteration;
extern int* last_iteration;

void distribute_iterations(int i1, int i2, int nproc, int rank);
void distribute_free(void);

#define _DISTRIBUTE_H
#endif
