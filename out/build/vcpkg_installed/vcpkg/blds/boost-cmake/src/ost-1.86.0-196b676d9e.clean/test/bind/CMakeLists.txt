# Copyright 2018, 2019, 2023 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(boost_bind_install_test LANGUAGES CXX)

# Test use of Boost::headers instead of Boost::bind

find_package(boost_headers REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main Boost::headers)

enable_testing()
add_test(NAME main COMMAND main)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)
