# Copyright 2023 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

# Boost.MySQL requires OpenSSL to work

cmake_minimum_required(VERSION 3.5...3.16)

project(boost_mysql_install_test LANGUAGES CXX)

find_package(boost_mysql REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main Boost::mysql)

enable_testing()
add_test(NAME main COMMAND main)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)
