/******************************************************************************
 *
 * File:           istack.h
 *
 * Created:        06/06/2001
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Header for handling stack of integers.
 *
 * Description:    None
 *
 * Revisions:      None
 *
 *****************************************************************************/

#if !defined(_ISTACK_H)
#define _ISTACK_H

#if !defined(_ISTACK_STRUCT)
#define _ISTACK_STRUCT
struct istack;
typedef struct istack istack;
#endif

istack* istack_create(void);
void istack_destroy(istack* s);
void istack_push(istack* s, int v);
int istack_pop(istack* s);
int istack_contains(istack* s, int v);
void istack_reset(istack* s);

#endif
