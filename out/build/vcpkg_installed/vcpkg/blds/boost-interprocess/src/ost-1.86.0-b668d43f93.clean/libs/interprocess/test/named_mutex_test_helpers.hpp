//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2024-2024. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_TEST_NAMED_MUTEX_HELPERS_HEADER
#define BOOST_INTERPROCESS_TEST_NAMED_MUTEX_HELPERS_HEADER

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include "mutex_test_template.hpp"
#include "named_creation_template.hpp"
#include "get_process_id_name.hpp"
#include <exception>

namespace boost { namespace interprocess { namespace test {

template<class NamedMutex>
int test_named_mutex()
{
   int ret = 0;
   BOOST_TRY{
      NamedMutex::remove(test::get_process_id_name());
      test::test_named_creation< test::named_sync_creation_test_wrapper<NamedMutex> >();
      #if defined(BOOST_INTERPROCESS_WCHAR_NAMED_RESOURCES)
      test::test_named_creation< test::named_sync_creation_test_wrapper_w<NamedMutex> >();
      #endif   //defined(BOOST_INTERPROCESS_WCHAR_NAMED_RESOURCES)
      test::test_all_lock< test::named_sync_wrapper<NamedMutex> >();
      test::test_all_mutex<test::named_sync_wrapper<NamedMutex> >();
   }
   BOOST_CATCH(std::exception& ex) {
      std::cout << ex.what() << std::endl;
      ret = 1;
   } BOOST_CATCH_END
   NamedMutex::remove(test::get_process_id_name());
   return ret;
}

template<class NamedRecursiveMutex>
int test_named_recursive_mutex()
{
   int ret = 0;
   BOOST_TRY{
      NamedRecursiveMutex::remove(test::get_process_id_name());
      test::test_named_creation< test::named_sync_creation_test_wrapper<NamedRecursiveMutex> >();
      #if defined(BOOST_INTERPROCESS_WCHAR_NAMED_RESOURCES)
      test::test_named_creation< test::named_sync_creation_test_wrapper_w<NamedRecursiveMutex> >();
      #endif   //defined(BOOST_INTERPROCESS_WCHAR_NAMED_RESOURCES)
      test::test_all_lock< test::named_sync_wrapper<NamedRecursiveMutex> >();
      test::test_all_mutex<test::named_sync_wrapper<NamedRecursiveMutex> >();
      test::test_all_recursive_lock<test::named_sync_wrapper<NamedRecursiveMutex> >();
   }
      BOOST_CATCH(std::exception& ex) {
      std::cout << ex.what() << std::endl;
      ret = 1;
   } BOOST_CATCH_END
      NamedRecursiveMutex::remove(test::get_process_id_name());
   return ret;
}



}}}   //namespace boost { namespace interprocess { namespace test {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_TEST_NAMED_MUTEX_HELPERS_HEADER
