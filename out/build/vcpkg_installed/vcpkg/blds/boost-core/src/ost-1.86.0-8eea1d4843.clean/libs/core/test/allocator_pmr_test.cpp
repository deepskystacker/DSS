//
// Testing stdlib polymorphic allocators
//
// Copyright 2024 Braden Ganetsky
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/core/allocator_access.hpp>
#include <boost/config.hpp>

#ifndef BOOST_NO_CXX17_HDR_MEMORY_RESOURCE
#include <memory_resource>

void pmr_allocator_destroy_compiles(std::pmr::polymorphic_allocator<int>& alloc, int* p)
{
    boost::allocator_destroy(alloc, p);
}

#endif // !defined(BOOST_NO_CXX17_HDR_MEMORY_RESOURCE)
