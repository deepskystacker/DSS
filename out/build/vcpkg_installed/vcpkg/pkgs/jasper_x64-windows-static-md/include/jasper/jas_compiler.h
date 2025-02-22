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
 * @file jas_compiler.h
 * @brief Compiler-related macros.
 */

#ifndef JAS_COMPILER_H
#define JAS_COMPILER_H

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#ifdef _MSC_VER
#	ifndef __cplusplus
#		undef inline
#		define inline __inline
#	endif
#endif

#if defined(__GNUC__)
#	define JAS_DEPRECATED __attribute__((deprecated))
#else
#	define JAS_DEPRECATED
#endif

#if defined(__GNUC__)
#	define JAS_ATTRIBUTE_CONST __attribute__((const))
#else
#	define JAS_ATTRIBUTE_CONST
#endif

#if defined(__GNUC__)
#	define JAS_ATTRIBUTE_PURE __attribute__((pure))
#else
#	define JAS_ATTRIBUTE_PURE
#endif

#if defined(__GNUC__)
#	define JAS_FORCE_INLINE inline __attribute__((always_inline))
#else
#	define JAS_FORCE_INLINE inline
#endif

#if defined(__GNUC__)
#	if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#		define JAS_UNREACHABLE() __builtin_unreachable()
#	else
#		define JAS_UNREACHABLE()
#	endif
#elif defined(__clang__)
#		define JAS_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#	define JAS_UNREACHABLE() __assume(0)
#else
#	define JAS_UNREACHABLE()
#endif

#if defined(__GNUC__)
#	define JAS_LIKELY(x) __builtin_expect (!!(x), 1)
#else
#	define JAS_LIKELY(x) (x)
#endif

#if defined(__GNUC__)
#	define JAS_UNLIKELY(x) __builtin_expect (!!(x), 0)
#else
#	define JAS_UNLIKELY(x) (x)
#endif

#if defined(__GNUC__) && __GNUC__ >= 6
#	define JAS_ATTRIBUTE_DISABLE_UBSAN \
	  __attribute__((no_sanitize_undefined))
#elif defined(__clang__)
#	define JAS_ATTRIBUTE_DISABLE_UBSAN \
	  __attribute__((no_sanitize("undefined")))
#else
#	define JAS_ATTRIBUTE_DISABLE_UBSAN
#endif

#ifdef __has_builtin
#define jas_has_builtin(x) __has_builtin(x)
#else
#define jas_has_builtin(x) 0
#endif

/*!
@brief
Indicate that a variable may be unused (in order to avoid a compiler warning).
*/
#define JAS_UNUSED(x) ((void) x)

#endif
