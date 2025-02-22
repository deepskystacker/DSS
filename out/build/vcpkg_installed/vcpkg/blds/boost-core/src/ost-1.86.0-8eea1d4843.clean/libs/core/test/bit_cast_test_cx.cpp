// constexpr test for boost/core/bit.hpp (bit_cast)
//
// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/bit.hpp>
#include <boost/cstdint.hpp>

#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX11_CONSTEXPR)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX11_CONSTEXPR is defined" )

#elif !defined(BOOST_CORE_HAS_BUILTIN_BIT_CAST)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_CORE_HAS_BUILTIN_BIT_CAST is not defined" )

#else

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT( boost::core::bit_cast<boost::uint32_t>( 1.0f ) == 0x3F800000u );
STATIC_ASSERT( boost::core::bit_cast<boost::uint64_t>( 1.0 ) == 0x3FF0000000000000ull );

#endif
