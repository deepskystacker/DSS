# Copyright 2018, 2019, 2023 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(boost_system_install_test LANGUAGES CXX)

# test find_package(Boost) instead of find_package(boost_system)

find_package(Boost 1.82 REQUIRED COMPONENTS system)

add_executable(main main.cpp)
target_link_libraries(main Boost::system)

enable_testing()
add_test(NAME main COMMAND main)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)
