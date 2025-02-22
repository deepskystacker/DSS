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
 * @file jas_math.h
 * @brief Math-Related Code
 */

#ifndef	JAS_MATH_H
#define	JAS_MATH_H

/******************************************************************************\
* Includes
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#include <jasper/jas_compiler.h>
#include <jasper/jas_types.h>

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
* Macros
\******************************************************************************/

#define JAS_KIBI	JAS_CAST(size_t, 1024)
#define JAS_MEBI	(JAS_KIBI * JAS_KIBI)

/* Compute the absolute value. */
#define	JAS_ABS(x) \
	(((x) >= 0) ? (x) : (-(x)))

/* Compute the minimum of two values. */
#define	JAS_MIN(x, y) \
	(((x) < (y)) ? (x) : (y))

/* Compute the maximum of two values. */
#define	JAS_MAX(x, y) \
	(((x) > (y)) ? (x) : (y))

/* Compute the remainder from division (where division is defined such
  that the remainder is always nonnegative). */
#define	JAS_MOD(x, y) \
	(((x) < 0) ? (((-x) % (y)) ? ((y) - ((-(x)) % (y))) : (0)) : ((x) % (y)))

/* Compute the integer with the specified number of least significant bits
  set to one. */
#define	JAS_ONES(n) \
  ((1 << (n)) - 1)
#if 0
#define	JAS_ONES_X(type, n) \
  ((JAS_CAST(type, 1) << (n)) - 1)
#endif
#define	JAS_POW2_X(type, n) \
  (JAS_CAST(type, 1) << (n))

/******************************************************************************\
*
\******************************************************************************/

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ > 6)
/* suppress clang warning "shifting a negative signed value is
   undefined" in the assertions below */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-negative-value"
#endif

JAS_ATTRIBUTE_CONST
JAS_ATTRIBUTE_DISABLE_UBSAN
inline static int jas_int_asr(int x, unsigned n)
{
	// Ensure that the shift of a negative value appears to behave as a
	// signed arithmetic shift.
	assert(((-1) >> 1) == -1);
	// The behavior is undefined when x is negative.
	// We tacitly assume the behavior is equivalent to a signed
	// arithmetic right shift.
	return x >> n;
}

JAS_ATTRIBUTE_CONST
JAS_ATTRIBUTE_DISABLE_UBSAN
inline static int jas_int_asl(int x, unsigned n)
{
	// Ensure that the shift of a negative value appears to behave as a
	// signed arithmetic shift.
	assert(((-1) << 1) == -2);
	// The behavior is undefined when x is negative.
	// We tacitly assume the behavior is equivalent to a signed
	// arithmetic left shift.
	return x << n;
}

JAS_ATTRIBUTE_CONST
JAS_ATTRIBUTE_DISABLE_UBSAN
inline static int_least32_t jas_least32_asr(int_least32_t x, unsigned n)
{
	// Ensure that the shift of a negative value appears to behave as a
	// signed arithmetic shift.
	assert(((JAS_CAST(int_least32_t, -1)) >> 1) == JAS_CAST(int_least32_t, -1));
	// The behavior is undefined when x is negative.
	// We tacitly assume the behavior is equivalent to a signed
	// arithmetic right shift.
	return x >> n;
}

JAS_ATTRIBUTE_CONST
JAS_ATTRIBUTE_DISABLE_UBSAN
inline static int_least32_t jas_least32_asl(int_least32_t x, unsigned n)
{
	// Ensure that the shift of a negative value appears to behave as a
	// signed arithmetic shift.
	assert(((JAS_CAST(int_least32_t, -1)) << 1) == JAS_CAST(int_least32_t, -2));
	// The behavior is undefined when x is negative.
	// We tacitly assume the behavior is equivalent to a signed
	// arithmetic left shift.
	return x << n;
}

JAS_ATTRIBUTE_CONST
JAS_ATTRIBUTE_DISABLE_UBSAN
inline static int_fast32_t jas_fast32_asr(int_fast32_t x, unsigned n)
{
	// Ensure that the shift of a negative value appears to behave as a
	// signed arithmetic shift.
	assert(((JAS_CAST(int_fast32_t, -1)) >> 1) == JAS_CAST(int_fast32_t, -1));
	// The behavior is undefined when x is negative.
	// We tacitly assume the behavior is equivalent to a signed
	// arithmetic right shift.
	return x >> n;
}

JAS_ATTRIBUTE_CONST
JAS_ATTRIBUTE_DISABLE_UBSAN
inline static int_fast32_t jas_fast32_asl(int_fast32_t x, unsigned n)
{
	// Ensure that the shift of a negative value appears to behave as a
	// signed arithmetic shift.
	assert(((JAS_CAST(int_fast32_t, -1)) << 1) == JAS_CAST(int_fast32_t, -2));
	// The behavior is undefined when x is negative.
	// We tacitly assume the behavior is equivalent to a signed
	// arithmetic left shift.
	return x << n;
}

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ > 6)
#pragma GCC diagnostic pop
#endif

/******************************************************************************\
* Safe integer arithmetic (i.e., with overflow checking).
\******************************************************************************/

/* Compute the product of two size_t integers with overflow checking. */
inline static bool jas_safe_size_mul(size_t x, size_t y, size_t *result)
{
#if jas_has_builtin(__builtin_mul_overflow) || (defined(__GNUC__) && __GNUC__ > 5)
	size_t result_buffer;
	bool valid = !__builtin_mul_overflow(x, y, &result_buffer);
	if (valid && result) {
		*result = result_buffer;
	}
	return valid;
#else
	/* Check if overflow would occur */
	if (x && y > SIZE_MAX / x) {
		/* Overflow would occur. */
		return false;
	}
	if (result) {
		*result = x * y;
	}
	return true;
#endif
}

/* Compute the product of three size_t integers with overflow checking. */
inline static bool jas_safe_size_mul3(size_t a, size_t b, size_t c,
  size_t *result)
{
	size_t tmp;
	if (!jas_safe_size_mul(a, b, &tmp) ||
	  !jas_safe_size_mul(tmp, c, &tmp)) {
		return false;
	}
	if (result) {
		*result = tmp;
	}
	return true;
}

/* Compute the sum of two size_t integers with overflow checking. */
inline static bool jas_safe_size_add(size_t x, size_t y, size_t *result)
{
#if jas_has_builtin(__builtin_add_overflow) || (defined(__GNUC__) && __GNUC__ > 5)
	size_t result_buffer;
	bool valid = !__builtin_add_overflow(x, y, &result_buffer);
	if (valid && result) {
		*result = result_buffer;
	}
	return valid;
#else
	if (y > SIZE_MAX - x) {
		return false;
	}
	if (result) {
		*result = x + y;
	}
	return true;
#endif
}

/* Compute the difference of two size_t integers with overflow checking. */
inline static bool jas_safe_size_sub(size_t x, size_t y, size_t *result)
{
#if jas_has_builtin(__builtin_sub_overflow) || (defined(__GNUC__) && __GNUC__ > 5)
	size_t result_buffer;
	bool valid = !__builtin_sub_overflow(x, y, &result_buffer);
	if (valid && result) {
		*result = result_buffer;
	}
	return valid;
#else
	if (y > x) {
		return false;
	}
	if (result) {
		*result = x - y;
	}
	return true;
#endif
}

/* Compute the product of two int_fast32_t integers with overflow checking. */
inline static bool jas_safe_intfast32_mul(int_fast32_t x, int_fast32_t y,
  int_fast32_t *result)
{
#if jas_has_builtin(__builtin_mul_overflow) || (defined(__GNUC__) && __GNUC__ > 5)
	int_fast32_t result_buffer;
	bool valid = !__builtin_mul_overflow(x, y, &result_buffer);
	if (valid && result) {
		*result = result_buffer;
	}
	return valid;
#else
	if (x > 0) {
		/* x is positive */
		if (y > 0) {
			/* x and y are positive */
			if (x > INT_FAST32_MAX / y) {
				return false;
			}
		} else {
			/* x positive, y nonpositive */
			if (y < INT_FAST32_MIN / x) {
				return false;
			}
		}
	} else {
		/* x is nonpositive */
		if (y > 0) {
			/* x is nonpositive, y is positive */
			if (x < INT_FAST32_MIN / y) {
				return false;
			}
		} else { /* x and y are nonpositive */
			if (x != 0 && y < INT_FAST32_MAX / x) {
				return false;
			}
		}
	}

	if (result) {
		*result = x * y;
	}
	return true;
#endif
}

/* Compute the product of three int_fast32_t integers with overflow checking. */
inline static bool jas_safe_intfast32_mul3(int_fast32_t a, int_fast32_t b,
  int_fast32_t c, int_fast32_t *result)
{
	int_fast32_t tmp;
	if (!jas_safe_intfast32_mul(a, b, &tmp) ||
	  !jas_safe_intfast32_mul(tmp, c, &tmp)) {
		return false;
	}
	if (result) {
		*result = tmp;
	}
	return true;
}

/* Compute the sum of two int_fast32_t integers with overflow checking. */
inline static bool jas_safe_intfast32_add(int_fast32_t x, int_fast32_t y,
  int_fast32_t *result)
{
#if jas_has_builtin(__builtin_add_overflow) || (defined(__GNUC__) && __GNUC__ > 5)
	int_fast32_t result_buffer;
	bool valid = !__builtin_add_overflow(x, y, &result_buffer);
	if (valid && result) {
		*result = result_buffer;
	}
	return valid;
#else
	if ((y > 0 && x > INT_FAST32_MAX - y) ||
	  (y < 0 && x < INT_FAST32_MIN - y)) {
		return false;
	}
	if (result) {
		*result = x + y;
	}
	return true;
#endif
}

#if 0
/*
This function is potentially useful but not currently used.
So, it is commented out.
*/
inline static bool jas_safe_uint_mul(unsigned x, unsigned y, unsigned *result)
{
	/* Check if overflow would occur */
	if (x && y > UINT_MAX / x) {
		/* Overflow would occur. */
		return false;
	}
	if (result) {
		*result = x * y;
	}
	return true;
}
#endif

/******************************************************************************\
* Safe 32-bit unsigned integer arithmetic (i.e., with overflow checking).
\******************************************************************************/

#define JAS_SAFEUI32_MAX (0xffffffffUL)

typedef struct {
	bool valid;
	uint_least32_t value;
} jas_safeui32_t;

JAS_ATTRIBUTE_CONST
static inline jas_safeui32_t jas_safeui32_from_ulong(unsigned long x)
{
	jas_safeui32_t result;
	if (x <= JAS_SAFEUI32_MAX) {
		result.valid = 1;
		result.value = JAS_CAST(uint_least32_t, x);
	} else {
		result.valid = 0;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_PURE
static inline bool jas_safeui32_to_intfast32(jas_safeui32_t x,
  int_fast32_t* y)
{
	const long I32_MAX = 0x7fffffffL;
	if (x.value <= I32_MAX) {
		*y = x.value;
		return true;
	} else {
		return false;
	}
}

JAS_ATTRIBUTE_CONST
static inline jas_safeui32_t jas_safeui32_add(jas_safeui32_t x,
  jas_safeui32_t y)
{
	jas_safeui32_t result;
	if (x.valid && y.valid && y.value <= UINT_LEAST32_MAX - x.value) {
		result.valid = true;
		result.value = x.value + y.value;
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safeui32_t jas_safeui32_sub(jas_safeui32_t x, jas_safeui32_t y)
{
	jas_safeui32_t result;
	if (x.valid && y.valid && y.value <= x.value) {
		result.valid = true;
		result.value = x.value - y.value;
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline jas_safeui32_t jas_safeui32_mul(jas_safeui32_t x,
  jas_safeui32_t y)
{
	jas_safeui32_t result;
	if (!x.valid || !y.valid || (x.value && y.value > UINT_LEAST32_MAX /
	  x.value)) {
		result.valid = false;
		result.value = 0;
	} else {
		result.valid = true;
		result.value = x.value * y.value;
	}
	return result;
}

/******************************************************************************\
* Safe 64-bit signed integer arithmetic (i.e., with overflow checking).
\******************************************************************************/

typedef struct {
	bool valid;
	int_least64_t value;
} jas_safei64_t;

JAS_ATTRIBUTE_CONST
static inline
jas_safei64_t jas_safei64_from_intmax(intmax_t x)
{
	jas_safei64_t result;
	if (x >= INT_LEAST64_MIN && x <= INT_LEAST64_MAX) {
		result.valid = true;
		result.value = JAS_CAST(int_least64_t, x);
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safei64_t jas_safei64_add(jas_safei64_t x, jas_safei64_t y)
{
	jas_safei64_t result;
	if (((y.value > 0) && (x.value > (INT_LEAST64_MAX - y.value))) ||
	  ((y.value < 0) && (x.value < (INT_LEAST64_MIN - y.value)))) {
		result.value = false;
		result.value = 0;
	} else {
		result.valid = true;
		result.value = x.value + y.value;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safei64_t jas_safei64_sub(jas_safei64_t x, jas_safei64_t y)
{
	jas_safei64_t result;
	if ((y.value > 0 && x.value < INT_LEAST64_MIN + y.value) ||
	(y.value < 0 && x.value > INT_LEAST64_MAX + y.value)) {
		result.valid = false;
		result.value = 0;
	} else {
		result.valid = true;
		result.value = x.value - y.value;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safei64_t jas_safei64_mul(jas_safei64_t x, jas_safei64_t y)
{
	jas_safei64_t result;
	if (x.value > 0) {  /* x.value is positive */
		if (y.value > 0) {  /* x.value and y.value are positive */
			if (x.value > (INT_LEAST64_MAX / y.value)) {
				goto error;
			}
		} else { /* x.value positive, y.value nonpositive */
			if (y.value < (INT_LEAST64_MIN / x.value)) {
				goto error;
			}
		} /* x.value positive, y.value nonpositive */
	} else { /* x.value is nonpositive */
		if (y.value > 0) { /* x.value is nonpositive, y.value is positive */
			if (x.value < (INT_LEAST64_MIN / y.value)) {
				goto error;
			}
		} else { /* x.value and y.value are nonpositive */
			if ( (x.value != 0) && (y.value < (INT_LEAST64_MAX / x.value))) {
				goto error;
			}
		} /* End if x.value and y.value are nonpositive */
	} /* End if x.value is nonpositive */
	result.valid = true;
	result.value = x.value * y.value;
	return result;
error:
	result.valid = false;
	result.value = 0;
	return result;
}

#if 0
JAS_ATTRIBUTE_CONST
static inline
jas_safei64_t jas_safei64_div(jas_safei64_t x, jas_safei64_t y)
{
	// TODO/FIXME: Not yet implemented.
	jas_safei64_t result;
	result.valid = false;
	result.value = 0;
	return result;
}
#endif

JAS_ATTRIBUTE_CONST
static inline
jas_i32_t jas_safei64_to_i32(jas_safei64_t x, jas_i32_t invalid_value)
{
	jas_i32_t result;
	if (x.valid && x.value >= JAS_I32_MIN && x.value <= JAS_I32_MAX) {
		result = JAS_CAST(jas_i32_t, x.value);
	} else {
		result = invalid_value;
	}
	return result;
}

/******************************************************************************\
* Safe 64-bit unsigned integer arithmetic (i.e., with overflow checking).
\******************************************************************************/

typedef struct {
	bool valid;
	uint_least64_t value;
} jas_safeui64_t;

JAS_ATTRIBUTE_CONST
static inline
jas_safeui64_t jas_safeui64_from_intmax(intmax_t x)
{
	jas_safeui64_t result;
	if (x >= 0 && x <= UINT_LEAST64_MAX) {
		result.valid = true;
		result.value = JAS_CAST(uint_least64_t, x);
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safeui64_t jas_safeui64_add(jas_safeui64_t x, jas_safeui64_t y)
{
	jas_safeui64_t result;
	if (x.valid && y.valid && y.value <= UINT_LEAST64_MAX - x.value) {
		result.valid = true;
		result.value = x.value + y.value;
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safeui64_t jas_safeui64_sub(jas_safeui64_t x, jas_safeui64_t y)
{
	jas_safeui64_t result;
	if (x.valid && y.valid && y.value <= x.value) {
		result.valid = true;
		result.value = x.value - y.value;
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safeui64_t jas_safeui64_mul(jas_safeui64_t x, jas_safeui64_t y)
{
	jas_safeui64_t result;
	if (!x.valid || !y.valid || (x.value && y.value > UINT_LEAST64_MAX /
	  x.value)) {
		result.valid = false;
		result.value = 0;
	} else {
		result.valid = true;
		result.value = x.value * y.value;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safeui64_t jas_safeui64_div(jas_safeui64_t x, jas_safeui64_t y)
{
	jas_safeui64_t result;
	if (x.valid && y.valid && y.value) {
		result.valid = true;
		result.value = x.value / y.value;
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_safeui64_t jas_safeui64_pow2_intmax(intmax_t x)
{
	jas_safeui64_t result;
	if (x >= 0 && x < 64) {
		result.valid = true;
		result.value = JAS_CAST(uint_least64_t, 1) << x;
	} else {
		result.valid = false;
		result.value = 0;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
int jas_safeui64_to_int(jas_safeui64_t x, int invalid_value)
{
	int result;
	if (x.valid && x.value <= INT_MAX) {
		result = JAS_CAST(int, x.value);
	} else {
		result = invalid_value;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_ui32_t jas_safeui64_to_ui32(jas_safeui64_t x, jas_ui32_t invalid_value)
{
	jas_ui32_t result;
	if (x.valid && x.value <= JAS_UI32_MAX) {
		result = JAS_CAST(jas_ui32_t, x.value);
	} else {
		result = invalid_value;
	}
	return result;
}

JAS_ATTRIBUTE_CONST
static inline
jas_i32_t jas_safeui64_to_i32(jas_safeui64_t x, jas_i32_t invalid_value)
{
	jas_i32_t result;
	if (x.valid && x.value >= JAS_I32_MIN && x.value <= JAS_I32_MAX) {
		result = JAS_CAST(jas_i32_t, x.value);
	} else {
		result = invalid_value;
	}
	return result;
}

/******************************************************************************\
\******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
