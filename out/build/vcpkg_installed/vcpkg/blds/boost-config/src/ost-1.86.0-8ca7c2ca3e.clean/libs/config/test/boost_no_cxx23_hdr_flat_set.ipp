//  (C) Copyright John Maddock 2023

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX23_HDR_FLAT_SET
//  TITLE:         C++23 header <flat_set> unavailable
//  DESCRIPTION:   The standard library does not supply C++23 header <flat_set>

#include <flat_set>

namespace boost_no_cxx23_hdr_flat_set {

int test()
{
  using std::flat_set;
  using std::sorted_unique_t;
  using std::sorted_unique;
  using std::erase_if;
  using std::uses_allocator;
  using std::flat_multiset;
  using std::sorted_equivalent_t;
  using std::sorted_equivalent;

  return 0;
}

}
