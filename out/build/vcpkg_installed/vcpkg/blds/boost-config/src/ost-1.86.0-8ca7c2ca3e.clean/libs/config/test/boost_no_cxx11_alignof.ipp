//  Copyright Peter Dimov 2022

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX11_ALIGNOF
//  TITLE:         C++11 alignof keyword.
//  DESCRIPTION:   The compiler does not support the C++11 alignof keyword.

namespace boost_no_cxx11_alignof {

int test()
{
    return alignof(int) == alignof(int(&)[2])? 0: 1;
}

}
