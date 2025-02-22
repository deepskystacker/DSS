// Test for sp_thread_sleep
//
// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/yield_primitives.hpp>

int main()
{
    for( int i = 0; i < 100; ++i )
    {
        boost::core::sp_thread_sleep();
    }
}
