// Test for yield_primitives.hpp compatibility with windows.h
//
// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
# include <windows.h>
#endif

#include <boost/core/yield_primitives.hpp>

int main()
{
    boost::core::sp_thread_pause();
    boost::core::sp_thread_yield();
    boost::core::sp_thread_sleep();
}
