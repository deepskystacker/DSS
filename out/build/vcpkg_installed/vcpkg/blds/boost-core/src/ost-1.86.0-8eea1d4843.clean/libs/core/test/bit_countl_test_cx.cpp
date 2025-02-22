// constexpr test for boost/core/bit.hpp (countl_zero, countl_one)
//
// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(_MSC_VER)
# pragma warning(disable: 4310) // cast truncates constant value
#endif

#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX14_CONSTEXPR)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX14_CONSTEXPR is defined" )

#elif defined(BOOST_MSVC) && BOOST_MSVC / 10 == 191

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_MSVC is " BOOST_STRINGIZE(BOOST_MSVC) )

#else

#include <boost/core/bit.hpp>
#include <climits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT( boost::core::countl_zero( (unsigned char)0x1F ) == CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_zero( (unsigned short)0x1F ) == sizeof(unsigned short) * CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_zero( 0x1Fu ) == sizeof(unsigned int) * CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_zero( 0x1Ful ) == sizeof(unsigned long) * CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_zero( 0x1Full ) == sizeof(unsigned long long) * CHAR_BIT - 5 );

STATIC_ASSERT( boost::core::countl_one( (unsigned char)~0x1Fu ) == CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_one( (unsigned short)~0x1Fu ) == sizeof(unsigned short) * CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_one( ~0x1Fu ) == sizeof(unsigned int) * CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_one( ~0x1Ful ) == sizeof(unsigned long) * CHAR_BIT - 5 );
STATIC_ASSERT( boost::core::countl_one( ~0x1Full ) == sizeof(unsigned long long) * CHAR_BIT - 5 );

#endif
