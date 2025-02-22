/*
Copyright 2021-2023 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#include <boost/core/identity.hpp>
#include <boost/core/lightweight_test.hpp>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

bool test(std::string&)
{
    return true;
}

bool test(const std::string&)
{
    return true;
}

template<class T>
bool test(T&)
{
    return false;
}

template<class T>
bool test(const T&)
{
    return false;
}

void simple_test()
{
    std::string s1("a");
    BOOST_TEST(boost::identity()(s1) == s1);
    BOOST_TEST(&boost::identity()(s1) == &s1);
    BOOST_TEST(test(boost::identity()(s1)));
    const std::string s2("a");
    BOOST_TEST(boost::identity()(s2) == s2);
    BOOST_TEST(&boost::identity()(s2) == &s2);
    BOOST_TEST(test(boost::identity()(s2)));
}

void algorithm_test()
{
    std::vector<std::string> v1;
    v1.push_back(std::string("a"));
    v1.push_back(std::string("b"));
    v1.push_back(std::string("c"));
    std::vector<std::string> v2;
    std::transform(v1.begin(), v1.end(), std::back_inserter(v2),
         boost::identity());
    BOOST_TEST(v2 == v1);
}

int main()
{
    simple_test();
    algorithm_test();
    return boost::report_errors();
}
