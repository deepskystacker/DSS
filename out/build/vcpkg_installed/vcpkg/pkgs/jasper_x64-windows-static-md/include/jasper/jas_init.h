/*
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
 * @file jas_init.h
 * @brief JasPer Initialization/Cleanup Code
 */

#ifndef JAS_INIT_H
#define JAS_INIT_H

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#include "jasper/jas_malloc.h"
#include "jasper/jas_image.h"
#include "jasper/jas_log.h"

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup module_init
 * @{
 */

#if defined(JAS_FOR_INTERNAL_USE_ONLY)
/*
Per-thread library context.
*/
typedef struct {

	/*
	The level of debugging checks/output enabled by the library.
	A larger value corresponds to a greater level of debugging checks/output.
	*/
	int debug_level;

	/*
	The function used to output error/warning/informational messages.
	int (*vlogmsgf)(jas_logtype_t type, const char *format, va_list ap);
	*/
	jas_vlogmsgf_t *vlogmsgf;

	/*
	The image format information to be used to populate the image format
	table.
	*/
	size_t image_numfmts;
	jas_image_fmtinfo_t image_fmtinfos[JAS_IMAGE_MAXFMTS];

	/* The maximum number of samples allowable in an image to be decoded. */
	size_t dec_default_max_samples;

} jas_ctx_t;
#endif

/*!
@brief
An opaque handle type used to represent a JasPer library context.
*/
typedef void *jas_context_t;

/******************************************************************************\
* Library Run-Time Configuration.
\******************************************************************************/

/*!
@brief
Configure the JasPer library with the default configuration settings.

@details
This function configures the JasPer library with the default configuration
settings.
These settings may be change via the @c jas_conf_* family of function
prior to invoking jas_init_library().

@warning
Configuration, initialization, and cleanup of the library must be performed
on the same thread.
*/
JAS_EXPORT
void jas_conf_clear(void);

/*!
@brief Set the multithreading flag for the library.

@details
*/
JAS_EXPORT
void jas_conf_set_multithread(int multithread);

/*!
@brief Set the memory allocator to be used by the library.

@details
The object referenced by @c allocator must have a live at least
until jas_cleanup() is invoked.
How the memory in which @c *allocator reside is allocated is the
responsibility of the caller.
*/
JAS_EXPORT
void jas_conf_set_allocator(jas_allocator_t *allocator);

/*!
@brief Set the initial debug level for the library.

@details
*/
JAS_EXPORT
void jas_conf_set_debug_level(int debug_level);

/*!
@brief
Set the maximum amount of memory that can be used by the library
(assuming the allocator wrapper is not disabled).

@details

@warning
It is strongly recommended that the memory usage limit not be set to an
excessively large value, as this poses security risks (e.g., decoding a
malicious image file could exhaust all virtual memory and effectively
crash the system).
*/
JAS_EXPORT
void jas_conf_set_max_mem_usage(size_t max_mem);

/*!
@brief
Set the default value for the maximum number of samples that is
allowed in an image to be decoded.

@details
*/
JAS_EXPORT
void jas_conf_set_dec_default_max_samples(size_t max_samples);

/*!
@brief
Set the function used by the library to output error, warning, and
informational messages.

@details
*/
JAS_EXPORT
void jas_conf_set_vlogmsgf(jas_vlogmsgf_t *func);

/******************************************************************************\
* Library Initialization and Cleanup.
\******************************************************************************/

/*!
@brief
Initialize the JasPer library with the current configuration settings.

@details
The library must be configured by invoking the jas_conf_clear() function
prior to calling jas_init_library().

@returns
If the initialization of the library is successful, zero is returned;
otherwise, a nonzero value is returned.

@warning
The jas_init_library() function does NOT synchronize with the
jas_conf_clear() function.
Configuration, initialization, and cleanup of the library must be
performed on the same thread.
*/
JAS_EXPORT
int jas_init_library(void);

/*!
@brief
Perform clean up for the JasPer library.

@details
At the point when this function is called, all threads that have called
jas_init_thread() must have called jas_cleanup_thread().

@returns
If the operation is successful, zero is returned.
Otherwise, a nonzero value is returned.
*/
JAS_EXPORT
int jas_cleanup_library(void);

/******************************************************************************\
* Thread Initialization and Cleanup.
\******************************************************************************/

/*!
@brief
Perform per-thread initialization for the JasPer library.

@details
The library must be initialized by invoking the jas_init_library() function
prior to calling jas_init_thread().

@warning
The jas_init_thread() function can only be invoked in a single thread
unless the run-time configuration has enabled multithreading via
jas_set_multithread().
*/
JAS_EXPORT
int jas_init_thread(void);

/*!
@brief
Perform per-thread cleanup for the JasPer library.

@details
*/
JAS_EXPORT
int jas_cleanup_thread(void);

/******************************************************************************\
* Legacy Initialization and Cleanup Functions.
\******************************************************************************/

/*!
@brief
Configure and initialize the JasPer library using the default
configuration settings.

@details
The jas_init() function initializes the JasPer library.
The library must be initialized before most code in the library can be used.

The jas_init() function exists only for reasons of backward compatibility
with earlier versions of the library.
It is recommended that this function not be used.
Instead, the jas_conf_clear() and jas_init_library() functions should be used
to configure and initialize the library.

@returns
If the library is successfully initialized, zero is returned;
otherwise, a nonzero value is returned.

@warning
Configuration, initialization, and cleanup of the library must be performed
on the same thread.

@deprecated
This function is deprecated.
*/
JAS_EXPORT
int jas_init(void);

/*!
@brief
Perform any clean up for the JasPer library.

@details
This function performs any clean up for the JasPer library.

@warning
Configuration, initialization, and cleanup of the library must be performed
on the same thread.

@deprecated
This function is deprecated.
*/
JAS_EXPORT
void jas_cleanup(void);

/******************************************************************************\
* Context Management
\******************************************************************************/

/*!
@brief
Create a context.

@details
*/
JAS_EXPORT
jas_context_t jas_context_create(void);

/*!
@brief
Destroy a context.

@details
The context being destroyed must not be the current context.
*/
JAS_EXPORT
void jas_context_destroy(jas_context_t context);

/*!
@brief
Get the current context for the calling thread.

@details
*/
JAS_EXPORT
jas_context_t jas_get_default_context(void);

/*!
@brief
Get the current context for the calling thread.

@details
*/
JAS_EXPORT
jas_context_t jas_get_context(void);

/*!
@brief
Set the current context for the calling thread.

@details
*/
JAS_EXPORT
void jas_set_context(jas_context_t context);

/******************************************************************************\
* Getting/Setting Context Properties
\******************************************************************************/

/* This function is only for internal use by the library. */
JAS_EXPORT
int jas_get_debug_level_internal(void);

/* This function is only for internal use by the library. */
JAS_EXPORT
size_t jas_get_dec_default_max_samples_internal(void);

/* This function is only for internal use by the library. */
JAS_EXPORT
jas_vlogmsgf_t *jas_get_vlogmsgf_internal(void);

#if defined(JAS_FOR_INTERNAL_USE_ONLY)
#if defined(JAS_HAVE_THREAD_LOCAL)
extern _Thread_local jas_ctx_t *jas_cur_ctx;
#endif

/* This function is only for internal use by the library. */
jas_ctx_t *jas_get_ctx_internal(void);

/* This function is only for internal use by the library. */
static inline jas_ctx_t *jas_get_ctx(void)
{
#if defined(JAS_HAVE_THREAD_LOCAL)
	return jas_cur_ctx ? jas_cur_ctx : jas_get_ctx_internal();
#else
	return JAS_CAST(jas_ctx_t *, jas_get_ctx_internal());
#endif
}
#endif

/*!
@brief
Set the debug level for a particular context.

@details
*/
JAS_EXPORT
void jas_set_debug_level(int debug_level);

/*!
@brief
Get the debug level for a particular context.

@details
*/
static inline int jas_get_debug_level(void)
{
#if defined(JAS_FOR_INTERNAL_USE_ONLY)
	jas_ctx_t *ctx = jas_get_ctx();
	return ctx->debug_level;
#else
	return jas_get_debug_level_internal();
#endif
}

/*!
@brief
Set the default maximum number of samples that a decoder is permitted to
process.

@details
*/
JAS_EXPORT
void jas_set_dec_default_max_samples(size_t max_samples);

/*!
@brief
Get the default maximum number of samples that a decoder is permitted to
process.

@details
*/
static inline size_t jas_get_dec_default_max_samples(void)
{
#if defined(JAS_FOR_INTERNAL_USE_ONLY)
	jas_ctx_t *ctx = jas_get_ctx();
	return ctx->dec_default_max_samples;
#else
	return jas_get_dec_default_max_samples_internal();
#endif
}

/*!
@brief
Set the function to be used for log messages.

@details
*/
JAS_EXPORT
void jas_set_vlogmsgf(jas_vlogmsgf_t *func);

/*!
@brief
Get the function to be used for log messages.

@details
*/
static inline
jas_vlogmsgf_t *jas_get_vlogmsgf(void)
{
#if defined(JAS_FOR_INTERNAL_USE_ONLY)
	jas_ctx_t *ctx = jas_get_ctx();
	return ctx->vlogmsgf;
#else
	return jas_get_vlogmsgf_internal();
#endif
}

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
