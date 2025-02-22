/*
Copyright 2020 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#include <boost/core/allocator_access.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/type_traits/is_same.hpp>
#if !defined(BOOST_NO_CXX11_ALLOCATOR)
#include <boost/type_traits/make_unsigned.hpp>
#endif

template<class T>
struct A1 {
    typedef T value_type;
    typedef int size_type;
};

#if !defined(BOOST_NO_CXX11_ALLOCATOR)
template<class T>
struct A2 {
    typedef T value_type;
};
#endif

int main()
{
    BOOST_TEST_TRAIT_TRUE((boost::is_same<int,
        boost::allocator_size_type<A1<char> >::type>));
#if !defined(BOOST_NO_CXX11_ALLOCATOR)
    BOOST_TEST_TRAIT_TRUE((boost::is_same<
        boost::make_unsigned<std::ptrdiff_t>::type,
        boost::allocator_size_type<A2<int> >::type>));
#endif
    return boost::report_errors();
}
