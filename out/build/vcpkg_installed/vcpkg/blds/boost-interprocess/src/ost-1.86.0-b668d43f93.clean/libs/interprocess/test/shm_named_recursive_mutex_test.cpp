//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include "named_mutex_test_helpers.hpp"
#include <boost/interprocess/sync/shm/named_recursive_mutex.hpp>

using namespace boost::interprocess;

int main()
{
   return test::test_named_recursive_mutex<ipcdetail::shm_named_recursive_mutex>();
}
