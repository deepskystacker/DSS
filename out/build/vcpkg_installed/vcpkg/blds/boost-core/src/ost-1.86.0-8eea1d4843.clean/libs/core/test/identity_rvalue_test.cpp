/*
Copyright 2021-2023 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#include <boost/config.hpp>
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#include <boost/core/identity.hpp>
#include <boost/core/lightweight_test.hpp>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

bool test(std::string&&)
{
    return true;
}

bool test(const std::string&&)
{
    return true;
}

template<class T>
bool test(T&&)
{
    return false;
}

void simple_test()
{
    typedef std::string string;
    BOOST_TEST(boost::identity()(string("a")) == string("a"));
    BOOST_TEST(test(boost::identity()(string("a"))));
    typedef const std::string cstring;
    BOOST_TEST(boost::identity()(cstring("a")) == cstring("a"));
    BOOST_TEST(test(boost::identity()(cstring("a"))));
}

void algorithm_test()
{
    std::vector<std::string> v1;
    v1.push_back(std::string("a"));
    v1.push_back(std::string("b"));
    v1.push_back(std::string("c"));
    std::vector<std::string> v2(v1);
    std::vector<std::string> v3;
    std::transform(std::make_move_iterator(v2.begin()),
        std::make_move_iterator(v2.end()),
        std::back_inserter(v3),
        boost::identity());
    BOOST_TEST(v3 == v1);
}

int main()
{
    simple_test();
    algorithm_test();
    return boost::report_errors();
}
#else
int main()
{
    return 0;
}
#endif
