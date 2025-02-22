/*
 * Copyright 2024 Andrey Semashev
 *
 * Distributed under Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

 // MACRO: BOOST_NO_CXX17_AUTO_NONTYPE_TEMPLATE_PARAMS
 // TITLE: C++17 auto non-type template parameters
 // DESCRIPTION: C++17 auto non-type template parameters are not supported.

namespace boost_no_cxx17_auto_nontype_template_params {

template< auto Value >
struct foo
{
    static auto get()
    {
        return Value;
    }
};

const int ten = 10;

int test()
{
    return foo< 10 >::get() - *foo< &ten >::get();
}

} // boost_no_cxx17_auto_nontype_template_params
