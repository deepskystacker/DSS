// Copyright 2020, 2021 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/assert/source_location.hpp>
#include <boost/core/lightweight_test.hpp>
#include <sstream>
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
        BOOST_TEST_EQ( loc.to_string(), std::string( "(unknown source location)" ) );

        std::ostringstream os;
        os << loc;

        BOOST_TEST_EQ( os.str(), loc.to_string() );
    }

    {
        boost::source_location loc( "file", 5, "" );
        BOOST_TEST_EQ( loc.to_string(), std::string( "file:5" ) );

        std::ostringstream os;
        os << loc;

        BOOST_TEST_EQ( os.str(), loc.to_string() );
    }

    {
        boost::source_location loc( "file", 7, "main()" );
        BOOST_TEST_EQ( loc.to_string(), std::string( "file:7 in function 'main()'" ) );

        std::ostringstream os;
        os << loc;

        BOOST_TEST_EQ( os.str(), loc.to_string() );
    }

    {
        boost::source_location loc( "file", 11, "main()", 13 );
        BOOST_TEST_EQ( loc.to_string(), std::string( "file:11:13 in function 'main()'" ) );

        std::ostringstream os;
        os << loc;

        BOOST_TEST_EQ( os.str(), loc.to_string() );
    }

    {
        boost::source_location loc( "file", 17, "", 19 );
        BOOST_TEST_EQ( loc.to_string(), std::string( "file:17:19" ) );

        std::ostringstream os;
        os << loc;

        BOOST_TEST_EQ( os.str(), loc.to_string() );
    }

    {
        boost::source_location loc = BOOST_CURRENT_LOCATION;

        std::string prefix = std::string( adjust_filename(__FILE__) ) + ":77";
        BOOST_TEST_EQ( loc.to_string().substr( 0, prefix.size() ), prefix );

        std::ostringstream os;
        os << loc;

        BOOST_TEST_EQ( os.str(), loc.to_string() );
    }

    return boost::report_errors();
}
