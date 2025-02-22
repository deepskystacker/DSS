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
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTERPROCESS_CONDITION_TEST_TEMPLATE_HPP
#define BOOST_INTERPROCESS_CONDITION_TEST_TEMPLATE_HPP

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include "boost_interprocess_check.hpp"
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "util.hpp"
#include <iostream>
#include <typeinfo>

namespace boost{
namespace interprocess{
namespace test {

template <typename F, typename T>
class binder
{
public:
    binder(const F& f, const T& p)
        : func(f), param(p) { }
    void operator()() const { func(param); }

private:
    F func;
    T param;
};

template <typename F, typename T>
binder<F, T> bind_function(F func, T param)
{
    return binder<F, T>(func, param);
}

template <class Condition, class Mutex>
struct condition_test_data
{
   condition_test_data() : notified(0), awoken(0) { }

   ~condition_test_data()
   {}

   Mutex      mutex;
   Condition  condition;
   int notified;
   int awoken;
};

template <class Condition, class Mutex>
void condition_test_thread(condition_test_data<Condition, Mutex>* data)
{
    boost::interprocess::scoped_lock<Mutex>
      lock(data->mutex);
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    while (!(data->notified > 0))
        data->condition.wait(lock);
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    data->awoken++;
}

struct cond_predicate
{
    cond_predicate(int& var, int val) : _var(var), _val(val) { }

    bool operator()() { return _var == _val; }

    int& _var;
    int _val;
};



template <class Condition, class Mutex>
void do_test_condition_notify_one()
{
   condition_test_data<Condition, Mutex> data;

   boost::interprocess::ipcdetail::OS_thread_t thread;
   boost::interprocess::ipcdetail::thread_launch(thread, bind_function(&condition_test_thread<Condition, Mutex>, &data));
   //Make sure thread is blocked
   boost::interprocess::ipcdetail::thread_sleep_ms(1*BaseMs);
   {
      boost::interprocess::scoped_lock<Mutex>
         lock(data.mutex);
      BOOST_INTERPROCESS_CHECK(lock ? true : false);
      data.notified++;
      data.condition.notify_one();
   }

   boost::interprocess::ipcdetail::thread_join(thread);
   BOOST_INTERPROCESS_CHECK(data.awoken == 1);
}

template <class Condition, class Mutex>
void do_test_condition_notify_all()
{
   const int NUMTHREADS = 3;

   boost::interprocess::ipcdetail::OS_thread_t thgroup[std::size_t(NUMTHREADS)];
   condition_test_data<Condition, Mutex> data;

   for(int i = 0; i< NUMTHREADS; ++i){
      boost::interprocess::ipcdetail::thread_launch(thgroup[i], bind_function(&condition_test_thread<Condition, Mutex>, &data));
   }

   //Make sure all threads are blocked
   boost::interprocess::ipcdetail::thread_sleep_ms(1*BaseMs);
   {
      boost::interprocess::scoped_lock<Mutex>
         lock(data.mutex);
      BOOST_INTERPROCESS_CHECK(lock ? true : false);
      data.notified++;
   }
   data.condition.notify_all();

   for(int i = 0; i< NUMTHREADS; ++i){
      boost::interprocess::ipcdetail::thread_join(thgroup[i]);
   }
   BOOST_INTERPROCESS_CHECK(data.awoken == NUMTHREADS);
}

template <class Condition, class Mutex>
void do_test_condition_waits_step( condition_test_data<Condition, Mutex> &data
                                 , boost::interprocess::scoped_lock<Mutex> &lock
                                 , int awoken)
{
      boost::interprocess::ipcdetail::thread_sleep_ms(1*BaseMs);
      data.notified++;
      data.condition.notify_one();
      while (data.awoken != awoken)
         data.condition.wait(lock);
      BOOST_INTERPROCESS_CHECK(lock ? true : false);
      BOOST_INTERPROCESS_CHECK(data.awoken == awoken);
}

template <class Condition, class Mutex>
void condition_test_waits(condition_test_data<Condition, Mutex>* data)
{
    boost::interprocess::scoped_lock<Mutex>
      lock(data->mutex);
    BOOST_INTERPROCESS_CHECK(lock ? true : false);

    // Test wait.
    while (data->notified != 1)
        data->condition.wait(lock);
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 1);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate wait.
    data->condition.wait(lock, cond_predicate(data->notified, 2));
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 2);
    data->awoken++;
    data->condition.notify_one();

    // Test timed_wait
    while (data->notified != 3)
        data->condition.timed_wait(lock, ptime_delay_ms(5*BaseMs));
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 3);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate timed_wait.
   {
    bool ret = data->condition.timed_wait(lock, boost_systemclock_delay_ms(5*BaseMs), cond_predicate (data->notified, 4));
    BOOST_INTERPROCESS_CHECK(ret);(void)ret;
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 4);
    data->awoken++;
    data->condition.notify_one();
   }

    // Test timed_wait
    while (data->notified != 5)
        data->condition.timed_wait(lock, std_systemclock_delay_ms(5*BaseMs));
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 5);
    data->awoken++;
    data->condition.notify_one();

    // Test wait_until
    while (data->notified != 6)
        data->condition.wait_until(lock, ptime_delay_ms(5*BaseMs));
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 6);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate wait_until.
   {
    bool ret = data->condition.wait_until(lock, boost_systemclock_delay_ms(5*BaseMs), cond_predicate (data->notified, 7));
    BOOST_INTERPROCESS_CHECK(ret);(void)ret;
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 7);
    data->awoken++;
    data->condition.notify_one();
   }

   // Test wait_for
    while (data->notified != 8)
        data->condition.wait_for(lock, ptime_ms(5*BaseMs));
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 8);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate wait_for.
   {
    bool ret = data->condition.wait_for(lock, ptime_ms(5*BaseMs), cond_predicate (data->notified, 9));
    BOOST_INTERPROCESS_CHECK(ret);(void)ret;
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 9);
    data->awoken++;
    data->condition.notify_one();
   }

   // Test wait_for
    while (data->notified != 10)
        data->condition.wait_for(lock, boost_systemclock_ms(5*BaseMs));
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 10);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate wait_for.
   {
    bool ret = data->condition.wait_for(lock, boost_systemclock_ms(5*BaseMs), cond_predicate (data->notified, 11));
    BOOST_INTERPROCESS_CHECK(ret);(void)ret;
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 11);
    data->awoken++;
    data->condition.notify_one();
   }

   // Test wait_for
    while (data->notified != 12)
        data->condition.wait_for(lock, std_systemclock_ms(5*BaseMs));
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 12);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate wait_for.
   {
    bool ret = data->condition.wait_for(lock, std_systemclock_ms(5*BaseMs), cond_predicate (data->notified, 13));
    BOOST_INTERPROCESS_CHECK(ret);(void)ret;
    BOOST_INTERPROCESS_CHECK(lock ? true : false);
    BOOST_INTERPROCESS_CHECK(data->notified == 13);
    data->awoken++;
    data->condition.notify_one();
   }
}

template <class Condition, class Mutex>
void do_test_condition_waits()
{
   condition_test_data<Condition, Mutex> data;
   boost::interprocess::ipcdetail::OS_thread_t thread;
   boost::interprocess::ipcdetail::thread_launch(thread, bind_function(&condition_test_waits<Condition, Mutex>, &data));

   {
      boost::interprocess::scoped_lock<Mutex>
         lock(data.mutex);
      BOOST_INTERPROCESS_CHECK(lock ? true : false);

      for(int i = 1; i <= 13; ++i)
         do_test_condition_waits_step(data, lock, i);
   }

   boost::interprocess::ipcdetail::thread_join(thread);
   BOOST_INTERPROCESS_CHECK(data.awoken == 13);
}

/*
//Message queue simulation test
template <class Condition>
inline Condition &cond_empty()
{
   static Condition cond_empty;
   return cond_empty;
}

template <class Condition>
inline Condition &cond_full()
{
   static Condition cond_full;
   return cond_full;
}


template <class Mutex>
inline Mutex &mutex()
{
   static Mutex mut;
   return mut;
}
*/
static int count = 0;
static int waiting_readers = 0;
static int waiting_writer  = 0;
const int queue_size    = 3;
const int thread_factor = 10;
const int NumThreads    = thread_factor*queue_size;

//Function that removes items from queue
template <class Condition, class Mutex>
struct condition_func
{
   condition_func(Condition &cond_full, Condition &cond_empty, Mutex &mutex)
      :  cond_full_(cond_full), cond_empty_(cond_empty), mutex_(mutex)
   {}

   void operator()()
   {
      boost::interprocess::scoped_lock<Mutex>lock(mutex_);
      while(count == 0){
         ++waiting_readers;
         cond_empty_.wait(lock);
         --waiting_readers;
      }
      --count;
      if(waiting_writer)
         cond_full_.notify_one();
   }
   Condition &cond_full_;
   Condition &cond_empty_;
   Mutex     &mutex_;
};

//Queue functions
template <class Condition, class Mutex>
void do_test_condition_queue_notify_one(void)
{
   //Force mutex and condition creation
   Condition cond_empty;
   Condition cond_full;
   Mutex mutex;

   //Create threads that will decrease count
   {
      //Initialize counters
      count = 0;
      waiting_readers = 0;
      waiting_writer  = 0;

      boost::interprocess::ipcdetail::OS_thread_t thgroup[std::size_t(NumThreads)];
      for(int i = 0; i< NumThreads; ++i){
         condition_func<Condition, Mutex> func(cond_full, cond_empty, mutex);
         boost::interprocess::ipcdetail::thread_launch(thgroup[i], func);
      }

      //Add 20 elements one by one in the queue simulation
      //The sender will block if it fills the queue
      for(int i = 0; i < NumThreads; ++i){
         boost::interprocess::scoped_lock<Mutex> lock(mutex);
         while(count == queue_size){
            ++waiting_writer;
            cond_full.wait(lock);
            --waiting_writer;
         }
         count++;

         if(waiting_readers)
            cond_empty.notify_one();
      }
      for(int i = 0; i< NumThreads; ++i){
         boost::interprocess::ipcdetail::thread_join(thgroup[i]);
      }
      BOOST_INTERPROCESS_CHECK(count == 0);
      BOOST_INTERPROCESS_CHECK(waiting_readers == 0);
      BOOST_INTERPROCESS_CHECK(waiting_writer  == 0);
   }
}

//Queue functions
template <class Condition, class Mutex>
void do_test_condition_queue_notify_all(void)
{
   //Force mutex and condition creation
   Condition cond_empty;
   Condition cond_full;
   Mutex mutex;

   //Create threads that will decrease count
   {
      //Initialize counters
      count = 0;
      waiting_readers = 0;
      waiting_writer  = 0;

      boost::interprocess::ipcdetail::OS_thread_t thgroup[std::size_t(NumThreads)];
      for(int i = 0; i< NumThreads; ++i){
         condition_func<Condition, Mutex> func(cond_full, cond_empty, mutex);
         boost::interprocess::ipcdetail::thread_launch(thgroup[i], func);
      }

      //Fill queue to the max size and notify all several times
      for(int i = 0; i < NumThreads; ++i){
         boost::interprocess::scoped_lock<Mutex>lock(mutex);
         while(count == queue_size){
            ++waiting_writer;
            cond_full.wait(lock);
            --waiting_writer;
         }
         count++;

         if(waiting_readers)
            cond_empty.notify_all();
      }
      for(int i = 0; i< NumThreads; ++i){
         boost::interprocess::ipcdetail::thread_join(thgroup[i]);
      }
      BOOST_INTERPROCESS_CHECK(count == 0);
      BOOST_INTERPROCESS_CHECK(waiting_readers == 0);
      BOOST_INTERPROCESS_CHECK(waiting_writer  == 0);
   }
}

template <class Condition, class Mutex>
bool do_test_condition()
{
   std::cout << "do_test_condition_notify_one<" << typeid(Condition).name() << "," << typeid(Mutex).name() << '\n' << std::endl;
   do_test_condition_notify_one<Condition, Mutex>();
   std::cout << "do_test_condition_notify_all<" << typeid(Condition).name() << "," << typeid(Mutex).name() << '\n' << std::endl;
   do_test_condition_notify_all<Condition, Mutex>();
   std::cout << "do_test_condition_waits<" << typeid(Condition).name() << "," << typeid(Mutex).name() << '\n' << std::endl;
   do_test_condition_waits<Condition, Mutex>();
   std::cout << "do_test_condition_queue_notify_one<" << typeid(Condition).name() << "," << typeid(Mutex).name() << '\n' << std::endl;
   do_test_condition_queue_notify_one<Condition, Mutex>();
   std::cout << "do_test_condition_queue_notify_all<" << typeid(Condition).name() << "," << typeid(Mutex).name() << '\n' << std::endl;
   do_test_condition_queue_notify_all<Condition, Mutex>();
   return true;
}

}  //namespace test
}  //namespace interprocess{
}  //namespace boost{

#include <boost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_CONDITION_TEST_TEMPLATE_HPP
