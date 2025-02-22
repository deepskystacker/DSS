// Copyright 2019, 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/timer/timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

int main()
{
    boost::shared_ptr<boost::timer::cpu_timer> timer = boost::make_shared<boost::timer::cpu_timer>();
    timer->stop();
}
