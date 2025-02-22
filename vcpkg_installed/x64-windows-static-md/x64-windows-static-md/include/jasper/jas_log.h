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
 * @file jas_log.h
 * @brief JasPer Logging Functionality
 */

#ifndef JAS_LOG_H
#define JAS_LOG_H

/******************************************************************************\
* Includes.
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup module_log
 * @{
 */

/******************************************************************************\
* Macros and functions.
\******************************************************************************/

/*! Log type class for unclassified messages. */
#define JAS_LOGTYPE_CLASS_NULL 0
/*! Log type class for errors. */
#define JAS_LOGTYPE_CLASS_ERROR 1
/*! Log type class for warnings. */
#define JAS_LOGTYPE_CLASS_WARN 2
/*! Log type class for informational messages. */
#define JAS_LOGTYPE_CLASS_INFO 3
/*! Log type class for debugging messages. */
#define JAS_LOGTYPE_CLASS_DEBUG 4
#define JAS_LOGTYPE_NUM_CLASSES 5

#define JAS_LOGTYPE_MAX_PRIORITY 16384

// NOTE: without the @struct, jas_logtype_t autolinks are not generated
/*!
@struct jas_logtype_t
@brief Type used for the log type.
*/
typedef unsigned int jas_logtype_t;

/*!
@brief Type used for formatted message logging function.
*/
typedef int (jas_vlogmsgf_t)(jas_logtype_t, const char *, va_list);

/*!
@brief Create an instance of a logtype.
*/
static inline jas_logtype_t jas_logtype_init(int clas, int priority)
{
	assert(clas >= 0 && clas < JAS_LOGTYPE_NUM_CLASSES);
	assert(priority >= 0 && priority <= JAS_LOGTYPE_MAX_PRIORITY);
	return (clas & 0xf) | (priority << 4);
}

/*!
@brief Get the class of a logtype.
*/
static inline int jas_logtype_getclass(jas_logtype_t type)
{
	return type & 0xf;
}

/*!
@brief Get the priority of a logtype.
*/
static inline int jas_logtype_getpriority(jas_logtype_t type)
{
	return type >> 4;
}

/*!
@brief Print formatted log message.
*/
JAS_EXPORT
int jas_vlogmsgf(jas_logtype_t type, const char *fmt, va_list ap);

/*!
@brief Output a log message to standard error.
*/
JAS_EXPORT
int jas_vlogmsgf_stderr(jas_logtype_t type, const char *fmt, va_list ap);

/*!
@brief Output a log message to nowhere (i.e., discard the message).
*/
JAS_EXPORT
int jas_vlogmsgf_discard(jas_logtype_t type, const char *fmt, va_list ap);

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
