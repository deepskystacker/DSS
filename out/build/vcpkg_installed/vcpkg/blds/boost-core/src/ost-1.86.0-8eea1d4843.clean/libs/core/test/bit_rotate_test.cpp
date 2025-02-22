// Test for boost/core/bit.hpp (rotl, rotr)
//
// Copyright 2020 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/bit.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/detail/splitmix64.hpp>
#include <boost/cstdint.hpp>
#include <limits>

int const M = 256;

template<class T> void test_rotate( T x )
{
    for( int i = 0; i < M; ++i )
    {
        BOOST_TEST_EQ( +boost::core::rotl( x, i ), +boost::core::rotr( x, -i ) );
        BOOST_TEST_EQ( +boost::core::rotl( x, -i ), +boost::core::rotr( x, i ) );

        unsigned const width = std::numeric_limits<T>::digits;
        unsigned r = static_cast<unsigned>( i ) & ( width - 1 );

        if( r == 0 )
        {
            BOOST_TEST_EQ( +boost::core::rotl( x, i ), +x );
            BOOST_TEST_EQ( +boost::core::rotr( x, i ), +x );
        }
        else
        {
            BOOST_TEST_EQ( +boost::core::rotl( x, i ), +static_cast<T>( (x << r) | (x >> (width - r)) ) );
            BOOST_TEST_EQ( +boost::core::rotr( x, i ), +static_cast<T>( (x >> r) | (x << (width - r)) ) );
        }
    }
}

int main()
{
    {
        boost::uint8_t x = 0x11;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x22 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x88 );

        x = 0x22;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x44 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x11 );

        x = 0x44;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x88 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x22 );

        x = 0x88;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x11 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x44 );
    }

    {
        boost::uint16_t x = 0x1111;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x2222 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x8888 );

        x = 0x2222;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x4444 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x1111 );

        x = 0x4444;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x8888 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x2222 );

        x = 0x8888;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x1111 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x4444 );
    }

    {
        boost::uint32_t x = 0x11111111;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x22222222 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x88888888 );

        x = 0x22222222;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x44444444 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x11111111 );

        x = 0x44444444;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x88888888 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x22222222 );

        x = 0x88888888;

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), 0x11111111 );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), 0x44444444 );
    }

#define CONST64(x) ((boost::uint64_t(x) << 32) + (x))

    {
        boost::uint64_t x = CONST64(0x11111111);

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), CONST64(0x22222222) );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), CONST64(0x88888888) );

        x = CONST64(0x22222222);

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), CONST64(0x44444444) );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), CONST64(0x11111111) );

        x = CONST64(0x44444444);

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), CONST64(0x88888888) );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), CONST64(0x22222222) );

        x = CONST64(0x88888888);

        BOOST_TEST_EQ( +boost::core::rotl( x, 1 ), CONST64(0x11111111) );
        BOOST_TEST_EQ( +boost::core::rotr( x, 1 ), CONST64(0x44444444) );
    }

    for( int i = -M; i <= M; ++i )
    {
        {
            unsigned char x = 0;
            BOOST_TEST_EQ( +boost::core::rotl( x, i ), +x );
            BOOST_TEST_EQ( +boost::core::rotr( x, i ), +x );
        }

        {
            unsigned short x = 0;
            BOOST_TEST_EQ( boost::core::rotl( x, i ), x );
            BOOST_TEST_EQ( boost::core::rotr( x, i ), x );
        }

        {
            unsigned int x = 0;
            BOOST_TEST_EQ( boost::core::rotl( x, i ), x );
            BOOST_TEST_EQ( boost::core::rotr( x, i ), x );
        }

        {
            unsigned long x = 0;
            BOOST_TEST_EQ( boost::core::rotl( x, i ), x );
            BOOST_TEST_EQ( boost::core::rotr( x, i ), x );
        }

        {
            boost::ulong_long_type x = 0;
            BOOST_TEST_EQ( boost::core::rotl( x, i ), x );
            BOOST_TEST_EQ( boost::core::rotr( x, i ), x );
        }
    }

    boost::detail::splitmix64 rng;

    for( int i = 0; i < 1000; ++i )
    {
        boost::uint64_t x = rng();

        test_rotate( static_cast<unsigned char>( x ) );
        test_rotate( static_cast<unsigned short>( x ) );
        test_rotate( static_cast<unsigned int>( x ) );
        test_rotate( static_cast<unsigned long>( x ) );
        test_rotate( static_cast<boost::ulong_long_type>( x ) );
    }

    return boost::report_errors();
}
