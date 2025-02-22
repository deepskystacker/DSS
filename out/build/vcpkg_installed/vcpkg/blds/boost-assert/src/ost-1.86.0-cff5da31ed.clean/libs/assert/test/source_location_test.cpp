// Copyright 2019 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/assert/source_location.hpp>
#include <boost/core/lightweight_test.hpp>
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

int main()
{
    {
        boost::source_location loc;

        BOOST_TEST_CSTR_EQ( loc.file_name(), "" );
        BOOST_TEST_CSTR_EQ( loc.function_name(), "" );
        BOOST_TEST_EQ( loc.line(), 0 );
        BOOST_TEST_EQ( loc.column(), 0 );
    }

    {
        boost::source_location loc( __FILE__, __LINE__, "main()" );

        BOOST_TEST_CSTR_EQ( loc.file_name(), __FILE__ );
        BOOST_TEST_EQ( loc.line(), 35 );
        BOOST_TEST_CSTR_EQ( loc.function_name(), "main()" );
        BOOST_TEST_EQ( loc.column(), 0 );
    }

    {
        boost::source_location loc( "file", 1, "main()", 2 );

        BOOST_TEST_CSTR_EQ( loc.file_name(), "file" );
        BOOST_TEST_EQ( loc.line(), 1 );
        BOOST_TEST_CSTR_EQ( loc.function_name(), "main()" );
        BOOST_TEST_EQ( loc.column(), 2 );
    }

    {
        boost::source_location loc = BOOST_CURRENT_LOCATION;

        BOOST_TEST_CSTR_EQ( loc.file_name(), adjust_filename(__FILE__) );
        BOOST_TEST_EQ( loc.line(), 53 );
    }

    {
        BOOST_STATIC_CONSTEXPR boost::source_location loc = BOOST_CURRENT_LOCATION;

        BOOST_TEST_CSTR_EQ( loc.file_name(), adjust_filename(__FILE__) );
        BOOST_TEST_EQ( loc.line(), 60 );
    }

#if defined(__cpp_lib_source_location) && __cpp_lib_source_location >= 201907L

    {
        std::source_location loc = std::source_location::current();
        boost::source_location loc2 = loc;

        BOOST_TEST_CSTR_EQ( loc2.file_name(), loc.file_name() );
        BOOST_TEST_CSTR_EQ( loc2.function_name(), loc.function_name() );
        BOOST_TEST_EQ( loc2.line(), loc.line() );
        BOOST_TEST_EQ( loc2.column(), loc.column() );
    }

#endif

    return boost::report_errors();
}
