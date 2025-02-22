//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2006-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/interprocess/detail/workaround.hpp>
#include "condition_test_template.hpp"

#if defined(BOOST_INTERPROCESS_WINDOWS)
#include <boost/interprocess/sync/windows/condition.hpp>
#include <boost/interprocess/sync/windows/mutex.hpp>

using namespace boost::interprocess;

int main ()
{
   if(!test::do_test_condition<ipcdetail::winapi_condition, ipcdetail::winapi_mutex>())
      return 1;
   return 0;
}

#else
int main()
{
   return 0;
}
#endif