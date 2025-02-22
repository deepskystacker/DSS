//  (C) Copyright John Maddock 2023

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_CXX23_HDR_STDFLOAT
//  TITLE:         C++23 header <stdfloat> unavailable
//  DESCRIPTION:   The standard library does not supply C++23 header <stdfloat>

#include <stdfloat>

namespace boost_no_cxx23_hdr_stdfloat {

int test()
{
#if defined(__STDCPP_FLOAT16_T__)
   using std::float16_t;
#endif
#if defined(__STDCPP_FLOAT32_T__)
   using std::float32_t;
#endif
#if defined(__STDCPP_FLOAT64_T__)
   using std::float64_t;
#endif
#if defined(__STDCPP_FLOAT128_T__)
   using std::float128_t;
#endif
#if defined(__STDCPP_BFLOAT16_T__)
   using std::bfloat16_t;
#endif  
   return 0;
}

}
