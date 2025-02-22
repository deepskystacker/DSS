/*
 *             Copyright Andrey Semashev 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   fclose_deleter_test.cpp
 * \author Andrey Semashev
 * \date   21.09.2022
 *
 * This file contains tests for \c boost::fclose_deleter.
 */

#include <boost/core/fclose_deleter.hpp>
#include <cstdio>
#include <cstddef>
#include <boost/config.hpp>
#include <boost/move/unique_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#if !defined(BOOST_NO_CXX11_SMART_PTR)
#include <memory>
#endif

boost::movelib::unique_ptr< std::FILE, boost::fclose_deleter > make_boost_unique_file(const char* filename)
{
    return boost::movelib::unique_ptr< std::FILE, boost::fclose_deleter >(std::fopen(filename, "w"));
}

boost::shared_ptr< std::FILE > make_boost_shared_file(const char* filename)
{
    return boost::shared_ptr< std::FILE >(std::fopen(filename, "w"), boost::fclose_deleter());
}

#if !defined(BOOST_NO_CXX11_SMART_PTR)
std::unique_ptr< std::FILE, boost::fclose_deleter > make_std_unique_file(const char* filename)
{
    return std::unique_ptr< std::FILE, boost::fclose_deleter >(std::fopen(filename, "w"));
}

std::shared_ptr< std::FILE > make_std_shared_file(const char* filename)
{
    return std::shared_ptr< std::FILE >(std::fopen(filename, "w"), boost::fclose_deleter());
}
#endif

int main()
{
    const char* const filename = "fcd_test.txt";

    std::FILE* file = std::fopen(filename, "w");
    if (file)
    {
        boost::fclose_deleter()(file);
        file = NULL;
    }

    make_boost_unique_file(filename);
    make_boost_shared_file(filename);

#if !defined(BOOST_NO_CXX11_SMART_PTR)
    make_std_unique_file(filename);
    make_std_shared_file(filename);
#endif

    // Test if the deleter can be called on a NULL pointer
    boost::shared_ptr< std::FILE >(static_cast< std::FILE* >(NULL), boost::fclose_deleter());
#if !defined(BOOST_NO_CXX11_SMART_PTR)
    std::shared_ptr< std::FILE >(static_cast< std::FILE* >(NULL), boost::fclose_deleter());
#endif

    std::remove(filename);
}
