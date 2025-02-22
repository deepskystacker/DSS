// constexpr test for boost/core/bit.hpp (rotl, rotr)
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
#include <cstdint>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT( boost::core::rotl( (std::uint8_t)0x11, 1 ) == 0x22 );
STATIC_ASSERT( boost::core::rotr( (std::uint8_t)0x11, 1 ) == 0x88 );

STATIC_ASSERT( boost::core::rotl( (std::uint16_t)0x1111, 1 ) == 0x2222 );
STATIC_ASSERT( boost::core::rotr( (std::uint16_t)0x1111, 1 ) == 0x8888 );

STATIC_ASSERT( boost::core::rotl( (std::uint32_t)0x11111111, 1 ) == 0x22222222 );
STATIC_ASSERT( boost::core::rotr( (std::uint32_t)0x11111111, 1 ) == 0x88888888 );

STATIC_ASSERT( boost::core::rotl( (std::uint64_t)0x1111111111111111, 1 ) == 0x2222222222222222 );
STATIC_ASSERT( boost::core::rotr( (std::uint64_t)0x1111111111111111, 1 ) == 0x8888888888888888 );

#endif
