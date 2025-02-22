// Copyright 2019, 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#undef NDEBUG

#include <boost/endian/arithmetic.hpp>
#include <cassert>

#define BOOST_TEST_EQ(x, y) assert((x) == (y))

int main()
{
    using namespace boost::endian;

    {
        little_uint32_t v( 0x01020304 );

        BOOST_TEST_EQ( v.data()[ 0 ], 0x04 );
        BOOST_TEST_EQ( v.data()[ 1 ], 0x03 );
        BOOST_TEST_EQ( v.data()[ 2 ], 0x02 );
        BOOST_TEST_EQ( v.data()[ 3 ], 0x01 );
    }

    {
        big_uint32_t v( 0x01020304 );

        BOOST_TEST_EQ( v.data()[ 0 ], 0x01 );
        BOOST_TEST_EQ( v.data()[ 1 ], 0x02 );
        BOOST_TEST_EQ( v.data()[ 2 ], 0x03 );
        BOOST_TEST_EQ( v.data()[ 3 ], 0x04 );
    }
}
