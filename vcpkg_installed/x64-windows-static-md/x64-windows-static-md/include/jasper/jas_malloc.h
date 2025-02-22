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
 * @file jas_malloc.h
 * @brief JasPer Memory Allocator
 */

#ifndef JAS_MALLOC_H
#define JAS_MALLOC_H

/******************************************************************************\
* Includes.
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#include <jasper/jas_types.h>
#include <jasper/jas_thread.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup module_alloc
 * @{
 */

/******************************************************************************\
* Types.
\******************************************************************************/

/*!
@brief A memory allocator.
*/
typedef struct jas_allocator_s {

	/*!
	Function to clean up the allocator when no longer needed.
	The allocator cannot be used after the clean-up operation is performed.
	This function pointer may be null, in which case the clean-up operation
	is treated as a no-op.
	*/
	void (*cleanup)(struct jas_allocator_s *allocator);

	/*!
	Function to allocate memory.
	This function should have behavior similar to malloc.
	*/
	void *(*alloc)(struct jas_allocator_s *allocator, size_t size);

	/*!
	Function to deallocate memory.
	This function should have behavior similar to free.
	*/
	void (*free)(struct jas_allocator_s *allocator, void *pointer);

	/*!
	Function to reallocate memory.
	This function should have behavior similar to realloc.
	*/
	void *(*realloc)(struct jas_allocator_s *allocator, void *pointer,
	  size_t new_size);

	/*! For future use. */
	void (*(reserved[4]))(void);

} jas_allocator_t;

/*!
@brief The standard library allocator (i.e., a wrapper for malloc and friends).

@details
Essentially, jas_std_allocator_t can be thought of as having an inheritance
relationship with jas_allocator_t.
In particular, jas_std_allocator_t is derived from jas_allocator_t.
*/
typedef struct {

	/* The base class. */
	jas_allocator_t base;

} jas_std_allocator_t;

#if defined(JAS_FOR_INTERNAL_USE_ONLY)
/*
The allocator wrapper type.
This type is an allocator that adds memory usage tracking to another
allocator.
The allocator wrapper does not directly perform memory allocation itself.
Instead, it delegate to another allocator.
*/
typedef struct {

	/* The base class. */
	jas_allocator_t base;

	/* The delegated-to allocator. */
	jas_allocator_t *delegate;

	/* The maximum amount of memory that can be used by the allocator. */
	size_t max_mem;

	/* The current amount of memory in use by the allocator. */
	size_t mem;

#if defined(JAS_THREADS)
	/* A mutex for synchronized access to the allocator. */
	jas_mutex_t mutex;
#endif

} jas_basic_allocator_t;
#endif

/******************************************************************************\
* Data.
\******************************************************************************/

#if defined(JAS_FOR_INTERNAL_USE_ONLY)
extern jas_allocator_t *jas_allocator;
extern jas_std_allocator_t jas_std_allocator;
extern jas_basic_allocator_t jas_basic_allocator;
#endif

/******************************************************************************\
* Functions.
\******************************************************************************/

/*!
@brief
Allocate memory.

@details
This function has an identical behavior as malloc (from the C standard
library), except that a zero-sized allocation returns a non-null pointer
(assuming no out-of-memory error occurs).
*/
JAS_EXPORT
void *jas_malloc(size_t size);

/*!
@brief
Free memory.

@details
This function has an identical behavior as free (from the C standard library).
*/
JAS_EXPORT
void jas_free(void *ptr);

/*!
@brief
Resize a block of allocated memory.

@details
This function has an identical behavior as realloc (from the C standard
library).
*/
JAS_EXPORT
void *jas_realloc(void *ptr, size_t size);

/*!
@brief
Allocate a block of memory and initialize the contents to zero.

@details
This function has an identical behavior as calloc (from the C standard
library).
*/
JAS_EXPORT
void *jas_calloc(size_t num_elements, size_t element_size);

/*!
@brief
Allocate array (with overflow checking).
*/
JAS_EXPORT
void *jas_alloc2(size_t num_elements, size_t element_size);

/*!
@brief
Allocate array of arrays (with overflow checking).
*/
JAS_EXPORT
void *jas_alloc3(size_t num_arrays, size_t array_size, size_t element_size);

/*!
@brief
Resize a block of allocated memory (with overflow checking).
*/
JAS_EXPORT
void *jas_realloc2(void *ptr, size_t num_elements, size_t element_size);

/*!
@brief
Set the maximum memory usage allowed by the allocator wrapper.

@param max_mem
The maximum amount of memory (in bytes) that the allocator can use.


@details
This function sets the maximum amount of memory (in bytes) that the
allocator wrapper is permitted to use to @c max_mem.
If @c max_mem is zero, no limit is imposed on the amount of memory used by
allocator.
This function can only be called if the use of the allocator wrapper
is enabled.
Calling this function if the allocator wrapper is not enabled results
in undefined behavior.
The limit on the amount of memory that the allocator can use should
never be set to a value less than the amount of memory currently being
used by the allocator (as doing so results in undefined behavior).
*/
JAS_EXPORT
void jas_set_max_mem_usage(size_t max_mem);

/*!
@brief
Get the current memory usage from the allocator wrapper.

@details
This function queries the amount of memory currently in use by the allocator
wrapper.
This function can only be called if the use of the allocator wrapper
is enabled.
Calling this function if the allocator wrapper is not enabled results
in undefined behavior.
*/
JAS_EXPORT
size_t jas_get_mem_usage(void);

/*!
@brief
Initialize a memory allocator that uses malloc and related functions
for managing memory.

@param allocator
A pointer to the storage in memory that will hold the state associated
with the allocator.

@details
The object referenced by @c allocator must have a lifetime that extends
until @c jas_allocator_cleanup is called for the allocator.
*/
JAS_EXPORT
void jas_std_allocator_init(jas_std_allocator_t *allocator);

/*!
@brief
Clean up an allocator that is no longer needed.

@details
This function cleans up an allocator, releasing any resources associated
with the allocator.
After clean up is performed, the allocator can no longer be used.
*/
JAS_EXPORT
void jas_allocator_cleanup(jas_allocator_t *allocator);

#if defined(JAS_FOR_INTERNAL_USE_ONLY)

/* This function is for internal library use only. */
void jas_set_allocator(jas_allocator_t* allocator);

/* This function is for internal library use only. */
void jas_basic_allocator_init(jas_basic_allocator_t *allocator,
  jas_allocator_t *delegate, size_t max_mem);

#endif

/*!
@brief
Get the total amount of memory available on the system.

@details
This function may be called prior to the library being initialized.
In fact, this function may be useful for determining a reasonable value
for the memory limit setting to be used during (run-time) library
configuration.

@returns
The total amount of memory available (in bytes) is returned, if this can
be determined.
Otherwise, zero is returned.
*/
JAS_EXPORT
size_t jas_get_total_mem_size(void);

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
