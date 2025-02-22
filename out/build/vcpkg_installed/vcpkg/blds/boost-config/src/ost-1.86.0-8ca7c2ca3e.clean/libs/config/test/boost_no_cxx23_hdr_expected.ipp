//  (C) Copyright John Maddock 2023

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX23_HDR_EXPECTED
//  TITLE:         C++23 header <expected> unavailable
//  DESCRIPTION:   The standard library does not supply C++23 header <expected>

#include <expected>

namespace boost_no_cxx23_hdr_expected {

int test()
{
  using std::unexpected;
  using std::bad_expected_access;
  using std::unexpect_t;
  using std::unexpect;
  using std::expected;

  return 0;
}

}
