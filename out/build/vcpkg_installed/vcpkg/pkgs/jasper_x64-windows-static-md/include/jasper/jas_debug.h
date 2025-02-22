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
 * @file jas_debug.h
 * @brief JasPer Debugging-Related Functionality
 */

#ifndef JAS_DEBUG_H
#define JAS_DEBUG_H

/******************************************************************************\
* Includes.
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#include "jasper/jas_init.h"
#include "jasper/jas_debug.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
* Macros and functions.
\******************************************************************************/

/* Output debugging information to standard error provided that the debug
  level is set sufficiently high. */
#if !defined(NDEBUG)
#define	JAS_DBGLOG(n, x) \
	((jas_get_debug_level() >= (n)) ? (jas_eprintf x) : 0)
#else
#define	JAS_DBGLOG(n, x)
#endif

#if !defined(NDEBUG)
#define	JAS_LOGDEBUGF(n, ...) \
	((jas_get_debug_level() >= (n)) ? jas_logdebugf((n), __VA_ARGS__) : 0)
#else
#define	JAS_LOGDEBUGF(n, ...)
#endif

/*!
@brief
Warn about the use of deprecated functionality.
*/
JAS_EXPORT
void jas_deprecated(const char *fmt, ...);

/*!
@brief
Get the library debug level.

@deprecated
This function is deprecated.
*/
JAS_DEPRECATED
static inline
int jas_getdbglevel(void)
{
	jas_deprecated("jas_getdbglevel is deprecated\n");
	return jas_get_debug_level();
}

/*!
@brief
Set the library debug level.
*/
JAS_EXPORT
int jas_setdbglevel(int dbglevel);

/*!
@brief
Print formatted text for the standard error stream (i.e., stderr).
*/
JAS_EXPORT
int jas_eprintf(const char *fmt, ...);

/*!
@brief
Generate a generic log message.
*/
JAS_EXPORT
int jas_logprintf(const char *fmt, ...);

/*!
@brief
Generate an error log message.
*/
JAS_EXPORT
int jas_logerrorf(const char *fmt, ...);

/*!
@brief
Generate a warning log message.
*/
JAS_EXPORT
int jas_logwarnf(const char *fmt, ...);

/*!
@brief
Generate an informational log message.
*/
JAS_EXPORT
int jas_loginfof(const char *fmt, ...);

/*!
@brief
Generate a debugging log message.
*/
JAS_EXPORT
int jas_logdebugf(int priority, const char *fmt, ...);

/*!
@brief
Dump memory.
*/
int jas_logmemdump(const void *data, size_t len);

/*!
@brief
Dump memory to a stream.
*/
JAS_EXPORT
int jas_memdump(FILE *out, const void *data, size_t len);

/*!
@brief
Convert to a string literal.
*/
#define JAS_STRINGIFY(x) #x

/*!
@brief
Convert to a string literal after macro expansion.
*/
#define JAS_STRINGIFYX(x) JAS_STRINGIFY(x)

#ifdef __cplusplus
}
#endif

#endif
