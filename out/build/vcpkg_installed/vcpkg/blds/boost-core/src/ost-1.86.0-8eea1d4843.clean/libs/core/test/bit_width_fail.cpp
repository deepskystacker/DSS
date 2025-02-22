// Negative test for boost/core/bit.hpp (bit_width)
//
// Copyright 2022 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/bit.hpp>

int main()
{
    // should fail, because 0 is a signed integral type
    return boost::core::bit_width( 0 );
}
