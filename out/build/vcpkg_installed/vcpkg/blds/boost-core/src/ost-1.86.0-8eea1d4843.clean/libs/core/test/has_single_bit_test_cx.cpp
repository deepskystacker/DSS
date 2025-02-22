// constexpr test for boost/core/bit.hpp (has_single_bit)
//
// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX14_CONSTEXPR)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX14_CONSTEXPR is defined" )

#else

#include <boost/core/bit.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT( boost::core::has_single_bit( (unsigned char)0x80 ) == true );
STATIC_ASSERT( boost::core::has_single_bit( (unsigned char)0x90 ) == false );

STATIC_ASSERT( boost::core::has_single_bit( (unsigned short)0x8000 ) == true );
STATIC_ASSERT( boost::core::has_single_bit( (unsigned short)0x9000 ) == false );

STATIC_ASSERT( boost::core::has_single_bit( 0x800000u ) == true );
STATIC_ASSERT( boost::core::has_single_bit( 0x900000u ) == false );

STATIC_ASSERT( boost::core::has_single_bit( 0x80000000ul ) == true );
STATIC_ASSERT( boost::core::has_single_bit( 0x90000000ul ) == false );

STATIC_ASSERT( boost::core::has_single_bit( 0x8000000000ull ) == true );
STATIC_ASSERT( boost::core::has_single_bit( 0x9000000000ull ) == false );

#endif
