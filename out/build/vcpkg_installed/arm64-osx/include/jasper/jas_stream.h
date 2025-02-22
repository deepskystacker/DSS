/*
 * Copyright (c) 1999-2000 Image Power, Inc. and the University of
 *   British Columbia.
 * Copyright (c) 2001-2003 Michael David Adams.
 * All rights reserved.
 */

/* __START_OF_JASPER_LICENSE__
 * 
 * JasPer License Version 2.0
 * 
 * Copyright (c) 2001-2006 Michael David Adams
 * Copyright (c) 1999-2000 Image Power, Inc.
 * Copyright (c) 1999-2000 The University of British Columbia
 * 
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person (the
 * "User") obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 * 
 * 1.  The above copyright notices and this permission notice (which
 * includes the disclaimer below) shall be included in all copies or
 * substantial portions of the Software.
 * 
 * 2.  The name of a copyright holder shall not be used to endorse or
 * promote products derived from the Software without specific prior
 * written permission.
 * 
 * THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
 * LICENSE.  NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.  THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  NO ASSURANCES ARE
 * PROVIDED BY THE COPYRIGHT HOLDERS THAT THE SOFTWARE DOES NOT INFRINGE
 * THE PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF ANY OTHER ENTITY.
 * EACH COPYRIGHT HOLDER DISCLAIMS ANY LIABILITY TO THE USER FOR CLAIMS
 * BROUGHT BY ANY OTHER ENTITY BASED ON INFRINGEMENT OF INTELLECTUAL
 * PROPERTY RIGHTS OR OTHERWISE.  AS A CONDITION TO EXERCISING THE RIGHTS
 * GRANTED HEREUNDER, EACH USER HEREBY ASSUMES SOLE RESPONSIBILITY TO SECURE
 * ANY OTHER INTELLECTUAL PROPERTY RIGHTS NEEDED, IF ANY.  THE SOFTWARE
 * IS NOT FAULT-TOLERANT AND IS NOT INTENDED FOR USE IN MISSION-CRITICAL
 * SYSTEMS, SUCH AS THOSE USED IN THE OPERATION OF NUCLEAR FACILITIES,
 * AIRCRAFT NAVIGATION OR COMMUNICATION SYSTEMS, AIR TRAFFIC CONTROL
 * SYSTEMS, DIRECT LIFE SUPPORT MACHINES, OR WEAPONS SYSTEMS, IN WHICH
 * THE FAILURE OF THE SOFTWARE OR SYSTEM COULD LEAD DIRECTLY TO DEATH,
 * PERSONAL INJURY, OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE ("HIGH
 * RISK ACTIVITIES").  THE COPYRIGHT HOLDERS SPECIFICALLY DISCLAIM ANY
 * EXPRESS OR IMPLIED WARRANTY OF FITNESS FOR HIGH RISK ACTIVITIES.
 * 
 * __END_OF_JASPER_LICENSE__
 */

/*!
 * @file jas_stream.h
 * @brief I/O Stream Class
 */

#ifndef JAS_STREAM_H
#define JAS_STREAM_H

/******************************************************************************\
* Includes.
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h> /* IWYU pragma: export */

#include <stdio.h>
#if defined(JAS_HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#include <jasper/jas_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup module_iostreams
 * @{
 */

/******************************************************************************\
* Constants.
\******************************************************************************/

/* On most UNIX systems, we probably need to define O_BINARY ourselves. */
#ifndef O_BINARY
#define O_BINARY	0
#endif

/*
 * Stream open flags.
 */

/* The stream was opened for reading. */
#define JAS_STREAM_READ	0x0001
/* The stream was opened for writing. */
#define JAS_STREAM_WRITE	0x0002
/* The stream was opened for appending. */
#define JAS_STREAM_APPEND	0x0004
/* The stream was opened in binary mode. */
#define JAS_STREAM_BINARY	0x0008
/* The stream should be created/truncated. */
#define JAS_STREAM_CREATE	0x0010

/*
 * Stream buffering flags.
 */

/* The stream is unbuffered. */
#define JAS_STREAM_UNBUF	0x0000
/* The stream is line buffered. */
#define JAS_STREAM_LINEBUF	0x0001
/* The stream is fully buffered. */
#define JAS_STREAM_FULLBUF	0x0002
/* The buffering mode mask. */
#define	JAS_STREAM_BUFMODEMASK	0x000f

/* The memory associated with the buffer needs to be deallocated when the
  stream is destroyed. */
#define JAS_STREAM_FREEBUF	0x0008
/* The buffer is currently being used for reading. */
#define JAS_STREAM_RDBUF	0x0010
/* The buffer is currently being used for writing. */
#define JAS_STREAM_WRBUF	0x0020

/*
 * Stream error flags.
 */

/* The end-of-file has been encountered (on reading). */
#define JAS_STREAM_EOF	0x0001
/* An I/O error has been encountered on the stream. */
#define JAS_STREAM_ERR	0x0002
/* The read/write limit has been exceeded. */
#define	JAS_STREAM_RWLIMIT	0x0004
/* The error mask. */
#define JAS_STREAM_ERRMASK \
	(JAS_STREAM_EOF | JAS_STREAM_ERR | JAS_STREAM_RWLIMIT)

/*
 * Other miscellaneous constants.
 */

/* The default buffer size (for fully-buffered operation). */
#define JAS_STREAM_BUFSIZE	8192
/* The default permission mask for file creation. */
#define JAS_STREAM_PERMS	0666

/* The maximum number of characters that can always be put back on a stream. */
#define	JAS_STREAM_MAXPUTBACK	16

/******************************************************************************\
* Types.
\******************************************************************************/

/*
 * Generic file object.
 */

typedef void jas_stream_obj_t;

/*
 * Generic file object operations.
 */

typedef struct {

	/* Read characters from a file object. */
	ssize_t (*read_)(jas_stream_obj_t *obj, char *buf, size_t cnt);

	/* Write characters to a file object. */
	ssize_t (*write_)(jas_stream_obj_t *obj, const char *buf, size_t cnt);

	/* Set the position for a file object. */
	long (*seek_)(jas_stream_obj_t *obj, long offset, int origin);

	/* Close a file object. */
	int (*close_)(jas_stream_obj_t *obj);

} jas_stream_ops_t;

/*!
@brief
I/O stream object.

@warning
Library users should never directly access any of the members of this
class.
The functions/macros provided by the JasPer library API should always
be used.
 */

typedef struct {

	/* The mode in which the stream was opened. */
	int openmode_;

	/* The buffering mode. */
	int bufmode_;

	/* The stream status. */
	int flags_;

	/* The start of the buffer area to use for reading/writing. */
	jas_uchar *bufbase_;

	/* The start of the buffer area excluding the extra initial space for
	  character putback. */
	jas_uchar *bufstart_;

	/* The buffer size. */
	int bufsize_;

	/* The current position in the buffer. */
	jas_uchar *ptr_;

	/* The number of characters that must be read/written before
	the buffer needs to be filled/flushed. */
	int cnt_;

	/* A trivial buffer to be used for unbuffered operation. */
	jas_uchar tinybuf_[JAS_STREAM_MAXPUTBACK + 1];

	/* The operations for the underlying stream file object. */
	const jas_stream_ops_t *ops_;

	/* The underlying stream file object. */
	jas_stream_obj_t *obj_;

	/* The number of characters read/written. */
	long rwcnt_;

	/* The maximum number of characters that may be read/written. */
	long rwlimit_;

} jas_stream_t;

/*
 * Regular file object.
 */

/*
 * File descriptor file object.
 */
typedef struct {
	int fd;
	int flags;
#if defined(JAS_WASI_LIBC)
#define L_tmpnam 4096
#endif
	char pathname[L_tmpnam + 1];
} jas_stream_fileobj_t;

/* Delete underlying file object upon stream close. */
#define	JAS_STREAM_FILEOBJ_DELONCLOSE	0x01
/* Do not close underlying file object upon stream close. */
#define JAS_STREAM_FILEOBJ_NOCLOSE	0x02

/*
 * Memory file object.
 */

typedef struct {

	/* The data associated with this file. */
	jas_uchar *buf_;

	/* The allocated size of the buffer for holding file data. */
	size_t bufsize_;

	/* The length of the file. */
	size_t len_;

	/* The seek position. */
	size_t pos_;

	/* Is the buffer growable? */
	int growable_;

	/* Was the buffer allocated internally? */
	int myalloc_;

} jas_stream_memobj_t;

/******************************************************************************\
* Macros/functions for opening and closing streams.
\******************************************************************************/

/*!
@brief Open a file as a stream.

@param filename
A pointer to the pathname of the file to be opened.
@param mode
A pointer to the string specifying the open mode.
The open mode is similar to that used by the fopen function in the
C standard library.

@return
Upon success, a pointer to the opened stream is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_stream_t *jas_stream_fopen(const char *filename, const char *mode);

/*!
@brief Open a memory buffer as a stream.

@param buffer
A pointer to the buffer to be used to store stream data.
@param buffer_size
The size of the buffer.

@details
<ul>
<li>
If buffer is 0 and buffer_size > 0:
a buffer is dynamically allocated with size buffer_size and this buffer is
not growable.
<li>
If buffer is 0 and buffer_size is 0:
a buffer is dynamically allocated whose size will automatically grow to
accommodate the amount of data written.
<li>
If buffer is not 0:
buffer_size (which, in this case, is not currently allowed to be zero) is
the size of the (nongrowable) buffer pointed to by buffer.
</ul>
*/
JAS_EXPORT
jas_stream_t *jas_stream_memopen(char *buffer, size_t buffer_size);

/*!
@brief
Do not use.
@deprecated
Do not use this function.
This function is deprecated.
Use jas_stream_memopen instead.
*/
JAS_DEPRECATED
JAS_EXPORT
jas_stream_t *jas_stream_memopen2(char *buffer, size_t buffer_size);

/*!
@brief Open a file descriptor as a stream.

@param fd
The file descriptor of the file to open as a stream.
@param mode
A pointer to a string specifying the open mode.
The format of this string is similar to that of the fdopen function
in the C standard library.

@return
Upon success, a pointer to the opened stream is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_stream_t *jas_stream_fdopen(int fd, const char *mode);

/*!
@brief Open a stdio (i.e., C standard library) stream as a stream.

@param path
A pointer to a null-terminated string containing the pathname of the file
to be reopened.
@param mode
A pointer to a null-terminated string containing the mode to be used for
reopening the file.
This string is similar to that used by the fdopen function in the
C standard library.
@param fp
A pointer to the `FILE` (i.e., stdio stream) to be reopened.

@details
It is unspecified whether the open mode specified by mode can be
changed from the open mode used for opening the stdio stream.

@return
Upon success, a pointer to the opened stream is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_stream_t *jas_stream_freopen(const char *path, const char *mode, FILE *fp);

/*!
@brief Open a temporary file as a stream.

@details
A temporary file is created and opened as a stream.
The temporary file is deleted when closed via jas_stream_close().
Some operating systems provide a mechanism for ensuring that a file
is removed when closed.
Such functionality may be used by the implementation when available.

@return
Upon success, a pointer to the opened stream is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_stream_t *jas_stream_tmpfile(void);

/*!
@brief Close a stream.

@param stream
A (nonnull) pointer to the stream to be closed.

@details
The close operation will implicitly flush any pending output
to the stream before closing.
If such a flush operation fails, this will be reflected in
the return value of this function.
For many systems, it is likely that the main reason that this function
can fail is due to an I/O error when flushing buffered output.

@return
If no errors are encountered when closing the stream, 0 is returned.
Otherwise, a nonzero value is returned.
*/
JAS_EXPORT
int jas_stream_close(jas_stream_t *stream);

/******************************************************************************\
* Macros/functions for getting/setting the stream state.
\******************************************************************************/

/*!
@brief Get the EOF indicator for a stream.

@param stream
The stream whose EOF indicator is to be queried.

@return
The value of the EOF indicator is returned.
A nonzero value indicates that the stream has encountered EOF.
*/
#define jas_stream_eof(stream) \
	(((stream)->flags_ & JAS_STREAM_EOF) != 0)

/*!
@brief Get the error indicator for a stream.

@param stream
The stream whose error indicator is to be queried.
@return
The value of the error indicator is returned.
A nonzero value indicates that the stream has encountered an error
of some type (such as an I/O error).
Note that EOF is not an error.
*/
#define jas_stream_error(stream) \
	(((stream)->flags_ & JAS_STREAM_ERR) != 0)

/*!
@brief Clear the error indicator for a stream.

@param stream
The stream whose error indicator is to be cleared.

@todo
TODO/FIXME: Should this macro evaluate to void?
*/
#define jas_stream_clearerr(stream) \
	((stream)->flags_ &= ~(JAS_STREAM_ERR | JAS_STREAM_EOF))

/*!
@brief Get the read/write limit for a stream.
@param stream
A pointer to the stream whose read/write limit is to be queried.
@return
The read/write limit for the stream is returned.
This operation cannot fail.
A negative read/write limit indicates no limit (i.e., an limit that is
effectively infinite).
*/
#define	jas_stream_getrwlimit(stream) \
	(((const jas_stream_t *)(stream))->rwlimit_)

/*!
@brief Set the read/write limit for a stream.

@param stream
A pointer to the stream whose read/write limit is to be set.
@param rwlimit
The new value for the read/write limit.

@details
A negative read/write limit is treated as if it were infinity
(i.e., there is no read/write limit).

@return
The old read/write limit is returned.
*/
JAS_EXPORT long jas_stream_setrwlimit(jas_stream_t *stream, long rwlimit);

/*!
@brief Get the read/write count for a stream.

@param stream
A pointer to the stream whose read/write count is to be queried.

@return
The read/write count is returned.
This operation cannot fail.
*/
#define	jas_stream_getrwcount(stream) \
	(((const jas_stream_t *)(stream))->rwcnt_)

/*!
@brief Set the read/write count for a stream.

@param stream
A pointer to the stream whose read/write count is to be set.
@param rw_count
The new value for the read/write count.
@return
The old value of the read/write count is returned.
This operation cannot fail.
@todo
TODO/FIXME: Should this macro evaluate to void?
*/
JAS_EXPORT
long jas_stream_setrwcount(jas_stream_t *stream, long rw_count);

/******************************************************************************\
* Macros/functions for I/O.
\******************************************************************************/

/* Read a character from a stream. */
#ifndef NDEBUG
/*!
@brief jas_stream_getc
Read a character from a stream.

@param stream
A pointer to the stream from which to read a character.

@returns
If a character is succesfully read, the character is returned.
Otherwise, EOF is returned.
*/
#define	jas_stream_getc(stream)	jas_stream_getc_func(stream)
#else
#define jas_stream_getc(stream)	jas_stream_getc_macro(stream)
#endif

/* Write a character to a stream. */
#ifndef NDEBUG
/*!
@brief jas_stream_putc
Write a character to a stream.

@param stream
A pointer to the stream to which to write the character.
@param c
The character to be written.

@returns
If the character is successfully output, the value of the character is
returned.
Otherwise, EOF is returned.
*/
#define jas_stream_putc(stream, c)	jas_stream_putc_func(stream, c)
#else
#define jas_stream_putc(stream, c)	jas_stream_putc_macro(stream, c)
#endif

/*!
@brief Read characters from a stream into a buffer.

@param stream
A pointer to the stream from which to read data.
@param buffer
A pointer to the start of the buffer.
@param count
A count of the number of characters to read (nominally).

@details
If @c count is zero, the function has no effect (and therefore cannot fail).
Otherwise, the function attempts to read @c count characters from the
stream @c stream into the buffer starting at @c buffer.
The number of characters read can be less than @c count, due to
end-of-file (EOF) or an I/O error.

(This function is analogous to fread with the two read-count
parameters combined into a single size.)

@return
The number of characters read is returned.
In the case that the number of characters read is less than @c count,
jas_stream_eof() and/or jas_stream_error() must be used
to distinguish between:
<ol>
<li>a failure due to an I/O error
<li>a failure due to the read/write limit being exceeded
<li>EOF.
</ol>
(The functions jas_stream_getrwcount() and jas_stream_getrwlimit()
can be used to distinguish between failure due to an I/O error
and failure due to the read/write limit being exceeed.)

@todo
TODO: should jas_stream_error be true if RWLIMIT exceeded?
or maybe introduce a jas_stream_rwlimit predicate?
*/
JAS_EXPORT
size_t jas_stream_read(jas_stream_t *stream, void *buffer, size_t count);

/*!
@brief Attempt to retrieve one or more pending characters of input
from a stream into a buffer
without actually removing the characters from the stream.

@param stream
A pointer to the stream from which to retrieve pending input.
@param buffer
A pointer to the start of the buffer.
@param count
A count of how many characters to retrieve.

@details
The extent to which one can peek into the stream is limited.
Therefore, this function can fail if count is sufficiently large.

@return
Returns the number of bytes copied to the given buffer, or 0 on error
or EOF.

@warning
TODO/FIXME: peeking at EOF should be distinguishable from an I/O error;
also should return type be changed to size_t?

*/
JAS_EXPORT
unsigned jas_stream_peek(jas_stream_t *stream, void *buffer, size_t count);

/*!
@brief Write characters from a buffer to a stream.
@param stream
A pointer to the stream to which to write data.
@param buffer
A pointer to the start of the buffer.
@param count
A count of the number of characters to write.

@details
If @c count is zero, the function has no effect (and therefore cannot fail).
Otherwise, the function will attempt to write @c count characters
from the buffer starting at @c buffer to the stream @c stream.
The number of characters written can be less than @c count due to
an I/O error or the read/write limit being exceeded.

(This function is analogous to fwrite with the two write-count
parameters combined into a single size.)

@return
Upon success, the number of characters successfully written is returned.
If an error occurs, the value returned will be less than @c count.
The jas_stream_error() and jas_stream_rwlimit() function (TODO/CHECK: the latter
of which does not currently exist?) can be used to distinguish between:
<ol>
<li>failure due to an I/O error
<li>failure due to the read/write limit being exceeded
</ol>
*/
JAS_EXPORT
size_t jas_stream_write(jas_stream_t *stream, const void *buffer,
  size_t count);

/*!
@brief Write formatted output to a stream.

@param stream
A pointer to the stream to which to write output.
@param format
A pointer to a format string similar to the printf function in the C standard
library.

@details
The function prints the information associated with the format string
to the specified stream.

@return
Upon success, the number of characters output to the stream is returned.
If an error is encountered, a negative value is returned.

@todo
I think that the return type of int is okay here.
It is consistent with printf and friends.
*/
JAS_EXPORT
int jas_stream_printf(jas_stream_t *stream, const char *format, ...);

/*!
@brief Write a string to a stream.

@param stream
A pointer to the stream to which the string should be written.
@param s
A pointer to a null-terminated string for output.

@details
The null character is not output.

(This function is analogous to fputs for C standard library streams.)

@return
Upon success, a nonnegative value is returned.
Upon failure, a negative value is returned.
*/
JAS_EXPORT
int jas_stream_puts(jas_stream_t *stream, const char *s);

/*!
@brief Read a line of input from a stream.

@param stream
A pointer to the stream from which to read input.
@param buffer
A pointer to the start of the buffer to hold to input to be read.
@param buffer_size
The size of the buffer in characters.

@details
The function reads a line of input from a stream into a buffer.
If a newline character is read, it is placed in the buffer.
Since the buffer may be too small to hold the input,
this operation can fail due to attempted buffer overrun.

(This function is analogous to fgets for C standard library streams.)

@return
If the operation fails (e.g., due to an I/O error or attempted buffer overrun),
a null pointer is returned.
Otherwise, buffer is returned.
*/
JAS_EXPORT
char *jas_stream_gets(jas_stream_t *stream, char *buffer, int buffer_size);

/*!
@brief Look at the next character to be read from a stream without actually
removing the character from the stream.

@param stream
A pointer to the stream to be examined.

@details
This function examines the next character that would be read from the
stream and returns this character without actually removing it from the
stream.

@return
If the peek operation fails (e.g., due to EOF or I/O error),
EOF is returned.
Otherwise, the character that would be next read from the stream
is returned.
*/
#define	jas_stream_peekc(stream) \
	(((stream)->cnt_ <= 0) ? jas_stream_fillbuf(stream, 0) : \
	  ((int)(*(stream)->ptr_)))

/*!
@brief Put a character back on a stream.

@param stream
A pointer to the stream to which the character should be put back.
@param c
The character to put back.

@details
The character @c c (which was presumably read previously from the stream
@c stream) is put back on the stream (as if it had not yet been read).
In other words, this function undoes the effect of jas_stream_getc().
It is unspecified what happens if the character put back was not the
one originally read.
The number of characters that can be pushed back onto the stream
for subsequent reading is limited.
Trying to push back too many characters on a stream will result in an error.
The approximate limit is given by the value of JAS_STREAM_MAXPUTBACK.

@return
Upon success, zero is returned.
If the specified character cannot be pushed back, a negative value is returned.
*/
JAS_EXPORT
int jas_stream_ungetc(jas_stream_t *stream, int c);

/******************************************************************************\
* Macros/functions for getting/setting the stream position.
\******************************************************************************/

/*!
@brief Determine if stream supports seeking.

@param stream
A pointer to the stream to query.

@details
The function is a predicate that tests if the underlying file object
supports seek operations.

@return
If the underlying file object supports seek operations, a (strictly)
positive value is returned.
Otherwise, 0 is returned.
*/
JAS_EXPORT
JAS_ATTRIBUTE_PURE
int jas_stream_isseekable(jas_stream_t *stream);

/*!
@brief Set the current position within the stream.

@param stream
A pointer to the stream for which to set the current position.
@param offset
The new position for the stream.
@param origin
The origin to which this new position is relative.

@details
The origin can be SEEK_CUR, SEEK_SET, or SEEK_END
in a similar fashion as the fseek function in the C standard library
(and the lseek function in POSIX).

@return
Upon success, the new stream position is returned.
Upon failure, a negative value is returned.
*/
JAS_EXPORT
long jas_stream_seek(jas_stream_t *stream, long offset, int origin);

/*!
@brief Get the current position within the stream.

@param stream
A pointer to the stream whose current position is to be queried.

@details
The current position of the stream is returned.

(This function is analogous to ftell for C standard library streams.)

@return
Upon success, the current stream position is returned.
If an error is encountered, a negative value is returned.
*/
JAS_EXPORT
long jas_stream_tell(jas_stream_t *stream);

/*!
@brief Seek to the beginning of a stream.

@param stream
A pointer to the stream whose position is to be set.

@details
The stream position is set to the start of the stream.
This function is equivalent to returning the value
of jas_stream_seek(stream, 0, SEEK_SET).

(This function is analogous to frewind for C standard library streams.)

@return
Upon success, the new stream position is returned.
Otherwise, a negative value is returned.
*/
JAS_EXPORT
int jas_stream_rewind(jas_stream_t *stream);

/******************************************************************************\
* Macros/functions for flushing.
\******************************************************************************/

/*!
@brief Flush any pending output to a stream.

@param stream
A pointer to the stream for which output should be flushed.

@details
The function flushes any buffered output to the underlying file object.

(This function is analogous to fflush for C standard library streams.)

@return
Upon success, zero is returned.
Otherwise, a negative value is returned.
*/
JAS_EXPORT
int jas_stream_flush(jas_stream_t *stream);

/******************************************************************************\
* Miscellaneous macros/functions.
\******************************************************************************/

/*!
@brief Copy data from one stream to another.

@param destination
A pointer to the stream that is the destination for the copy.
@param source
A pointer to the stream that is the source for the copy.
@param count
The number of characters to copy.

@details
The function copies the specified number of characters from the
source stream to the destination stream.
In particular, if @c count is nonnegative, @c count characters are
copied from the source stream @c source to the destination stream
@c destination.
Otherwise (i.e., if @c count is negative), the entire source
stream @c source (i.e., until EOF is reached) is copied to the
destination stream @c destination.

@return
Upon success, 0 is returned; otherwise, -1 is returned.

@todo
TODO/FIXME: should return type be ssize_t and the return value be
the count of the characters copied?
Perhaps, it might be safer to introduce a new function with differing
semantics and deprecate this one?
*/
JAS_EXPORT
int jas_stream_copy(jas_stream_t *destination, jas_stream_t *source,
  ssize_t count);

/*!
@brief Print a hex dump of data read from a stream.

@param stream
A pointer to the stream from which to read data.
@param fp
A pointer to a stdio stream (i.e., FILE) to which to print the hex dump.
@param count
The number of characters to include in the hex dump.

@details
This function prints a hex dump of data read from a stream to a
stdio stream.
This function is most likely to be useful for debugging.

@return
Upon success, 0 is returned.
Otherwise, a negative value is returned.

@todo
TODO/FIXME: should count be unsigned int or size_t instead of int?
*/
JAS_EXPORT
int jas_stream_display(jas_stream_t *stream, FILE *fp, int count);

/*!
@brief Consume (i.e., discard) characters from stream.

@param stream
A pointer to the stream from which to discard data.
@param count
The number of characters to discard.

@details
This function reads and discards  the specified number of characters from the
given stream.

@return
This function returns the number of characters read and discarded.
If an error or EOF is encountered, the number of characters read
will be less than count.
To distinguish EOF from an I/O error, jas_stream_eof() and jas_stream_error()
can be used.
*/
JAS_EXPORT
ssize_t jas_stream_gobble(jas_stream_t *stream, size_t count);

/*!
@brief Write a fill character multiple times to a stream.

@param stream
A pointer to the stream to which to write.
@param count
The number of times to write the fill character to the stream.
@param value
The fill character.

@details
This function writes the given fill character to a stream a
specified number of times.
If a count of zero is specified, the function should have no effect.

@return
The number of times the fill character was written to the stream is
returned.
If this value is less than the specified count, an error must have
occurred.
*/
JAS_EXPORT
ssize_t jas_stream_pad(jas_stream_t *stream, size_t count, int value);

/*!
@brief Get the size of the file associated with the specified stream.

@param stream
A pointer to the stream.

@details
This function queries the size (i.e., length) of the underlying file object
associated with the specified stream.
The specified stream must be seekable.

@return
Upon success, the size of the stream is returned.
If an error occurs, a negative value is returned.

@todo
Should the return type be long or ssize_t?  long is consistent with the
type used for seek offsets.
*/
JAS_EXPORT
long jas_stream_length(jas_stream_t *stream);

/******************************************************************************\
* Internal functions.
\******************************************************************************/

/* The following functions are for internal use only!  If you call them
directly, you will die a horrible, miserable, and painful death! */

/* These prototypes need to be here for the sake of the stream_getc and
stream_putc macros. */
/* Library users must not invoke these functions directly. */
JAS_EXPORT int jas_stream_fillbuf(jas_stream_t *stream, int getflag);
JAS_EXPORT int jas_stream_flushbuf(jas_stream_t *stream, int c);
JAS_EXPORT int jas_stream_getc_func(jas_stream_t *stream);
JAS_EXPORT int jas_stream_putc_func(jas_stream_t *stream, int c);

/* Read a character from a stream. */
static inline int jas_stream_getc2(jas_stream_t *stream)
{
	if (--stream->cnt_ < 0)
		return jas_stream_fillbuf(stream, 1);

	++stream->rwcnt_;
	return (int)(*stream->ptr_++);
}

static inline int jas_stream_getc_macro(jas_stream_t *stream)
{
	if (stream->flags_ & (JAS_STREAM_ERR | JAS_STREAM_EOF | JAS_STREAM_RWLIMIT))
		return EOF;

	if (stream->rwlimit_ >= 0 && stream->rwcnt_ >= stream->rwlimit_) {
		stream->flags_ |= JAS_STREAM_RWLIMIT;
		return EOF;
	}

	return jas_stream_getc2(stream);
}

/* Write a character to a stream. */
static inline int jas_stream_putc2(jas_stream_t *stream, jas_uchar c)
{
	stream->bufmode_ |= JAS_STREAM_WRBUF;

	if (--stream->cnt_ < 0)
		return jas_stream_flushbuf(stream, c);
	else {
		++stream->rwcnt_;
		return (int)(*stream->ptr_++ = c);
	}
}

static inline int jas_stream_putc_macro(jas_stream_t *stream, jas_uchar c)
{
	if (stream->flags_ & (JAS_STREAM_ERR | JAS_STREAM_EOF | JAS_STREAM_RWLIMIT))
	    return EOF;

	if (stream->rwlimit_ >= 0 && stream->rwcnt_ >= stream->rwlimit_) {
		stream->flags_ |= JAS_STREAM_RWLIMIT;
		return EOF;
	}

	return jas_stream_putc2(stream, c);
}

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
