/*
Copyright 2023 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#include <boost/config.hpp>
#if !defined(BOOST_NO_CXX11_CONSTEXPR) && !defined(BOOST_NO_CXX11_DECLTYPE)
#include <boost/core/size.hpp>
#include <boost/core/lightweight_test.hpp>

struct range {
    std::size_t size() const {
        return 4;
    }
};

void test_range()
{
    range c;
    BOOST_TEST_EQ(boost::size(c), 4);
}

void test_array()
{
    int a[4];
    BOOST_TEST_EQ(boost::size(a), 4);
}

int main()
{
    test_range();
    test_array();
    return boost::report_errors();
}
#else
int main()
{
    return 0;
}
#endif
