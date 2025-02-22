/*
 *             Copyright Andrey Semashev 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   snprintf_test.cpp
 * \author Andrey Semashev
 * \date   06.12.2022
 *
 * This file contains tests for \c boost::core::snprintf.
 */

#include <boost/core/snprintf.hpp>
#include <cstddef>
#include <cstring>
#include <boost/core/lightweight_test.hpp>

void test_snprintf()
{
    char buf[11];
    std::memset(buf, 0xFF, sizeof(buf));

    std::size_t buf_size = sizeof(buf) - 1u;

    int res = boost::core::snprintf(buf, buf_size, "%s", "0123");
    BOOST_TEST_EQ(res, 4);
    int cmp_res = std::memcmp(buf, "0123", sizeof("0123"));
    BOOST_TEST_EQ(cmp_res, 0);

    std::memset(buf, 0xFF, sizeof(buf));

    // Suppress compiler checks for buffer overflow
    const char* volatile str = "0123456789";

    res = boost::core::snprintf(buf, buf_size, "%s", str);
    BOOST_TEST_GE(res, 10);
    cmp_res = std::memcmp(buf, "012345678", sizeof("012345678"));
    BOOST_TEST_EQ(cmp_res, 0);
    BOOST_TEST_EQ(buf[10], static_cast< char >(~static_cast< char >(0)));

    std::memset(buf, 0xFF, sizeof(buf));

    res = boost::core::snprintf(buf, 0, "%s", str);
    BOOST_TEST_GE(res, 0);
    BOOST_TEST_EQ(buf[0], static_cast< char >(~static_cast< char >(0)));
}

void test_swprintf()
{
    wchar_t buf[11];
    std::memset(buf, 0xFF, sizeof(buf));

    std::size_t buf_size = sizeof(buf) / sizeof(*buf) - 1u;

    int res = boost::core::swprintf(buf, buf_size, L"%ls", L"0123");
    BOOST_TEST_EQ(res, 4);
    int cmp_res = std::memcmp(buf, L"0123", sizeof(L"0123"));
    BOOST_TEST_EQ(cmp_res, 0);

    std::memset(buf, 0xFF, sizeof(buf));

    // Suppress compiler checks for buffer overflow
    const wchar_t* volatile str = L"0123456789";

    res = boost::core::swprintf(buf, buf_size, L"%ls", str);
    BOOST_TEST_LT(res, 0);
    // swprintf may or may not write to the buffer in case of overflow.
    // E.g. glibc 2.35 doesn't and libc on MacOS 11 does.
    BOOST_TEST_EQ(buf[10], static_cast< wchar_t >(~static_cast< wchar_t >(0)));

    std::memset(buf, 0xFF, sizeof(buf));

    res = boost::core::swprintf(buf, 0, L"%ls", str);
    BOOST_TEST_LT(res, 0);
    BOOST_TEST_EQ(buf[0], static_cast< wchar_t >(~static_cast< wchar_t >(0)));
}

int main()
{
    test_snprintf();
    test_swprintf();

    return boost::report_errors();
}
