// Copyright 2022 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

// Should fail, because boost::report_errors() hasn't been called

int main()
{
    boost::core::lwt_init();
}
