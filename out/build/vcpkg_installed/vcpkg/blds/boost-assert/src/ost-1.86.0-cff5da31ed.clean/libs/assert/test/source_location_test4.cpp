// Copyright 2022 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/assert/source_location.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/config.hpp>
#include <cstring>

static char const* adjust_filename( char const* file )
{
#if defined(__INTEL_LLVM_COMPILER) && __INTEL_LLVM_COMPILER >= 20210300

    char const* fn = std::strrchr( file, '/' );
    return fn? fn + 1: file;

#else

    return file;

#endif
}

boost::source_location s_loc = BOOST_CURRENT_LOCATION;

BOOST_STATIC_CONSTEXPR boost::source_location c_loc = BOOST_CURRENT_LOCATION;

boost::source_location f( boost::source_location const& loc = BOOST_CURRENT_LOCATION )
{
    return loc;
}

int main()
{
    {
        BOOST_TEST_CSTR_EQ( s_loc.file_name(), adjust_filename(__FILE__) );
        BOOST_TEST_EQ( s_loc.line(), 24 );

#if defined(BOOST_GCC) && BOOST_GCC < 90000
        // '__static_initialization_and_destruction_0'
#else
        BOOST_TEST_CSTR_EQ( s_loc.function_name(), "" );
#endif
    }

    {
        BOOST_TEST_CSTR_EQ( c_loc.file_name(), adjust_filename(__FILE__) );
        BOOST_TEST_EQ( c_loc.line(), 26 );
    }

    {
        boost::source_location loc = f();

        BOOST_TEST_CSTR_EQ( loc.file_name(), adjust_filename(__FILE__) );
        BOOST_TEST( loc.line() == 28 || loc.line() == 52 );
    }

    return boost::report_errors();
}
