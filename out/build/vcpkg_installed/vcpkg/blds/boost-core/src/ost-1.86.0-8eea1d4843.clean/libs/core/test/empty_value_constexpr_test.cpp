/*
Copyright 2022 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#include <boost/config.hpp>
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
#include <boost/core/empty_value.hpp>
#include <boost/core/lightweight_test.hpp>

struct empty {
    constexpr int value() const {
        return 1;
    }
};

class type {
public:
    explicit constexpr type(int count)
        : value_(count) { }

    constexpr int value() const {
        return value_;
    }

private:
    int value_;
};

void test_int()
{
    constexpr boost::empty_value<int> v(boost::empty_init_t(), 4);
    constexpr int c = v.get();
    BOOST_TEST_EQ(c, 4);
}

void test_empty()
{
    constexpr boost::empty_value<empty> v = boost::empty_init_t();
    constexpr int c = v.get().value();
    BOOST_TEST_EQ(c, 1);
}

void test_type()
{
    constexpr boost::empty_value<type> v(boost::empty_init_t(), 2);
    constexpr int c = v.get().value();
    BOOST_TEST_EQ(c, 2);
}

int main()
{
    test_int();
    test_empty();
    test_type();
    return boost::report_errors();
}
#else
int main()
{
    return 0;
}
#endif
