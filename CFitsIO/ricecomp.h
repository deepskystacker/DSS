/* @(#) buffer.h 1.1 98/07/21 12:34:27 */
/* buffer.h: structure for compression to buffer rather than to a file, including
 *   bit I/O buffer
 *
 * R. White, 19 June 1998
 */


typedef unsigned char Buffer_t;

typedef struct {
	int bitbuffer;		/* bit buffer					*/
	int bits_to_go;		/* bits to go in buffer			*/
	Buffer_t *start;	/* start of buffer				*/
	Buffer_t *current;	/* current position in buffer	*/
	Buffer_t *end;		/* end of buffer				*/
} Buffer;

#define buffree(mf)		(free(mf->start), free(mf))
#define bufused(mf)		(mf->current - mf->start)
#define bufreset(mf)	(mf->current = mf->start)

/*
 * getcbuf, putcbuf macros for character IO to buffer
 * putcbuf returns EOF on end of buffer, else returns 0
 */
#define getcbuf(mf) ((mf->current >= mf->end) ? EOF : *(mf->current)++)
#define putcbuf(c,mf) \
	((mf->current >= mf->end) ? \
	EOF :\
	((*(mf->current)++ = c), 0))

/*
 * bufalloc sets up buffer of length n
 */

/*  not needed by CFITSIO

static Buffer *bufalloc(int n)
{
Buffer *mf;

	mf = (Buffer *) malloc(sizeof(Buffer));
	if (mf == (Buffer *)NULL) return((Buffer *)NULL);

	mf->start = (Buffer_t *) malloc(n*sizeof(Buffer_t));
	if (mf->start == (Buffer_t *)NULL) {
		free(mf);
		return((Buffer *)NULL);
	}
	mf->bits_to_go = 8;
	mf->end = mf->start + n;
	mf->current = mf->start;
	return(mf);
}
*/

/*
 * bufrealloc extends buffer (or truncates it) by
 * reallocating memory
 */

/* not needed by CFITSIO 
static int bufrealloc(Buffer *mf, int n)
{
int len;

	len = mf->current - mf->start;

	* silently throw away data if buffer is already longer than n *
	if (len>n) len = n;
	if (len<0) len = 0;

	mf->start = (Buffer_t *) realloc(mf->start, n*sizeof(Buffer_t));
	if (mf->start == (Buffer_t *)NULL) return(0);

	mf->end = mf->start + n;
	mf->current = mf->start + len;
	return(n);
}
*/

/*
 * bufdump dumps contents of buffer to outfile and resets
 * it to be empty.  Returns number of bytes written.
 *
 * Note we don't write out the bit buffer -- you must call
 * done_outputing_bits() first to ensure that the bit buffer
 * is written out.  I do it this way to allow incremental
 * buffer dumps while bit IO is still going on.
 */

/*  not needed by CFITSIO

static int bufdump(FILE *outfile, Buffer *buffer)
{
int ndump;

	ndump = bufused(buffer);
	if (fwrite(buffer->start, 1, ndump, outfile) != ndump) {
		fprintf(stderr, "bufdump: error in write\n");
		exit(1);
    }
	bufreset(buffer);
	return(ndump);
}
*/
