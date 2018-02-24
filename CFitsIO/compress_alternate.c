#include <stdlib.h>
#include <stdio.h>

/* ======================================================================

This file contains stubs for the compression and uncompression routines
that are contained in the source file compress.c.  Those routines (in
compress.c) can only be used by software which adheres to the terms of
the GNU General Public License.  Users who want to use CFITSIO but are
unwilling to release their code under the terms of the GNU General
Public License should replace the compress.c file with this current
file before building the CFITSIO library.   This alternative version of
CFITSIO will behave the same as the standard version, except that it
will not support reading or writing of FITS files in compressed format.

======================================================================== */
/* prototype for the following functions */

void ffpmsg(const char *err_message);

int uncompress2mem(char *filename, 
             FILE *diskfile, 
             char **buffptr, 
             size_t *buffsize, 
             void *(*mem_realloc)(void *p, size_t newsize),
             size_t *filesize,
             int *status);

int uncompress2mem_from_mem(                                                
             char *inmemptr,     
             size_t inmemsize, 
             char **buffptr,  
             size_t *buffsize,  
             void *(*mem_realloc)(void *p, size_t newsize), 
             size_t *filesize,  
             int *status);

int uncompress2file(char *filename, 
             FILE *indiskfile, 
             FILE *outdiskfile, 
             int *status);

int compress2mem_from_mem(                                                
             char *inmemptr,     
             size_t inmemsize, 
             char **buffptr,  
             size_t *buffsize,  
             void *(*mem_realloc)(void *p, size_t newsize), 
             size_t *filesize,  
             int *status);

int compress2file_from_mem(                                                
             char *inmemptr,     
             size_t inmemsize, 
             FILE *outdiskfile, 
             size_t *filesize,   /* O - size of file, in bytes              */
             int *status);
/*--------------------------------------------------------------------------*/
int uncompress2mem(char *filename,  /* name of input file                 */
             FILE *diskfile,     /* I - file pointer                        */
             char **buffptr,   /* IO - memory pointer                     */
             size_t *buffsize,   /* IO - size of buffer, in bytes           */
             void *(*mem_realloc)(void *p, size_t newsize), /* function     */
             size_t *filesize,   /* O - size of file, in bytes              */
             int *status)        /* IO - error status                       */

/*
  Uncompress the file into memory.  Fill whatever amount of memory has
  already been allocated, then realloc more memory, using the supplied
  input function, if necessary.
*/
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support compressed files");
    return(*status = 414);
}
/*--------------------------------------------------------------------------*/
int uncompress2mem_from_mem(                                                
             char *inmemptr,     /* I - memory pointer to compressed bytes */
             size_t inmemsize,   /* I - size of input compressed file      */
             char **buffptr,   /* IO - memory pointer                      */
             size_t *buffsize,   /* IO - size of buffer, in bytes           */
             void *(*mem_realloc)(void *p, size_t newsize), /* function     */
             size_t *filesize,   /* O - size of file, in bytes              */
             int *status)        /* IO - error status                       */

/*
  Uncompress the file into memory.  Fill whatever amount of memory has
  already been allocated, then realloc more memory, using the supplied
  input function, if necessary.
*/
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support compressed files");
    return(*status = 414);
}
/*--------------------------------------------------------------------------*/
int uncompress2file(char *filename,  /* name of input file                  */
             FILE *indiskfile,     /* I - input file pointer                */
             FILE *outdiskfile,    /* I - output file pointer               */
             int *status)        /* IO - error status                       */

/*
  Uncompress the file into file. 
*/
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support compressed files");
    return(*status = 414);
}
/*--------------------------------------------------------------------------*/
int compress2mem_from_mem(                                                
             char *inmemptr,     /* I - memory pointer to uncompressed bytes */
             size_t inmemsize,   /* I - size of input uncompressed file      */
             char **buffptr,   /* IO - memory pointer for compressed file    */
             size_t *buffsize,   /* IO - size of buffer, in bytes           */
             void *(*mem_realloc)(void *p, size_t newsize), /* function     */
             size_t *filesize,   /* O - size of file, in bytes              */
             int *status)        /* IO - error status                       */

/*
  Compress the file into memory.  Fill whatever amount of memory has
  already been allocated, then realloc more memory, using the supplied
  input function, if necessary.
*/
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support compressed files");
    return(*status = 413);
}
/*--------------------------------------------------------------------------*/
int compress2file_from_mem(                                                
             char *inmemptr,     /* I - memory pointer to uncompressed bytes */
             size_t inmemsize,   /* I - size of input uncompressed file      */
             FILE *outdiskfile, 
             size_t *filesize,   /* O - size of file, in bytes              */
             int *status)
/*
  Compress the memory file into disk file. 
*/
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support compressed files");
    return(*status = 413);
}
