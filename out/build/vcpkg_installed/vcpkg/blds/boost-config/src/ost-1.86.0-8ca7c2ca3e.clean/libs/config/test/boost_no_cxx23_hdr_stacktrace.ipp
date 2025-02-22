//  (C) Copyright John Maddock 2023

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX23_HDR_STACKTRACE
//  TITLE:         C++23 header <stacktrace> unavailable
//  DESCRIPTION:   The standard library does not supply C++23 header <stacktrace>

#include <stacktrace>

namespace boost_no_cxx23_hdr_stacktrace {

int test()
{
  using std::stacktrace_entry;
  using std::basic_stacktrace;
  using std::stacktrace;

  return 0;
}

}
