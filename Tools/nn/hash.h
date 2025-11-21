/******************************************************************************
 *
 * File:           hash.h
 *
 * Purpose:        Hash table header
 *
 * Author:         Jerry Coffin
 *
 * Description:    Public domain code by Jerry Coffin, with improvements by
 *                 HenkJan Wolthuis.
 *                 Date last modified: 05-Jul-1997
 *
 * Revisions:      18-09-2002 -- Pavel Sakov: modified
 *                 07-06-2017 -- Pavel Sakov: changed the hash type from
 *                               unsigned int to uint32_t
 *
 *****************************************************************************/

#ifndef _HASH_H
#define _HASH_H

#include <inttypes.h>

struct hashtable;
typedef struct hashtable hashtable;

/** Copies a key. The key must permit to be deallocated by free().
 */
typedef void* (*ht_keycp) (void*);

/** Returns 1 if two keys are equal, 0 otherwise.
 */
typedef int (*ht_keyeq) (void*, void*);

/** Converts key to an unsigned 32-bit integer (not necessarily unique).
 */
typedef uint32_t(*ht_key2hash) (void*);

hashtable* ht_create(int size, ht_keycp cp, ht_keyeq eq, ht_key2hash hash);

/** Create a hash table of specified size and key type.
 */
hashtable* ht_create_d1(int size);      /* double[1] */
hashtable* ht_create_d2(int size);      /* double[2] */
hashtable* ht_create_str(int size);     /* char* */
hashtable* ht_create_i1(int size);      /* uint32_t[1] */
hashtable* ht_create_i2(int size);      /* uint32_t[2] */
hashtable* ht_create_i1s2(int size);    /* uint32_t[1]uint16_t[2] */
hashtable* ht_create_s4(int size);      /* uint16_t[4] */

void ht_destroy(hashtable* table);
void* ht_insert(hashtable* table, void* key, void* data);
void* ht_find(hashtable* table, void* key);
int ht_findid(hashtable* table, void* key);
void* ht_delete(hashtable* table, void* key);
void ht_process(hashtable* table, void (*func) (void*));
int ht_getnentries(hashtable* table);
int ht_getsize(hashtable* table);
int ht_getnfilled(hashtable* table);

#endif                          /* _HASH_H */
