// Copyright (c) 2023 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Tests that boost::core::invoke_swap propagates noexcept specification correctly

#include <boost/core/invoke_swap.hpp>
#include <boost/config.hpp>

#if !defined(BOOST_NO_CXX11_NOEXCEPT) && !defined(BOOST_NO_CXX11_STATIC_ASSERT) && \
    !(defined(BOOST_GCC) && (BOOST_GCC < 40700))

namespace test_ns {

struct class_with_noexcept_swap
{
    static class_with_noexcept_swap& instance() noexcept;

    friend void swap(class_with_noexcept_swap&, class_with_noexcept_swap&) noexcept
    {
    }
};

struct class_with_except_swap
{
    static class_with_except_swap& instance() noexcept;

    friend void swap(class_with_except_swap&, class_with_except_swap&)
    {
    }
};

} // namespace test_ns

static_assert(noexcept(boost::core::invoke_swap(test_ns::class_with_noexcept_swap::instance(), test_ns::class_with_noexcept_swap::instance())),
    "boost::core::invoke_swap for class_with_noexcept_swap should have noexcept specification");
static_assert(!noexcept(boost::core::invoke_swap(test_ns::class_with_except_swap::instance(), test_ns::class_with_except_swap::instance())),
    "boost::core::invoke_swap for class_with_except_swap should not have noexcept specification");

#endif // !defined(BOOST_NO_CXX11_NOEXCEPT) && !defined(BOOST_NO_CXX11_STATIC_ASSERT) ...
