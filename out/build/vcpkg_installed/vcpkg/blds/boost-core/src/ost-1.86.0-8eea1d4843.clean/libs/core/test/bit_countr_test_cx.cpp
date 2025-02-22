// constexpr test for boost/core/bit.hpp (countr_zero, countr_one)
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

STATIC_ASSERT( boost::core::countr_zero( (unsigned char)0xF8 ) == 3 );
STATIC_ASSERT( boost::core::countr_zero( (unsigned short)0xF800 ) == 11 );
STATIC_ASSERT( boost::core::countr_zero( 0xF80000u ) == 19 );
STATIC_ASSERT( boost::core::countr_zero( 0xF8000000ul ) == 27 );
STATIC_ASSERT( boost::core::countr_zero( 0xF800000000ull ) == 35 );

STATIC_ASSERT( boost::core::countr_one( (unsigned char)~0xF8u ) == 3 );
STATIC_ASSERT( boost::core::countr_one( (unsigned short)~0xF800u ) == 11 );
STATIC_ASSERT( boost::core::countr_one( ~0xF80000u ) == 19 );
STATIC_ASSERT( boost::core::countr_one( ~0xF8000000ul ) == 27 );
STATIC_ASSERT( boost::core::countr_one( ~0xF800000000ull ) == 35 );

#endif
