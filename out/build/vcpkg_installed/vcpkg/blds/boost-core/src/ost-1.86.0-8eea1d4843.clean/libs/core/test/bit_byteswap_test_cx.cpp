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
#include <cstdint>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT( boost::core::byteswap( (std::int8_t)0x01 ) == 0x01 );
STATIC_ASSERT( boost::core::byteswap( (std::uint8_t)0xF1 ) == 0xF1 );

STATIC_ASSERT( boost::core::byteswap( (std::int16_t)0x0102 ) == 0x0201 );
STATIC_ASSERT( boost::core::byteswap( (std::uint16_t)0xF1E2 ) == 0xE2F1 );

STATIC_ASSERT( boost::core::byteswap( (std::int32_t)0x01020304 ) == 0x04030201 );
STATIC_ASSERT( boost::core::byteswap( (std::uint32_t)0xF1E2D3C4u ) == 0xC4D3E2F1u );

STATIC_ASSERT( boost::core::byteswap( (std::int64_t)0x0102030405060708ll ) == 0x0807060504030201ll );
STATIC_ASSERT( boost::core::byteswap( (std::uint64_t)0xF1E2D3C4B5A69788ull ) == 0x8897A6B5C4D3E2F1ull );

#endif
