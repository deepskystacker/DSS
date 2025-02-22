//  Copyright 2023 Andrey Semashev
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/is_swappable.hpp>
#endif

#include <boost/config.hpp>

#include "test.hpp"
#include "check_integral_constant.hpp"
#include <utility>

// These conditions should be similar to those in is_swappable.hpp
#if defined(BOOST_DINKUMWARE_STDLIB) && (BOOST_CXX_VERSION < 201703L) && \
    !defined(BOOST_NO_SFINAE_EXPR) && !defined(BOOST_NO_CXX11_DECLTYPE) && !defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) && !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && \
    !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) && !BOOST_WORKAROUND(BOOST_MSVC, < 1800) // these are required for is_constructible and is_assignable
#define BOOST_TYPE_TRAITS_IS_SWAPPABLE_MSVC_EMULATED
#elif defined(BOOST_NO_SFINAE_EXPR) || defined(BOOST_NO_CXX11_DECLTYPE) || defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS)
#define BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED
#endif

struct X
{
};

struct Y
{
    Y( Y const& ) {}
};

struct Z
{
    Z& operator=( Z const& ) { return *this; }
};

struct U
{
};

void swap(U&, U&) {}

#if !defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED) && !defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)

struct not_swappable
{
    not_swappable(not_swappable const&) = delete;
    not_swappable& operator= (not_swappable const&) = delete;
};

namespace test_ns {

// Not swappable using std::swap, but swappable using test_ns::swap
struct only_adl_swappable
{
    only_adl_swappable(only_adl_swappable const&) = delete;
    only_adl_swappable& operator= (only_adl_swappable const&) = delete;
};

inline void swap(only_adl_swappable&, only_adl_swappable&) BOOST_NOEXCEPT {}

} // namespace test_ns

#endif

TT_TEST_BEGIN(is_swappable)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int const volatile>::value, false);

#if defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int const volatile[2]>::value, false);
#else
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int[2]>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int volatile[2]>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<int const volatile[2]>::value, false);
#endif

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<void const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<void volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<void const volatile>::value, false);

#if defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const volatile[2]>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const volatile[2]>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const volatile[2]>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> >::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> const>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> volatile>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> const volatile>::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> >::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> const>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> volatile>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> const volatile>::value), false);

#else // defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X[2]>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<X const volatile[2]>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y[2]>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Y const volatile[2]>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z[2]>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<Z const volatile[2]>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<U const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> const>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> volatile>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<X, int> const volatile>::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> const>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> volatile>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_swappable<std::pair<Y, int> const volatile>::value), false);

#endif // defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED)

#if !defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED) && !defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<not_swappable const volatile[2]>::value, false);

#if !defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_MSVC_EMULATED)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable[2]>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const volatile[2]>::value, false);

#else // !defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_MSVC_EMULATED)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable volatile[2]>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_swappable<test_ns::only_adl_swappable const volatile[2]>::value, false);

#endif // !defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_MSVC_EMULATED)

#endif // !defined(BOOST_TYPE_TRAITS_IS_SWAPPABLE_CXX03_EMULATED) && !defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)

TT_TEST_END
