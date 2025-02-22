// Copyright 2024 Braden Ganetsky
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/empty_value.hpp>

struct empty {};

// This test ensures private inheritance of `boost::empty_value<T>` for empty `T`.
// With public inheritance, `boost::empty_value<empty>*` could cast to `empty*`.
void test_empty_not_convertible_to_base()
{
    const boost::empty_value<empty> x(boost::empty_init);
    const empty* x2 = static_cast<const empty*>(&x);
    (void)x2;
}

int main()
{
}
