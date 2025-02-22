// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/launder.hpp>
#include <boost/core/lightweight_test.hpp>
#include <new>

struct X
{
    int v_;

    explicit X( int v = 0 ): v_( v ) {}
};

int main()
{
    X x;

    typedef X const CX;

    ::new( &x ) CX( 1 );
    X const* px1 = &x;

    BOOST_TEST_EQ( px1->v_, 1 );

    ::new( &x ) CX( 2 );
    X const* px2 = boost::core::launder( px1 );

    BOOST_TEST_EQ( px1, px2 );
    BOOST_TEST_EQ( px2->v_, 2 );

    return boost::report_errors();
}
