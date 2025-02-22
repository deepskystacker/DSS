/*
Copyright 2024 Braden Ganetsky
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#include <boost/core/empty_value.hpp>

/*
Tests workaround for MSVC bug:
https://developercommunity.visualstudio.com/t/Compiler-bug:-Incorrect-C2247-and-C2248/10690025
*/
template<class T>
struct derived
    : boost::empty_value<T> {
    typedef typename boost::empty_value<T>::type type;
    derived(boost::empty_init_t e)
        : boost::empty_value<T>(e) { }
};

struct outer {
    struct inner_empty { };
    struct inner_non_empty {
        inner_non_empty()
            : value() { }
        int value;
    };
};

void test()
{
    const boost::empty_value<outer> x1(boost::empty_init);
    (void)x1;
    const boost::empty_value<outer::inner_empty> x2(boost::empty_init);
    (void)x2;
    const boost::empty_value<outer::inner_non_empty> x3(boost::empty_init);
    (void)x3;
    const derived<outer> x4(boost::empty_init);
    (void)x4;
    const derived<outer::inner_empty> x5(boost::empty_init);
    (void)x5;
    const derived<outer::inner_non_empty> x6(boost::empty_init);
    (void)x6;
}
