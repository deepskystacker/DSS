//  (C) Copyright John Maddock 2023

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX23_HDR_MDSPAN
//  TITLE:         C++23 header <mdspan> unavailable
//  DESCRIPTION:   The standard library does not supply C++23 header <mdspan>

#include <mdspan>

namespace boost_no_cxx23_hdr_mdspan {

int test()
{
  using std::extents;
  using std::dextents;
  using std::layout_left;
  using std::layout_right;
  using std::layout_stride;
  using std::default_accessor;
  using std::mdspan;

  return 0;
}

}
