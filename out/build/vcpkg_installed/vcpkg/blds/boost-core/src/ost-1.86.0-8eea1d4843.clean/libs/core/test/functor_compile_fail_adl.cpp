/*
 *             Copyright Andrey Semashev 2024.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   functor_compile_fail_adl.cpp
 * \author Andrey Semashev
 * \date   02.02.2024
 *
 * This file tests that \c boost::core::functor doesn't bring namespace
 * \c boost::core into ADL.
 */

#include <boost/core/functor.hpp>

void func() {}

namespace boost::core {

void check_adl(functor< ::func > const&)
{
}

} // namespace boost::core

int main()
{
    // Must not find boost::check_adl
    check_adl(boost::core::functor< ::func >());
}
