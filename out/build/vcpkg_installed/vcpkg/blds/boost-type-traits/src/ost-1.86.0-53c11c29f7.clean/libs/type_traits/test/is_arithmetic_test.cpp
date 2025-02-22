
//  (C) Copyright John Maddock 2000. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/is_arithmetic.hpp>
#endif
#include "test.hpp"
#include "check_integral_constant.hpp"

#ifndef BOOST_NO_CXX23_HDR_STDFLOAT
#include <stdfloat>
#endif

TT_TEST_BEGIN(is_arithmetic)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<bool>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<bool const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<bool volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<bool const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<signed char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<signed char const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<signed char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<signed char const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned char const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<char const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned char const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<char const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned short>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<short>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned short const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<short const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned short volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<short volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned short const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<short const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned int const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<int const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned int volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<int volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned int const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<int const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned long const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned long const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long const volatile>::value, true);

#ifdef BOOST_HAS_LONG_LONG

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::ulong_long_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::long_long_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::ulong_long_type const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::long_long_type const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::ulong_long_type volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::long_long_type volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::ulong_long_type const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic< ::boost::long_long_type const volatile>::value, true);

#endif

#ifdef BOOST_HAS_MS_INT64

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int8>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int8>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int8 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int8 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int8 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int8 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int8 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int8 const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int16>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int16>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int16 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int16 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int16 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int16 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int16 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int16 const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int32>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int32>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int32 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int32 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int32 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int32 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int32 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int32 const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int64>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int64>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int64 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int64 const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int64 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int64 volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<unsigned __int64 const volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<__int64 const volatile>::value, true);

#endif

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<double>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<double const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<double volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<double const volatile>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long double>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long double const>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long double volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<long double const volatile>::value, true);

//
// cases that should not be true:
//
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<empty_UDT>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float&>::value, false);
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float&&>::value, false);
#endif
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const float&>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<float[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<foo0_t>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<incomplete_type>::value, false);

#ifndef BOOST_NO_CXX11_CHAR16_T
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<char16_t>::value, true);
#endif
#ifndef BOOST_NO_CXX11_CHAR32_T
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<char32_t>::value, true);
#endif

#if defined(BOOST_HAS_INT128) && !defined(TEST_CUDA_DEVICE)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<boost::int128_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<boost::uint128_type>::value, true);
#endif

#ifdef BOOST_HAS_FLOAT128
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<boost::float128_type>::value, true);
#endif
#ifndef BOOST_NO_CXX23_HDR_STDFLOAT
#if defined(__STDCPP_FLOAT16_T__)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<std::float16_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const std::float16_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<volatile std::float16_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const volatile std::float16_t>::value, true);
#endif
#if defined(__STDCPP_FLOAT32_T__)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<std::float32_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const std::float32_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<volatile std::float32_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const volatile std::float32_t>::value, true);
#endif
#if defined(__STDCPP_FLOAT64_T__)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<std::float32_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const std::float32_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<volatile std::float32_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const volatile std::float32_t>::value, true);
#endif
#if defined(__STDCPP_FLOAT128_T__)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<std::float128_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const std::float128_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<volatile std::float128_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const volatile std::float128_t>::value, true);
#endif
#if defined(__STDCPP_BFLOAT16_T__)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<std::bfloat16_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const std::bfloat16_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<volatile std::bfloat16_t>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_arithmetic<const volatile std::bfloat16_t>::value, true);
#endif
#endif

TT_TEST_END








