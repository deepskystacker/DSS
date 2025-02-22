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
#if defined(BOOST_INTERPROCESS_WINDOWS)
#include <boost/interprocess/sync/windows/recursive_mutex.hpp>
#include "mutex_test_template.hpp"

int main ()
{
   using namespace boost::interprocess;
   //
   test::test_all_lock<ipcdetail::winapi_recursive_mutex>();
   test::test_all_mutex<ipcdetail::winapi_recursive_mutex>();
   test::test_all_recursive_lock<ipcdetail::winapi_recursive_mutex>();

   return 0;
}
#else

int main()
{
   return 0;
}

#endif
