// constexpr test for boost/core/bit.hpp (bit_width)
//
// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX14_CONSTEXPR)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX14_CONSTEXPR is defined" )

#elif defined(BOOST_MSVC) && BOOST_MSVC / 10 == 191

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_MSVC is " BOOST_STRINGIZE(BOOST_MSVC) )

#else

#include <boost/core/bit.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT( boost::core::bit_width( (unsigned char)0x74 ) == 7 );
STATIC_ASSERT( boost::core::bit_width( (unsigned short)0x7400 ) == 15 );
STATIC_ASSERT( boost::core::bit_width( 0x740000u ) == 23 );
STATIC_ASSERT( boost::core::bit_width( 0x74000000ul ) == 31 );
STATIC_ASSERT( boost::core::bit_width( 0x7400000000ull ) == 39 );

#endif
