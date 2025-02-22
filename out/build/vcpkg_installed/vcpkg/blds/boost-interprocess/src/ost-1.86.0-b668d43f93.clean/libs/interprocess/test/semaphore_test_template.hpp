//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.

#ifndef BOOST_INTERPROCESS_TEST_SEMAPHORE_TEST_TEMPLATE_HEADER
#define BOOST_INTERPROCESS_TEST_SEMAPHORE_TEST_TEMPLATE_HEADER

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/timed_utils.hpp>
#include "named_creation_template.hpp"
#include "mutex_test_template.hpp"

#include <boost/interprocess/exceptions.hpp>
#include "named_creation_template.hpp"
#include "mutex_test_template.hpp"

namespace boost { namespace interprocess { namespace test {

static const std::size_t SemCount      = 1;
static const std::size_t RecSemCount   = 100;

//This wrapper is necessary to plug this class
//in named creation tests and interprocess_mutex tests
template<class Semaphore>
class semaphore_test_wrapper
   : public Semaphore
{
   public:
   semaphore_test_wrapper()
      : Semaphore(SemCount)
   {}

   void lock()
   {  this->wait();  }

   bool try_lock()
   {  return this->try_wait();  }

   template<class TimePoint>
   bool timed_lock(const TimePoint &pt)
   {  return this->timed_wait(pt);  }

   template<class TimePoint> bool try_lock_until(const TimePoint &abs_time)
   {  return this->timed_lock(abs_time);  }

   template<class Duration>  bool try_lock_for(const Duration &dur)
   {  return this->timed_lock(boost::interprocess::ipcdetail::duration_to_ustime(dur)); }

   void unlock()
   {  this->post();  }

   protected:
   semaphore_test_wrapper(std::size_t initial_count)
     : Semaphore(unsigned(initial_count))
   {}
};

//This wrapper is necessary to plug this class
//in recursive tests
template<class Semaphore>
class recursive_semaphore_test_wrapper
   :  public semaphore_test_wrapper<Semaphore>
{
   public:
   recursive_semaphore_test_wrapper()
      :  semaphore_test_wrapper<Semaphore>(RecSemCount)
   {}
};

template <class Semaphore>
inline int test_all_semaphore()
{
   using namespace boost::interprocess;

   test::test_all_lock<semaphore_test_wrapper<Semaphore> >();
   test::test_all_recursive_lock<recursive_semaphore_test_wrapper<Semaphore> >();
   test::test_all_mutex<semaphore_test_wrapper<Semaphore> >();
   return 0;
}

}}}   //namespace boost { namespace interprocess { namespace test {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_TEST_SEMAPHORE_TEST_TEMPLATE_HEADER
