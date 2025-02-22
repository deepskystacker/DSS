//  (C) Copyright John Maddock 2023

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX23_HDR_PRINT
//  TITLE:         C++23 header <print> unavailable
//  DESCRIPTION:   The standard library does not supply C++23 header <print>

#include <print>

namespace boost_no_cxx23_hdr_print {

int test()
{
  using std::print;
  using std::println;
  using std::vprint_unicode;
  using std::vprint_nonunicode;

  return 0;
}

}
