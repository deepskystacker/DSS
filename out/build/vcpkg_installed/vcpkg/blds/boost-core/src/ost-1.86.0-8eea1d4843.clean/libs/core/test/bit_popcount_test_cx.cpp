// constexpr test for boost/core/bit.hpp (popcount)
//
// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(_MSC_VER) && _MSC_VER / 10 == 191
# pragma warning(disable: 4307) // '*': integral constant overflow
#endif

#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX14_CONSTEXPR)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX14_CONSTEXPR is defined" )

#else

#include <boost/core/bit.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT( boost::core::popcount( (unsigned char)0x74 ) == 4 );
STATIC_ASSERT( boost::core::popcount( (unsigned short)0x7400 ) == 4 );
STATIC_ASSERT( boost::core::popcount( 0x740000u ) == 4 );
STATIC_ASSERT( boost::core::popcount( 0x74000000ul ) == 4 );
STATIC_ASSERT( boost::core::popcount( 0x7400000000ull ) == 4 );

#endif
