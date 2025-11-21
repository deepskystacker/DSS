/******************************************************************************
 *
 * File:           istack_internal.h
 *
 * Created:        05/05/2021
 *
 * Author:         Pavel Sakov
 *                 BoM
 *
 * Purpose:        Internal header for istack.
 *
 * Description:    None
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_ISTACK_INTERNAL_H)
#define _ISTACK_INTERNAL_H

#include "istack.h"

struct istack {
    int n;
    int nallocated;
    int* v;
};

#endif                          /* _ISTACK_INTERNAL_H */
