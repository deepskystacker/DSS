//  This file was automatically generated on Mon Jan 22 16:16:53 2024
//  by libs/config/tools/generate.cpp
//  Copyright John Maddock 2002-21.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for the most recent version.//
//  Revision $Id$
//


// Test file for macro BOOST_NO_CXX17_AUTO_NONTYPE_TEMPLATE_PARAMS
// This file should not compile, if it does then
// BOOST_NO_CXX17_AUTO_NONTYPE_TEMPLATE_PARAMS should not be defined.
// See file boost_no_cxx17_auto_nontype_template_params.ipp for details

// Must not have BOOST_ASSERT_CONFIG set; it defeats
// the objective of this file:
#ifdef BOOST_ASSERT_CONFIG
#  undef BOOST_ASSERT_CONFIG
#endif

#include <boost/config.hpp>
#include "test.hpp"

#ifdef BOOST_NO_CXX17_AUTO_NONTYPE_TEMPLATE_PARAMS
#include "boost_no_cxx17_auto_nontype_template_params.ipp"
#else
#error "this file should not compile"
#endif

int main( int, char *[] )
{
   return boost_no_cxx17_auto_nontype_template_params::test();
}

