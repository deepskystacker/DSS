//  (C) Copyright John Maddock 2023

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX23_HDR_SPANSTREAM
//  TITLE:         C++23 header <spanstream> unavailable
//  DESCRIPTION:   The standard library does not supply C++23 header <spanstream>

#include <spanstream>

namespace boost_no_cxx23_hdr_spanstream {

int test()
{
  using std::basic_spanbuf;
  using std::spanbuf;
  using std::wspanbuf;
  using std::basic_ispanstream;
  using std::ispanstream;
  using std::wispanstream;
  using std::basic_ospanstream;
  using std::ospanstream;
  using std::wospanstream;
  using std::basic_spanstream;
  using std::spanstream;
  using std::wspanstream;

  return 0;
}

}
