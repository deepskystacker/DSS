//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/sync/spin/recursive_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "mutex_test_template.hpp"

int main ()
{
   using namespace boost::interprocess;
   //
   test::test_all_lock<ipcdetail::spin_recursive_mutex>();
   test::test_all_mutex<ipcdetail::spin_recursive_mutex>();
   test::test_all_recursive_lock<ipcdetail::spin_recursive_mutex>();

   return 0;
}
