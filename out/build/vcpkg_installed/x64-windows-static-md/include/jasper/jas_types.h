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
 * @file jas_types.h
 * @brief Primitive Types
 */

#ifndef JAS_TYPES_H
#define JAS_TYPES_H

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

/* Note: The immediately following header files should eventually be removed. */
#include <stddef.h> /* IWYU pragma: export */
#include <stdint.h> /* IWYU pragma: export */

#include <limits.h> /* IWYU pragma: export */

#if defined(JAS_HAVE_SYS_TYPES_H)
#include <sys/types.h> /* IWYU pragma: export */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define jas_uchar unsigned char
#define jas_uint unsigned int
#define jas_ulong unsigned long
#define jas_longlong long long
#define jas_ulonglong unsigned long long

#if !defined(JAS_NO_SET_SSIZE_T)
#	if !defined(SSIZE_MAX)
#		if (JAS_SIZEOF_INT == JAS_SIZEOF_SIZE_T)
#			define ssize_t int
#			define SSIZE_MAX INT_MAX
#		elif (JAS_SIZEOF_LONG == JAS_SIZEOF_SIZE_T)
#			define ssize_t long
#			define SSIZE_MAX LONG_MAX
#		else
#			define ssize_t jas_longlong
#			define SSIZE_MAX LLONG_MAX
#		endif
#	endif
#endif

#if 0
#if defined(JAS_HAVE_SSIZE_T)
#define jas_ssize_t ssize_t
#define JAS_SSIZE_MAX SSIZE_MAX
#else
#define jas_ssize_t jas_longlong
#define JAS_SSIZE_MAX LLONG_MAX
#endif
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1800)
#define bool  int
#define false 0
#define true  1

#define PRIxFAST32 "x"
#define PRIxFAST16 PRIxFAST32
#define PRIuFAST32 "u"
#define PRIuFAST16 PRIuFAST32
#define PRIiFAST32 "i"
#ifdef _WIN64
    #define PRIdPTR "lld"
#else
    #define PRIdPTR "d"
#endif

#ifndef _HUGE_ENUF
    #define _HUGE_ENUF 1e+300
#endif

#define INFINITY ((float)(_HUGE_ENUF * _HUGE_ENUF))

#define strtoull _strtoui64

#else
#include <stdbool.h> /* IWYU pragma: export */
#include <inttypes.h> /* IWYU pragma: export */
#endif

/* The below macro is intended to be used for type casts.  By using this
  macro, type casts can be easily located in the source code with
  tools like "grep". */
#define	JAS_CAST(t, e) \
	((t) (e))

/* The number of bits in the integeral type uint_fast32_t. */
/* NOTE: This could underestimate the size on some exotic architectures. */
#define JAS_UINTFAST32_NUMBITS (8 * sizeof(uint_fast32_t))

#if 0
#if defined(JAS_HAVE_MAX_ALIGN_T)
#define	jas_max_align_t	max_align_t
#else
#define	jas_max_align_t	long double
#endif
#endif

/*
Assume that a compiler claiming to be compliant with C11 or a later version
of the C standard provides a suitable definition of max_align_t.
The JAS_NO_SET_MAX_ALIGN_T preprocessor symbol can be used to override
this behavior.
*/
#if defined(JAS_NO_SET_MAX_ALIGN_T)
	/*
	The user of this header is assuming responsibility for providing a
	suitable definition for max_align_t.
	*/
#elif defined(_MSC_VER)
	/*
	Define max_align_t as a preprocessor symbol since using typedef will
	cause problems.
	*/
#	define max_align_t long double
#elif !(defined(__STDC_VERSION__) && (__STDC_VERSION__ - 0 >= 201112L))
#	define max_align_t long double
#endif

#if 0
#if defined(JAS_HAVE_UINTMAX_T)
#define jas_uintmax_t uintmax_t
#else
#define jas_uintmax_t uint_fast64_t
#endif
#endif

#if 0
#if defined(JAS_HAVE_INTMAX_T)
#define jas_intmax_t intmax_t
#else
#define jas_intmax_t int_fast64_t
#endif
#endif

/* 32-bit unsigned integer type */
typedef uint_least32_t jas_ui32_t;
#define JAS_UI32_MAX UINT_LEAST32_MAX

/* 32-bit signed integer type */
typedef int_least32_t jas_i32_t;
#define JAS_I32_MIN INT_LEAST32_MIN
#define JAS_I32_MAX INT_LEAST32_MAX

#ifdef __cplusplus
}
#endif

#endif
