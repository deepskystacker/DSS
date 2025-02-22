//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2024-2024. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_TEST_NAMED_SEMAPHORE_HELPERS_HEADER
#define BOOST_INTERPROCESS_TEST_NAMED_SEMAPHORE_HELPERS_HEADER

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include <boost/interprocess/detail/interprocess_tester.hpp>
#include <boost/interprocess/exceptions.hpp>
#include "named_creation_template.hpp"
#include "mutex_test_template.hpp"
#include "get_process_id_name.hpp"
#include <exception>
#include <boost/interprocess/timed_utils.hpp>

#include <exception>

namespace boost { namespace interprocess { namespace test {

static const std::size_t RecSemCount   = 100;

//This wrapper is necessary to plug this class
//in lock tests
template<class NamedSemaphore>
class lock_test_wrapper
   : public NamedSemaphore
{
   public:

   template <class CharT>
   lock_test_wrapper(create_only_t, const CharT *name, unsigned int count = 1)
      :  NamedSemaphore(create_only, name, count)
   {}

   template <class CharT>
   lock_test_wrapper(open_only_t, const CharT *name)
      :  NamedSemaphore(open_only, name)
   {}

   template <class CharT>
   lock_test_wrapper(open_or_create_t, const CharT *name, unsigned int count = 1)
      :  NamedSemaphore(open_or_create, name, count)
   {}

   ~lock_test_wrapper()
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
};

//This wrapper is necessary to plug this class
//in recursive tests
template<class NamedSemaphore>
class recursive_test_wrapper
   :  public lock_test_wrapper<NamedSemaphore>
{
   public:
   recursive_test_wrapper(create_only_t, const char *name)
      :  lock_test_wrapper<NamedSemaphore>(create_only, name, RecSemCount)
   {}

   recursive_test_wrapper(open_only_t, const char *name)
      :  lock_test_wrapper<NamedSemaphore>(open_only, name)
   {}

   recursive_test_wrapper(open_or_create_t, const char *name)
      :  lock_test_wrapper<NamedSemaphore>(open_or_create, name, RecSemCount)
   {}
};

template<class NamedSemaphore>
bool test_named_semaphore_specific()
{
   NamedSemaphore::remove(test::get_process_id_name());
   //Test persistance
   {
      NamedSemaphore sem(create_only, test::get_process_id_name(), 3);
   }
   {
      NamedSemaphore sem(open_only, test::get_process_id_name());
      BOOST_INTERPROCESS_CHECK(sem.try_wait() == true);
      BOOST_INTERPROCESS_CHECK(sem.try_wait() == true);
      BOOST_INTERPROCESS_CHECK(sem.try_wait() == true);
      BOOST_INTERPROCESS_CHECK(sem.try_wait() == false);
      sem.post();
   }
   {
      NamedSemaphore sem(open_only, test::get_process_id_name());
      BOOST_INTERPROCESS_CHECK(sem.try_wait() == true);
      BOOST_INTERPROCESS_CHECK(sem.try_wait() == false);
   }

   NamedSemaphore::remove(test::get_process_id_name());
   return true;
}

template<class NamedSemaphore>
int test_named_semaphore()
{
   int ret = 0;
   BOOST_TRY{
      test::test_named_creation< test::named_sync_creation_test_wrapper<lock_test_wrapper<NamedSemaphore> > >();
      #if defined(BOOST_INTERPROCESS_WCHAR_NAMED_RESOURCES)
      test::test_named_creation< test::named_sync_creation_test_wrapper_w<lock_test_wrapper<NamedSemaphore> > >();
      #endif //defined(BOOST_INTERPROCESS_WCHAR_NAMED_RESOURCES)

      test::test_all_lock< test::named_sync_wrapper<lock_test_wrapper<NamedSemaphore> > >();
      test::test_all_mutex<test::named_sync_wrapper<lock_test_wrapper<NamedSemaphore> > >();
      test::test_all_recursive_lock<test::named_sync_wrapper<recursive_test_wrapper<NamedSemaphore> > >();
      test_named_semaphore_specific<NamedSemaphore>();
   }
   BOOST_CATCH(std::exception &ex){
      std::cout << ex.what() << std::endl;
      ret = 1;
   } BOOST_CATCH_END
   NamedSemaphore::remove(test::get_process_id_name());
   return ret;
}

}}}   //namespace boost { namespace interprocess { namespace test {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_TEST_NAMED_SEMAPHORE_HELPERS_HEADER
