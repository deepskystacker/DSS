// Test for boost/core/detail/minstd_rand.hpp
//
// Copyright 2022, 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/detail/minstd_rand.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/cstdint.hpp>

int main()
{
    {
        boost::detail::minstd_rand rng;

        boost::uint_least32_t r1 = rng(), x1 = 48271;
        BOOST_TEST_EQ( r1, x1 );

        for( int i = 0; i < 1000; ++i ) rng();

        boost::uint_least32_t r2 = rng(), x2 = 2076422031;
        BOOST_TEST_EQ( r2, x2 );
    }

    {
        boost::detail::minstd_rand rng( 12345 );

        boost::uint_least32_t r1 = rng(), x1 = 595905495;
        BOOST_TEST_EQ( r1, x1 );

        for( int i = 0; i < 1000; ++i ) rng();

        boost::uint_least32_t r2 = rng(), x2 = 1065162103;
        BOOST_TEST_EQ( r2, x2 );
    }

    return boost::report_errors();
}
