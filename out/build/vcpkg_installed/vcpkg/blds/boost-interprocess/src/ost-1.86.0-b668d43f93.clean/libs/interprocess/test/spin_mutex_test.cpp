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
#include "mutex_test_template.hpp"

#include <boost/interprocess/sync/spin/mutex.hpp>

int main ()
{
   using namespace boost::interprocess;

   test::test_all_lock<ipcdetail::spin_mutex>();
   test::test_all_mutex<ipcdetail::spin_mutex>();
   return 0;
}
