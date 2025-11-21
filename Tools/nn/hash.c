/******************************************************************************
 *
 * File:           hash.c
 *
 * Purpose:        Hash table implementation
 *
 * Author:         Jerry Coffin
 *
 * Description:    Public domain code by Jerry Coffin, with improvements by
 *                 HenkJan Wolthuis.
 *
 * Revisions:      18-09-2002 -- Pavel Sakov: modified
 *                 07-06-2017 -- Pavel Sakov: changed the hash type from
 *                               unsigned int to uint32_t
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "hash.h"

#define SIZEOFDOUBLE 8

/** A hash table consists of an array of these buckets.
 */
typedef struct ht_bucket {
    void* key;
    void* data;
    int id;                     /* unique id -- just in case */
    struct ht_bucket* next;
} ht_bucket;

/** Hash table structure. 
 * Note that more nodes than `size' can be inserted in the table,
 * but performance degrades as this happens.
 */
struct hashtable {
    int size;                   /* table size */
    int n;                      /* current number of entries */
    int naccum;                 /* number of inserted entries */
    int nhash;                  /* number of used table elements */
    ht_keycp cp;
    ht_keyeq eq;
    ht_key2hash hash;
    ht_bucket** table;
};

/** Creates a hash table of specified size.
 *
 * @param size Size of hash table for output points
 * @param cp Key copy function
 * @param eq Key equality check function
 * @param hash Hash value calculation function
 */
hashtable* ht_create(int size, ht_keycp cp, ht_keyeq eq, ht_key2hash hash)
{
    hashtable* table = malloc(sizeof(hashtable));
    ht_bucket** bucket;
    int i;

    assert(table != NULL);

    if (size <= 0) {
        free(table);
        return NULL;
    }

    table->size = size;
    table->table = malloc(sizeof(ht_bucket*) * size);
    assert(table->table != NULL);
    bucket = table->table;

    if (bucket == NULL) {
        free(table);
        return NULL;
    }

    for (i = 0; i < size; ++i)
        bucket[i] = NULL;
    table->n = 0;
    table->naccum = 0;
    table->nhash = 0;
    table->eq = eq;
    table->cp = cp;
    table->hash = hash;

    return table;
}

/* Destroys a hash table. 
 * (Take care of deallocating data by ht_process() prior to destroying the
 * table if necessary.)
 *
 * @param table Hash table to be destroyed
 */
void ht_destroy(hashtable* table)
{
    int i;

    if (table == NULL)
        return;

    for (i = 0; i < table->size; ++i) {
        ht_bucket* bucket;

        for (bucket = (table->table)[i]; bucket != NULL;) {
            ht_bucket* prev = bucket;

            free(bucket->key);
            bucket = bucket->next;
            free(prev);
        }
    }

    free(table->table);
    free(table);
}

/* Inserts a new entry into the hash table.
 *
 * @param table The hash table
 * @param key Ponter to entry's key
 * @param data Pointer to associated data
 * @return Pointer to the old data associated with the key, NULL if the key
 *         wasn't in the table previously
 */
void* ht_insert(hashtable* table, void* key, void* data)
{
    uint32_t val = table->hash(key) % table->size;
    ht_bucket* bucket;

    /*
     * NULL means this bucket hasn't been used yet.  We'll simply allocate
     * space for our new bucket and put our data there, with the table
     * pointing at it. 
     */
    if ((table->table)[val] == NULL) {
        bucket = malloc(sizeof(ht_bucket));
        assert(bucket != NULL);

        bucket->key = table->cp(key);
        bucket->next = NULL;
        bucket->data = data;
        bucket->id = table->naccum;

        (table->table)[val] = bucket;
        table->n++;
        table->naccum++;
        table->nhash++;

        return NULL;
    }

    /*
     * This spot in the table is already in use.  See if the current string
     * has already been inserted, and if so, return corresponding data. 
     */
    for (bucket = (table->table)[val]; bucket != NULL; bucket = bucket->next)
        if (table->eq(key, bucket->key) == 1) {
            void* old_data = bucket->data;

            bucket->data = data;
            bucket->id = table->naccum;
            table->naccum++;

            return old_data;
        }

    /*
     * This key must not be in the table yet.  We'll add it to the head of
     * the list at this spot in the hash table.  Speed would be slightly
     * improved if the list was kept sorted instead.  In this case, this
     * code would be moved into the loop above, and the insertion would take 
     * place as soon as it was determined that the present key in the list
     * was larger than this one. 
     */
    bucket = (ht_bucket*) malloc(sizeof(ht_bucket));
    assert(bucket != NULL);
    bucket->key = table->cp(key);
    bucket->data = data;
    bucket->next = (table->table)[val];
    bucket->id = table->naccum;

    (table->table)[val] = bucket;
    table->n++;
    table->naccum++;

    return NULL;
}

/* Returns a pointer to the data associated with a key.  If the key has
 * not been inserted in the table, returns NULL.
 *
 * @param table The hash table
 * @param key The key
 * @return The associated data or NULL
 */
void* ht_find(hashtable* table, void* key)
{
    uint32_t val = table->hash(key) % table->size;
    ht_bucket* bucket;

    if ((table->table)[val] == NULL)
        return NULL;

    for (bucket = (table->table)[val]; bucket != NULL; bucket = bucket->next)
        if (table->eq(key, bucket->key) == 1)
            return bucket->data;

    return NULL;
}

/** Returns id of the bucket associated with a key.  If the key has
 * not been inserted in the table, returns -1.
 *
 * @param table The hash table
 * @param key The key
 * @return id or -1
 */
int ht_findid(hashtable* table, void* key)
{
    uint32_t val = table->hash(key) % table->size;
    ht_bucket* bucket;

    if ((table->table)[val] == NULL)
        return -1;

    for (bucket = (table->table)[val]; bucket != NULL; bucket = bucket->next)
        if (table->eq(key, bucket->key) == 1)
            return bucket->id;

    return -1;
}

/* Deletes an entry from the table.  Returns a pointer to the data that
 * was associated with the key so that the calling code can dispose it
 * properly.
 *
 * @param table The hash table
 * @param key The key
 * @return The associated data or NULL
 */
void* ht_delete(hashtable* table, void* key)
{
    uint32_t val = table->hash(key) % table->size;
    ht_bucket* prev;
    ht_bucket* bucket;
    void* data;

    if ((table->table)[val] == NULL)
        return NULL;

    /*
     * Traverse the list, keeping track of the previous node in the list.
     * When we find the node to delete, we set the previous node's next
     * pointer to point to the node after ourself instead.  We then delete
     * the key from the present node, and return a pointer to the data it
     * contains. 
     */
    for (prev = NULL, bucket = (table->table)[val]; bucket != NULL; prev = bucket, bucket = bucket->next) {
        if (table->eq(key, bucket->key) == 1) {
            data = bucket->data;
            if (prev != NULL)
                prev->next = bucket->next;
            else {
                /*
                 * If 'prev' still equals NULL, it means that we need to
                 * delete the first node in the list. This simply consists
                 * of putting our own 'next' pointer in the array holding
                 * the head of the list.  We then dispose of the current
                 * node as above. 
                 */
                (table->table)[val] = bucket->next;
                table->nhash--;
            }
            free(bucket->key);
            free(bucket);
            table->n--;

            return data;
        }
    }

    /*
     * If we get here, it means we didn't find the item in the table. Signal 
     * this by returning NULL. 
     */
    return NULL;
}

/* For each entry, calls a specified function with corresponding data as a
 * parameter.
 *
 * @param table The hash table
 * @param func The action function
 */
void ht_process(hashtable* table, void (*func) (void*))
{
    int i;

    for (i = 0; i < table->size; ++i)
        if ((table->table)[i] != NULL) {
            ht_bucket* bucket;

            for (bucket = (table->table)[i]; bucket != NULL; bucket = bucket->next)
                func(bucket->data);
        }
}

/* 
 * functions for for string keys 
 */

static uint32_t strhash(void* key)
{
    char* str = key;
    uint32_t hashvalue = 0;

    while (*str != 0) {
        hashvalue ^= (uint32_t) str[0];
        hashvalue <<= 1;
        str++;
    }

    return hashvalue;
}

static void* strcp(void* key)
{
    return strdup(key);
}

static int streq(void* key1, void* key2)
{
    return !strcmp(key1, key2);
}

/* functions for for double keys */

static uint32_t d1hash(void* key)
{
    uint32_t* v = key;

    return v[0] + v[1];
}

static void* d1cp(void* key)
{
    double* newkey = malloc(sizeof(double));

    *newkey = *(double*) key;

    return newkey;
}

static int d1eq(void* key1, void* key2)
{
    return *(double*) key1 == *(double*) key2;
}

/* 
 * functions for for double[2] keys 
 */

static uint32_t d2hash(void* key)
{
    uint32_t* v = key;

    /*
     * PS: here multiplications suppose to make (a,b) and (b,a) generate
     * different hash values 
     */
    return v[0] + v[1] + v[2] * 3 + v[3] * 7;
}

static void* d2cp(void* key)
{
    double* newkey = malloc(sizeof(double) * 2);

    newkey[0] = ((double*) key)[0];
    newkey[1] = ((double*) key)[1];

    return newkey;
}

static int d2eq(void* key1, void* key2)
{
    return (((double*) key1)[0] == ((double*) key2)[0]) && (((double*) key1)[1] == ((double*) key2)[1]);
}

/* 
 * functions for for int[1] keys 
 */

static uint32_t i1hash(void* key)
{
    return (uint32_t) ((uint32_t *) key)[0];
}

static void* i1cp(void* key)
{
    uint32_t* newkey = malloc(sizeof(int));

    newkey[0] = ((uint32_t *) key)[0];

    return newkey;
}

static int i1eq(void* key1, void* key2)
{
    return (((uint32_t *) key1)[0] == ((uint32_t *) key2)[0]);
}

/* 
 * functions for for int[2] keys 
 */

static uint32_t i2hash(void* key)
{
    uint32_t* v = key;

    return v[0] + (v[1] << 16);
}

static void* i2cp(void* key)
{
    uint32_t* newkey = malloc(sizeof(uint32_t) * 2);

    newkey[0] = ((uint32_t *) key)[0];
    newkey[1] = ((uint32_t *) key)[1];

    return newkey;
}

static int i2eq(void* key1, void* key2)
{
    return (((uint32_t *) key1)[0] == ((uint32_t *) key2)[0]) && (((uint32_t *) key1)[1] == ((uint32_t *) key2)[1]);
}

/* 
 * functions for for int[1]short[2] keys 
 */

static uint32_t i1s2hash(void* key)
{
    uint32_t* vi = key;
    uint16_t* vs = key;

    return vi[0] + ((uint32_t) vs[2] << 16) + ((uint32_t) vs[3] << 24);
}

static void* i1s2cp(void* key)
{
    uint32_t* newkey = malloc(sizeof(uint32_t) * 2);
    uint16_t* s = (uint16_t *) newkey;

    newkey[0] = ((uint32_t *) key)[0];
    s[2] = ((uint16_t *) key)[2];
    s[3] = ((uint16_t *) key)[3];

    return newkey;
}

static int i1s2eq(void* key1, void* key2)
{
    return (((uint32_t *) key1)[0] == ((uint32_t *) key2)[0]) && (((uint16_t *) key1)[2] == ((uint16_t *) key2)[2]) && (((uint16_t *) key1)[3] == ((uint16_t *) key2)[3]);
}

/* 
 * functions for for short[4] keys 
 */

static uint32_t s4hash(void* key)
{
    uint16_t* v = key;

    return (uint32_t) v[0] + ((uint32_t) v[1] << 8) + ((uint32_t) v[2] << 16) + ((uint32_t) v[3] << 24);
}

static void* s4cp(void* key)
{
    uint16_t* newkey = malloc(sizeof(short) * 4);

    newkey[0] = ((uint16_t *) key)[0];
    newkey[1] = ((uint16_t *) key)[1];
    newkey[2] = ((uint16_t *) key)[2];
    newkey[3] = ((uint16_t *) key)[3];

    return newkey;
}

static int s4eq(void* p1, void* p2)
{
    uint16_t* key1 = (uint16_t *) p1;
    uint16_t* key2 = (uint16_t *) p2;

    return (key1[0] == key2[0] && key1[1] == key2[1] && key1[2] == key2[2] && key1[3] == key2[3]);
}

hashtable* ht_create_d1(int size)
{
    assert(sizeof(double) == SIZEOFDOUBLE);
    return ht_create(size, d1cp, d1eq, d1hash);
}

hashtable* ht_create_d2(int size)
{
    assert(sizeof(double) == SIZEOFDOUBLE);
    return ht_create(size, d2cp, d2eq, d2hash);
}

hashtable* ht_create_str(int size)
{
    return ht_create(size, strcp, streq, strhash);
}

hashtable* ht_create_i1(int size)
{
    return ht_create(size, i1cp, i1eq, i1hash);
}

hashtable* ht_create_i2(int size)
{
    return ht_create(size, i2cp, i2eq, i2hash);
}

hashtable* ht_create_i1s2(int size)
{
    return ht_create(size, i1s2cp, i1s2eq, i1s2hash);
}

hashtable* ht_create_s4(int size)
{
    return ht_create(size, s4cp, s4eq, s4hash);
}

int ht_getnentries(hashtable* table)
{
    return table->n;
}

int ht_getsize(hashtable* table)
{
    return table->size;
}

int ht_getnfilled(hashtable* table)
{
    return table->nhash;
}

#if defined(HT_TEST)

#include <stdio.h>
#include <limits.h>

#define BUFSIZE 1024

static void print_double(void* data)
{
    printf(" \"%d\"", (int)* (double*) data);
}

static void print_string(void* data)
{
    printf(" \"%s\"", (char*) data);
}

int main()
{
    double points[] = {
        922803.7855, 7372394.688, 0,
        922849.2037, 7372307.027, 1,
        922894.657, 7372219.306, 2,
        922940.1475, 7372131.528, 3,
        922985.6777, 7372043.692, 4,
        923031.2501, 7371955.802, 5,
        923076.8669, 7371867.857, 6,
        923122.5307, 7371779.861, 7,
        923168.2439, 7371691.816, 8,
        923214.0091, 7371603.722, 9,
        923259.8288, 7371515.583, 10,
        922891.3958, 7372440.117, 11,
        922936.873, 7372352.489, 12,
        922982.3839, 7372264.804, 13,
        923027.9308, 7372177.064, 14,
        923073.5159, 7372089.268, 15,
        923119.1415, 7372001.42, 16,
        923164.8099, 7371913.521, 17,
        923210.5233, 7371825.572, 18,
        923256.2841, 7371737.575, 19,
        923302.0946, 7371649.534, 20,
        923347.9572, 7371561.45, 21,
        922978.9747, 7372485.605, 22,
        923024.5085, 7372398.009, 23,
        923070.0748, 7372310.358, 24,
        923115.6759, 7372222.654, 25,
        923161.3136, 7372134.897, 26,
        923206.9903, 7372047.09, 27,
        923252.7079, 7371959.233, 28,
        923298.4686, 7371871.33, 29,
        923344.2745, 7371783.381, 30,
        923390.1279, 7371695.389, 31,
        923436.0309, 7371607.357, 32,
        923066.5232, 7372531.148, 33,
        923112.1115, 7372443.583, 34,
        923157.7311, 7372355.966, 35,
        923203.3842, 7372268.296, 36,
        923249.0725, 7372180.577, 37,
        923294.7981, 7372092.808, 38,
        923340.5628, 7372004.993, 39,
        923386.3686, 7371917.132, 40,
        923432.2176, 7371829.229, 41,
        923478.1116, 7371741.284, 42,
        923524.0527, 7371653.302, 43,
        923154.0423, 7372576.746, 44,
        923199.6831, 7372489.211, 45,
        923245.3541, 7372401.625, 46,
        923291.0572, 7372313.989, 47,
        923336.7941, 7372226.305, 48,
        923382.5667, 7372138.574, 49,
        923428.3766, 7372050.798, 50,
        923474.2256, 7371962.978, 51,
        923520.1155, 7371875.118, 52,
        923566.0481, 7371787.218, 53,
        923612.0252, 7371699.282, 54,
        923241.533, 7372622.396, 55,
        923287.2244, 7372534.889, 56,
        923332.9449, 7372447.334, 57,
        923378.6963, 7372359.731, 58,
        923424.4801, 7372272.081, 59,
        923470.2979, 7372184.385, 60,
        923516.1513, 7372096.646, 61,
        923562.0418, 7372008.866, 62,
        923607.9709, 7371921.046, 63,
        923653.9402, 7371833.188, 64,
        923699.9514, 7371745.296, 65,
        923328.9962, 7372668.095, 66,
        923374.7365, 7372580.617, 67,
        923420.5049, 7372493.091, 68,
        923466.303, 7372405.519, 69,
        923512.1321, 7372317.901, 70,
        923557.9936, 7372230.24, 71,
        923603.8889, 7372142.536, 72,
        923649.8192, 7372054.793, 73,
        923695.786, 7371967.011, 74,
        923741.7905, 7371879.193, 75,
        923787.8341, 7371791.342, 76,
        923416.4327, 7372713.844, 77,
        923462.2204, 7372626.393, 78,
        923508.0353, 7372538.895, 79,
        923553.8787, 7372451.353, 80,
        923599.7517, 7372363.766, 81,
        923645.6555, 7372276.137, 82,
        923691.5914, 7372188.467, 83,
        923737.5603, 7372100.757, 84,
        923783.5634, 7372013.011, 85,
        923829.6017, 7371925.231, 86,
        923875.6763, 7371837.419, 87,
        923503.8433, 7372759.64, 88,
        923549.6771, 7372672.214, 89,
        923595.5372, 7372584.744, 90,
        923641.4246, 7372497.23, 91,
        923687.3404, 7372409.673, 92,
        923733.2855, 7372322.074, 93,
        923779.2608, 7372234.436, 94,
        923825.2672, 7372146.759, 95,
        923871.3056, 7372059.047, 96,
        923917.3766, 7371971.301, 97,
        923963.4812, 7371883.524, 98,
        923591.2288, 7372805.481, 99,
        923637.1076, 7372718.081, 100,
        923683.0118, 7372630.638, 101,
        923728.9423, 7372543.151, 102,
        923774.8998, 7372455.622, 103,
        923820.8852, 7372368.052, 104,
        923866.8991, 7372280.443, 105,
        923912.9422, 7372192.797, 106,
        923959.015, 7372105.116, 107,
        924005.118, 7372017.402, 108,
        924051.2518, 7371929.657, 109,
        923678.5898, 7372851.367, 110,
        923724.5126, 7372763.992, 111,
        923770.46, 7372676.574, 112,
        923816.4328, 7372589.113, 113,
        923862.4314, 7372501.611, 114,
        923908.4564, 7372414.069, 115,
        923954.5083, 7372326.488, 116,
        924000.5875, 7372238.87, 117,
        924046.6941, 7372151.218, 118,
        924092.8286, 7372063.533, 119,
        924138.9911, 7371975.818, 120
    };

    int size = sizeof(points) / sizeof(double) / 3;
    hashtable* ht;
    int i;

    /*
     * double[2] key 
     */

    printf("\n1. Testing a table with key of double[2] type\n\n");

    printf("  creating a table...");
    ht = ht_create_d2(size);
    printf("done\n");

    printf("  inserting %d values from a data array...", size);
    for (i = 0; i < size; ++i)
        ht_insert(ht, &points[i * 3], &points[i * 3 + 2]);
    printf("done\n");

    printf("  stats:\n");
    printf("    %d entries, %d table elements, %d filled elements\n", ht->n, ht->size, ht->nhash);
    printf("    %f entries per hash value in use\n", (double) ht->n / ht->nhash);

    printf("  finding and printing each 10th data:\n");
    for (i = 0; i < size; i += 10) {
        double* point = &points[i * 3];
        double* data = ht_find(ht, point);

        if (data != NULL)
            printf("    i = %d; data = \"%d\"\n", i, (int)* data);
        else
            printf("    i = %d; data = <none>\n", i);
    }

    printf("  removing every 3rd element...");
    for (i = 0; i < size; i += 3) {
        double* point = &points[i * 3];

        ht_delete(ht, point);
    }
    printf("done\n");

    printf("  stats:\n");
    printf("    %d entries, %d table elements, %d filled elements\n", ht->n, ht->size, ht->nhash);
    printf("    %f entries per hash value in use\n", (double) ht->n / ht->nhash);

    printf("  finding and printing each 10th data:\n");
    for (i = 0; i < size; i += 10) {
        double* point = &points[i * 3];
        double* data = ht_find(ht, point);

        if (data != NULL)
            printf("    i = %d; data = \"%d\"\n", i, (int)* data);
        else
            printf("    i = %d; data = <none>\n", i);
    }

    printf("  printing all data by calling ht_process():\n ");
    ht_process(ht, print_double);

    printf("\n  destroying the hash table...");
    ht_destroy(ht);
    printf("done\n");

    /*
     * char* key 
     */

    printf("\n2. Testing a table with key of char* type\n\n");

    printf("  creating a table...");
    ht = ht_create_str(size);
    printf("done\n");

    printf("  inserting %d elements with deep copy of each data string...", size);
    for (i = 0; i < size; ++i) {
        char key[BUFSIZE];
        char str[BUFSIZE];
        char* data;

        snprintf(key, BUFSIZE, "%d-th key", i);
        snprintf(str, BUFSIZE, "%d-th data", i);
        data = strdup(str);
        ht_insert(ht, key, data);
    }
    printf("done\n");

    printf("  stats:\n");
    printf("    %d entries, %d table elements, %d filled elements\n", ht->n, ht->size, ht->nhash);
    printf("    %f entries per hash value in use\n", (double) ht->n / ht->nhash);

    printf("  finding and printing each 10th data:\n");
    for (i = 0; i < size; i += 10) {
        char key[BUFSIZE];
        char* data;

        snprintf(key, BUFSIZE, "%d-th key", i);
        data = ht_find(ht, key);
        if (data != NULL)
            printf("    i = %d; data = \"%s\"\n", i, data);
        else
            printf("    i = %d; data = <none>\n", i);
    }

    printf("  removing every 3rd element...");
    for (i = 0; i < size; i += 3) {
        char key[BUFSIZE];

        snprintf(key, BUFSIZE, "%d-th key", i);
        free(ht_delete(ht, key));
    }
    printf("done\n");

    printf("  stats:\n");
    printf("    %d entries, %d table elements, %d filled elements\n", ht->n, ht->size, ht->nhash);
    printf("    %f entries per hash value in use\n", (double) ht->n / ht->nhash);

    printf("  finding and printing each 10th data:\n");
    for (i = 0; i < size; i += 10) {
        char key[BUFSIZE];
        char* data;

        snprintf(key, BUFSIZE, "%d-th key", i);
        data = ht_find(ht, key);
        if (data != NULL)
            printf("    i = %d; data = \"%s\"\n", i, data);
        else
            printf("    i = %d; data = <none>\n", i);
    }

    printf("  printing all data by calling ht_process():\n ");
    ht_process(ht, print_string);

    printf("\n  freeing the remaining data by calling ht_process()...");
    ht_process(ht, free);
    printf("done\n");

    printf("  destroying the hash table...");
    ht_destroy(ht);
    printf("done\n");

    return 0;
}

#endif                          /* HT_TEST */
