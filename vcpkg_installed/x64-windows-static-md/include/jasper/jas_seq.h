/*
 * Copyright (c) 1999-2000 Image Power, Inc. and the University of
 *   British Columbia.
 * Copyright (c) 2001-2002 Michael David Adams.
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
 * @file jas_seq.h
 * @brief Sequence/Matrix Library
 */

#ifndef JAS_SEQ_H
#define JAS_SEQ_H

/******************************************************************************\
* Includes.
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h> /* IWYU pragma: keep */

#include <jasper/jas_types.h>
#include <jasper/jas_fix.h>
#include <jasper/jas_math.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup module_seq
 * @{
 */

/******************************************************************************\
* Constants.
\******************************************************************************/

/* This matrix is a reference to another matrix. */
#define JAS_MATRIX_REF	0x0001

/******************************************************************************\
* Types.
\******************************************************************************/

/* An element in a sequence. */
typedef jas_fix_t jas_seqent_t;
#define PRIjas_seqent PRIjas_fix

/* An element in a matrix. */
typedef jas_fix_t jas_matent_t;

typedef jas_fix_t jas_matind_t;

/*!
@brief Matrix type.
*/
typedef struct {

	/* Additional state information. */
	int flags_;

	/* The starting horizontal index. */
	jas_matind_t xstart_;

	/* The starting vertical index. */
	jas_matind_t ystart_;

	/* The ending horizontal index. */
	jas_matind_t xend_;

	/* The ending vertical index. */
	jas_matind_t yend_;

	/* The number of rows in the matrix. */
	jas_matind_t numrows_;

	/* The number of columns in the matrix. */
	jas_matind_t numcols_;

	/* Pointers to the start of each row. */
	jas_seqent_t **rows_;

	/* The allocated size of the rows array. */
	int_fast32_t maxrows_;

	/* The matrix data buffer. */
	jas_seqent_t *data_;

	/* The allocated size of the data array. */
	int_fast32_t datasize_;

} jas_matrix_t;

/*!
@struct jas_seq2d_t
@brief Two-dimensional sequence type.
*/
typedef jas_matrix_t jas_seq2d_t;

/*!
@struct jas_seq_t
@brief One-dimensional sequence type.
*/
typedef jas_matrix_t jas_seq_t;

/******************************************************************************\
* Functions/macros for matrix class.
\******************************************************************************/

/*!
@brief
Get the number of rows in a matrix.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_matrix_numrows(const jas_matrix_t *matrix)
{
	return matrix->numrows_;
}

/*!
@brief Get the number of columns in a matrix.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_matrix_numcols(const jas_matrix_t *matrix)
{
	return matrix->numcols_;
}

/*!
@brief
Get the number of elements in a matrix.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_matrix_size(const jas_matrix_t *matrix)
{
	return jas_matrix_numcols(matrix) * jas_matrix_numrows(matrix);
}

/*!
@brief
Test if a matrix is empty (i.e., contains no elements).
*/
JAS_ATTRIBUTE_PURE
static inline bool jas_matrix_empty(const jas_matrix_t *matrix)
{
	return jas_matrix_numcols(matrix) == 0 || jas_matrix_numrows(matrix) == 0;
}

/*!
@brief
Get a matrix element.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t jas_matrix_get(const jas_matrix_t *matrix, jas_matind_t i, jas_matind_t j)
{
	assert(i >= 0 && i < matrix->numrows_ && j >= 0 && j < matrix->numcols_);
	return matrix->rows_[i][j];
}

/*!
@brief
Set a matrix element.
*/
static inline void jas_matrix_set(jas_matrix_t *matrix, jas_matind_t i, jas_matind_t j, jas_seqent_t v)
{
	assert(i >= 0 && i < matrix->numrows_ && j >= 0 && j < matrix->numcols_);
	matrix->rows_[i][j] = v;
}

/*!
@brief
Get an element from a matrix that is known to be a row or column vector.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t jas_matrix_getv(const jas_matrix_t *matrix, jas_matind_t i)
{
	return matrix->numrows_ == 1
		? matrix->rows_[0][i]
		: matrix->rows_[i][0];
}

/*!
@brief
Set an element in a matrix that is known to be a row or column vector.
*/
static inline void jas_matrix_setv(jas_matrix_t *matrix, jas_matind_t i, jas_seqent_t v)
{
	if (matrix->numrows_ == 1)
		matrix->rows_[0][i] = v;
	else
		matrix->rows_[i][0] = v;
}

/*!
@brief
Get the address of an element in a matrix.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t *jas_matrix_getref(const jas_matrix_t *matrix, jas_matind_t i, jas_matind_t j)
{
	return &matrix->rows_[i][j];
}

/*!
@brief
Get a reference to a particular row of a 2-D sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t *jas_matrix_getvref(const jas_matrix_t *matrix, jas_matind_t i)
{
	return matrix->numrows_ > 1
		? jas_matrix_getref(matrix, i, 0)
		: jas_matrix_getref(matrix, 0, i);
}

/*!
@brief
Create a matrix with the specified dimensions.
*/
JAS_EXPORT
jas_matrix_t *jas_matrix_create(jas_matind_t numrows, jas_matind_t numcols);

/*!
@brief
Destroy a matrix.
*/
JAS_EXPORT
void jas_matrix_destroy(jas_matrix_t *matrix);

/*!
@brief
Resize a matrix.  The previous contents of the matrix are lost.
*/
JAS_EXPORT
int jas_matrix_resize(jas_matrix_t *matrix, jas_matind_t numrows, jas_matind_t numcols);

/*!
@brief
Write a matrix to a C standard library stream.
*/
JAS_EXPORT
int jas_matrix_output(jas_matrix_t *matrix, FILE *out);

/*!
@brief
Create a matrix that references part of another matrix.
*/
JAS_EXPORT
int jas_matrix_bindsub(jas_matrix_t *mat0, jas_matrix_t *mat1, jas_matind_t r0,
  jas_matind_t c0, jas_matind_t r1, jas_matind_t c1);

/*!
@brief
Create a matrix that is a reference to a row of another matrix.
*/
static inline int jas_matrix_bindrow(jas_matrix_t *mat0, jas_matrix_t *mat1, jas_matind_t r)
{
	return jas_matrix_bindsub(mat0, mat1, r, 0, r, mat1->numcols_ - 1);
}

/*!
@brief
Create a matrix that is a reference to a column of another matrix.
*/
static inline int jas_matrix_bindcol(jas_matrix_t *mat0, jas_matrix_t *mat1, jas_matind_t c)
{
	return jas_matrix_bindsub(mat0, mat1, 0, c, mat1->numrows_ - 1, c);
}

/*!
@brief
Clip the values of matrix elements to the specified range.
*/
JAS_EXPORT
void jas_matrix_clip(jas_matrix_t *matrix, jas_seqent_t minval,
  jas_seqent_t maxval);

/*!
@brief
Arithmetic shift left of all elements in a matrix.
*/
JAS_EXPORT
void jas_matrix_asl(jas_matrix_t *matrix, unsigned n);

/*!
@brief
Arithmetic shift right of all elements in a matrix.
*/
JAS_EXPORT
void jas_matrix_asr(jas_matrix_t *matrix, unsigned n);

/*!
@brief
Almost-but-not-quite arithmetic shift right of all elements in a matrix.
*/
JAS_EXPORT
void jas_matrix_divpow2(jas_matrix_t *matrix, unsigned n);

/*!
@brief
Set all elements of a matrix to the specified value.
*/
JAS_EXPORT
void jas_matrix_setall(jas_matrix_t *matrix, jas_seqent_t val);

/*!
@brief
The spacing between rows of a matrix.
*/
JAS_ATTRIBUTE_PURE
static inline size_t jas_matrix_rowstep(const jas_matrix_t *matrix)
{
	return matrix->numrows_ > 1
		? (size_t)(matrix->rows_[1] - matrix->rows_[0])
		: 0u;
}

/*!
@brief
The spacing between columns of a matrix.
*/
JAS_ATTRIBUTE_PURE
static inline size_t jas_matrix_step(const jas_matrix_t *matrix)
{
	return matrix->numrows_ > 1
		? jas_matrix_rowstep(matrix)
		: 1;
}

/*!
@brief
Compare two matrices for equality.
*/
JAS_EXPORT
int jas_matrix_cmp(jas_matrix_t *mat0, jas_matrix_t *mat1);

/*!
@brief
Copy a matrix.
*/
JAS_EXPORT
jas_matrix_t *jas_matrix_copy(jas_matrix_t *x);

/*!
@brief
Read a matrix from a C standard library stream.
*/
JAS_EXPORT
jas_matrix_t *jas_matrix_input(FILE *);

/*!
@brief
*/
JAS_ATTRIBUTE_CONST
static inline jas_seqent_t jas_seqent_asl(jas_seqent_t x, unsigned n)
{
#if 0
#ifdef JAS_ENABLE_32BIT
	return jas_least32_asl(x, n);
#else
	return jas_fast32_asl(x, n);
#endif
#endif
	return jas_fix_asl(x, n);
}

/*!
@brief
*/
JAS_ATTRIBUTE_CONST
static inline jas_seqent_t jas_seqent_asr(jas_seqent_t x, unsigned n)
{
#if 0
#ifdef JAS_ENABLE_32BIT
	return jas_least32_asr(x, n);
#else
	return jas_fast32_asr(x, n);
#endif
#endif
	return jas_fix_asr(x, n);
}

/******************************************************************************\
* Functions/macros for 2-D sequence class.
\******************************************************************************/

/*!
@brief
Copy a 2-D sequence.
*/
JAS_EXPORT
jas_seq2d_t *jas_seq2d_copy(jas_seq2d_t *x);

/*!
@brief
Create a 2-D sequence.
*/
JAS_EXPORT
jas_matrix_t *jas_seq2d_create(jas_matind_t xstart, jas_matind_t ystart,
  jas_matind_t xend, jas_matind_t yend);

/*!
@brief
Destroy a 2-D sequence.
*/
static inline void jas_seq2d_destroy(jas_seq2d_t *s)
{
	jas_matrix_destroy(s);
}

/*!
@brief
Get the starting x-coordinate of the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_seq2d_xstart(const jas_seq2d_t *s)
{
	return s->xstart_;
}

/*!
@brief
Get the starting y-coordinate of the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_seq2d_ystart(const jas_seq2d_t *s)
{
	return s->ystart_;
}

/*!
@brief
Get the ending x-coordinate of the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_seq2d_xend(const jas_seq2d_t *s)
{
	return s->xend_;
}

/*!
@brief
Get the ending y-coordinate of the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_seq2d_yend(const jas_seq2d_t *s)
{
	return s->yend_;
}

/*!
@brief
Get a pointer (i.e., reference) to an element of a 2-D sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t *jas_seq2d_getref(const jas_seq2d_t *s, jas_matind_t x, jas_matind_t y)
{
	return jas_matrix_getref(s, y - s->ystart_, x - s->xstart_);
}

/*!
@brief
Get an element of a 2-D sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t jas_seq2d_get(const jas_seq2d_t *s, jas_matind_t x, jas_matind_t y)
{
	return jas_matrix_get(s, y - s->ystart_, x - s->xstart_);
}

/*!
@brief
Get the stride between successive rows in the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline size_t jas_seq2d_rowstep(const jas_seq2d_t *s)
{
	return jas_matrix_rowstep(s);
}

/*!
@brief
Get the number of columns in the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline unsigned jas_seq2d_width(const jas_seq2d_t *s)
{
	return (unsigned)(s->xend_ - s->xstart_);
}

/*!
@brief
Get the number of rows in the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline unsigned jas_seq2d_height(const jas_seq2d_t *s)
{
	return (unsigned)(s->yend_ - s->ystart_);
}

/*!
@brief
Set the shift (i.e., starting x- and y-coordinates) of the sequence.
*/
static inline void jas_seq2d_setshift(jas_seq2d_t *s, jas_matind_t x, jas_matind_t y)
{
	s->xstart_ = x;
	s->ystart_ = y;
	s->xend_ = s->xstart_ + s->numcols_;
	s->yend_ = s->ystart_ + s->numrows_;
}

/*!
@brief
Get the number of elements in the sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_seq2d_size(const jas_seq2d_t *s)
{
	return jas_seq2d_width(s) * jas_seq2d_height(s);
}

/*!
@brief
Test if the sequence is empty (i.e., contains no elements).
*/
JAS_ATTRIBUTE_PURE
static inline bool jas_seq2d_empty(const jas_seq2d_t *s)
{
	return jas_seq2d_width(s) == 0 || jas_seq2d_height(s) == 0;
}

/*!
@brief
Initialize a sequence to reference a subsequence of another sequence.
*/
JAS_EXPORT
int jas_seq2d_bindsub(jas_matrix_t *s, jas_matrix_t *s1, jas_matind_t xstart,
  jas_matind_t ystart, jas_matind_t xend, jas_matind_t yend);

/******************************************************************************\
* Functions/macros for 1-D sequence class.
\******************************************************************************/

/*!
@brief
Create a 1-D sequence.
*/
static inline jas_seq_t *jas_seq_create(jas_matind_t start, jas_matind_t end)
{
	return jas_seq2d_create(start, 0, end, 1);
}

/*!
@brief
Destroy a 1-D sequence.
*/
static inline void jas_seq_destroy(jas_seq_t *seq)
{
	jas_seq2d_destroy(seq);
}

/*!
@brief
Set an element of a sequence.
*/
static inline void jas_seq_set(jas_seq_t *seq, jas_matind_t i, jas_seqent_t v)
{
	seq->rows_[0][i - seq->xstart_] = v;
}

/*!
@brief
Get a pointer (i.e., reference) to an element of a sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t *jas_seq_getref(const jas_seq_t *seq, jas_matind_t i)
{
	return &seq->rows_[0][i - seq->xstart_];
}

/*!
@brief
Get an element of a sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_seqent_t jas_seq_get(const jas_seq_t *seq, jas_matind_t i)
{
	return seq->rows_[0][i - seq->xstart_];
}

/*!
@brief
Get the starting index of a sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_seq_start(const jas_seq_t *seq)
{
	return seq->xstart_;
}

/*!
@brief
Get the ending index of a sequence.
*/
JAS_ATTRIBUTE_PURE
static inline jas_matind_t jas_seq_end(const jas_seq_t *seq)
{
	return seq->xend_;
}

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
