// Copyright 2018 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/throw_exception.hpp>
#include <boost/exception/get_error_info.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <cstring>

class my_exception: public std::exception
{
};

class my_exception2: public std::exception, public boost::exception
{
};

class my_exception3: public std::exception, public virtual boost::exception
{
};

char const* translate_function( char const * fn, char const * cfn )
{
    // fn comes from BOOST_CURRENT_LOCATION, which is not necessarily the same as BOOST_CURRENT_FUNCTION
    // so translate the known problematic cases to BOOST_CURRENT_FUNCTION to make the test pass
    return fn[0] == 0 || std::strcmp( fn, "main" ) == 0 || std::strcmp( fn, "int __cdecl main(void)" ) == 0? cfn: fn;
}

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
    try
    {
        BOOST_THROW_EXCEPTION( my_exception() );
    }
    catch( boost::exception const & x )
    {
        {
            char const * const * file = boost::get_error_info<boost::throw_file>( x );

            BOOST_TEST( file != 0 );
            BOOST_TEST_CSTR_EQ( *file, adjust_filename(__FILE__) );
        }

        {
            int const * line = boost::get_error_info<boost::throw_line>( x );

            BOOST_TEST( line != 0 );
            BOOST_TEST_EQ( *line, 50 );
        }

        {
            char const * const * function = boost::get_error_info<boost::throw_function>( x );

            BOOST_TEST( function != 0 );
            BOOST_TEST_CSTR_EQ( translate_function( *function, BOOST_CURRENT_FUNCTION ), BOOST_CURRENT_FUNCTION );
        }
    }

    try
    {
        BOOST_THROW_EXCEPTION( my_exception2() );
    }
    catch( boost::exception const & x )
    {
        {
            char const * const * file = boost::get_error_info<boost::throw_file>( x );

            BOOST_TEST( file != 0 );
            BOOST_TEST_CSTR_EQ( *file, adjust_filename(__FILE__) );
        }

        {
            int const * line = boost::get_error_info<boost::throw_line>( x );

            BOOST_TEST( line != 0 );
            BOOST_TEST_EQ( *line, 78 );
        }

        {
            char const * const * function = boost::get_error_info<boost::throw_function>( x );

            BOOST_TEST( function != 0 );
            BOOST_TEST_CSTR_EQ( translate_function( *function, BOOST_CURRENT_FUNCTION ), BOOST_CURRENT_FUNCTION );
        }
    }

    try
    {
        BOOST_THROW_EXCEPTION( my_exception3() );
    }
    catch( boost::exception const & x )
    {
        {
            char const * const * file = boost::get_error_info<boost::throw_file>( x );

            BOOST_TEST( file != 0 );
            BOOST_TEST_CSTR_EQ( *file, adjust_filename(__FILE__) );
        }

        {
            int const * line = boost::get_error_info<boost::throw_line>( x );

            BOOST_TEST( line != 0 );
            BOOST_TEST_EQ( *line, 106 );
        }

        {
            char const * const * function = boost::get_error_info<boost::throw_function>( x );

            BOOST_TEST( function != 0 );
            BOOST_TEST_CSTR_EQ( translate_function( *function, BOOST_CURRENT_FUNCTION ), BOOST_CURRENT_FUNCTION );
        }
    }

    return boost::report_errors();
}
