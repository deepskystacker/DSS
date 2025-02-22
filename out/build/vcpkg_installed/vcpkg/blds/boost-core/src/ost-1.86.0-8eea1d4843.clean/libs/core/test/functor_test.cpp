/*
 *             Copyright Andrey Semashev 2024.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   functor_test.cpp
 * \author Andrey Semashev
 * \date   2024-01-23
 *
 * This file contains tests for \c boost::core::functor.
 */

#include <boost/config.hpp>

#if !defined(BOOST_NO_CXX17_AUTO_NONTYPE_TEMPLATE_PARAMS)

#include <boost/core/functor.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <type_traits>

#if (defined(__cpp_lib_is_invocable) && (__cpp_lib_is_invocable >= 201703l)) || \
    (defined(BOOST_MSSTL_VERSION) && (BOOST_MSSTL_VERSION >= 140) && (BOOST_CXX_VERSION >= 201703l))

namespace test {

using std::is_invocable;

} // namespace test

#else

namespace test {

// A simplified implementation that does not support member function pointers
template< typename Func, typename... Args >
struct is_invocable_impl
{
    template< typename F = Func, typename = decltype(std::declval< F >()(std::declval< Args >()...)) >
    static std::true_type _check_invocable(int);
    static std::false_type _check_invocable(...);

    typedef decltype(is_invocable_impl::_check_invocable(0)) type;
};

template< typename Func, typename... Args >
struct is_invocable : public is_invocable_impl< Func, Args... >::type { };

} // namespace test

#endif

int g_n = 0;

void void_func()
{
    ++g_n;
}

int int_func()
{
    return ++g_n;
}

int& int_ref_func()
{
    ++g_n;
    return g_n;
}

void void_add1(int x)
{
    g_n += x;
}

int add2(int x, int y)
{
    return x + y;
}

namespace test_ns {

int add3(int x, int y, int z)
{
    return x + y + z;
}

} // namespace test_ns

int int_func_noexcept() noexcept
{
    return ++g_n;
}

int main()
{
    {
        boost::core::functor< void_func > fun;
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< void_func >& >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< void_func > const& >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< void_func > const&, int >));
        BOOST_TEST_EQ(noexcept(fun()), false);
        fun();
        BOOST_TEST_EQ(g_n, 1);
        fun();
        BOOST_TEST_EQ(g_n, 2);
    }

    g_n = 0;
    {
        boost::core::functor< int_func > fun;
        int res = fun();
        BOOST_TEST_EQ(res, 1);
        BOOST_TEST_EQ(g_n, 1);
        res = fun();
        BOOST_TEST_EQ(res, 2);
        BOOST_TEST_EQ(g_n, 2);
    }

    g_n = 0;
    {
        boost::core::functor< int_ref_func > fun;
        int& res1 = fun();
        BOOST_TEST_EQ(&res1, &g_n);
        BOOST_TEST_EQ(res1, 1);
        int& res2 = fun();
        BOOST_TEST_EQ(&res2, &g_n);
        BOOST_TEST_EQ(res2, 2);
    }

    g_n = 0;
    {
        boost::core::functor< void_add1 > fun;
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< void_add1 >& >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< void_add1 > const& >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< void_add1 >&, int >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< void_add1 > const&, int >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< void_add1 >&, short int >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< void_add1 > const&, short int >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< void_add1 > const&, int, int >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< void_add1 > const&, const char* >));
        fun(10);
        BOOST_TEST_EQ(g_n, 10);
        fun(20);
        BOOST_TEST_EQ(g_n, 30);
    }

    {
        boost::core::functor< add2 > fun;
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< add2 >& >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< add2 > const& >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< add2 >&, int >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< add2 > const&, int >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< add2 >&, int, int >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< add2 > const&, int, int >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< add2 >&, short int, signed char >));
        BOOST_TEST_TRAIT_TRUE((test::is_invocable< boost::core::functor< add2 > const&, short int, signed char >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< add2 > const&, const char* >));
        BOOST_TEST_TRAIT_FALSE((test::is_invocable< boost::core::functor< add2 > const&, const char*, float >));
        int res = fun(10, 20);
        BOOST_TEST_EQ(res, 30);
        res = fun(30, 40);
        BOOST_TEST_EQ(res, 70);
    }

    {
        boost::core::functor< test_ns::add3 > fun;
        int res = fun(10, 20, 30);
        BOOST_TEST_EQ(res, 60);
        res = fun(40, 50, 60);
        BOOST_TEST_EQ(res, 150);
    }

    g_n = 0;
    {
        boost::core::functor< int_func_noexcept > fun;
        BOOST_TEST_EQ(noexcept(fun()), true);
        int res = fun();
        BOOST_TEST_EQ(res, 1);
        BOOST_TEST_EQ(g_n, 1);
        res = fun();
        BOOST_TEST_EQ(res, 2);
        BOOST_TEST_EQ(g_n, 2);
    }

    return boost::report_errors();
}

#else // !defined(BOOST_NO_CXX17_AUTO_NONTYPE_TEMPLATE_PARAMS)

#include <boost/config/pragma_message.hpp>

BOOST_PRAGMA_MESSAGE("Test skipped because C++17 auto non-type template parameters are not supported")

int main()
{
    return 0;
}

#endif // !defined(BOOST_NO_CXX17_AUTO_NONTYPE_TEMPLATE_PARAMS)
