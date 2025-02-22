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

#ifndef BOOST_INTERPROCESS_TEST_UTIL_HEADER
#define BOOST_INTERPROCESS_TEST_UTIL_HEADER

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/interprocess/timed_utils.hpp>

#if defined(BOOST_CLANG) || (defined(BOOST_GCC) && (BOOST_GCC >= 40600))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#  if (BOOST_GCC >= 100000)
#pragma GCC diagnostic ignored "-Warith-conversion"
#  endif
#endif

#if BOOST_CXX_VERSION >= 201103L
#define BOOST_CHRONO_HEADER_ONLY
#include <boost/chrono/system_clocks.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#endif

#include <boost/version.hpp>

#if !defined(BOOST_NO_CXX11_HDR_CHRONO)
#include <chrono>
#endif

#if defined(BOOST_GCC) && (BOOST_GCC >= 40600)
#pragma GCC diagnostic pop
#endif

namespace boost {
namespace interprocess {
namespace test {

// ptime_delay_ms + ptime_ms

#if BOOST_CXX_VERSION >= 201103L
inline boost::posix_time::ptime ptime_delay_ms(unsigned msecs)
{
   using namespace boost::posix_time;
   int count = static_cast<int>(double(msecs) *
      (double(time_duration::ticks_per_second()) / double(1000.0)));
   return microsec_clock::universal_time() + time_duration(0, 0, 0, count);
}

inline boost::posix_time::time_duration ptime_ms(unsigned msecs)
{
   using namespace boost::posix_time;
   int count = static_cast<int>(double(msecs) *
      (double(time_duration::ticks_per_second()) / double(1000.0)));
   return time_duration(0, 0, 0, count);
}
#else
   inline ustime ptime_delay_ms(unsigned msecs)
   {  return ustime_delay_milliseconds(msecs); }

   inline usduration ptime_ms(unsigned msecs)
   {  return usduration_from_milliseconds(msecs); }
#endif

// boost_systemclock_delay_ms + boost_systemclock_ms

#if BOOST_CXX_VERSION >= 201103L
   inline boost::chrono::system_clock::time_point boost_systemclock_delay_ms(unsigned msecs)
   {  return boost::chrono::system_clock::now() + boost::chrono::milliseconds(msecs);  }

   inline boost::chrono::milliseconds boost_systemclock_ms(unsigned msecs)
   {  return boost::chrono::milliseconds(msecs);  }
#else
   inline ustime boost_systemclock_delay_ms(unsigned msecs)
   {  return ustime_delay_milliseconds(msecs); }

   inline usduration boost_systemclock_ms(unsigned msecs)
   {  return usduration_from_milliseconds(msecs); }
#endif 

// std_systemclock_delay_ms + std_systemclock_ms

#if !defined(BOOST_NO_CXX11_HDR_CHRONO)
   //Use std chrono if available
   inline std::chrono::system_clock::time_point std_systemclock_delay_ms(unsigned msecs)
   {  return std::chrono::system_clock::now() + std::chrono::milliseconds(msecs);  }

   inline std::chrono::milliseconds std_systemclock_ms(unsigned msecs)
   {  return std::chrono::milliseconds(msecs);  }

#elif BOOST_CXX_VERSION >= 201103L
   //Otherwise use boost chrono
   inline boost::chrono::system_clock::time_point std_systemclock_delay_ms(unsigned msecs)
   {  return boost_systemclock_delay_ms(msecs);  }

   inline boost::chrono::milliseconds std_systemclock_ms(unsigned msecs)
   {  return boost_systemclock_ms(msecs);  }
#else
   inline ustime std_systemclock_delay_ms(unsigned msecs)
   {  return ustime_delay_milliseconds(msecs); }

   inline usduration std_systemclock_ms(unsigned msecs)
   {  return usduration_from_milliseconds(msecs); }
#endif

// thread_adapter + data

template <typename P>
class thread_adapter
{
   public:
   thread_adapter(void (*func)(void*, P &), void* param1, P &param2)
      : func_(func), param1_(param1) ,param2_(param2){ }
   void operator()() const { func_(param1_, param2_); }

   private:
   void (*func_)(void*, P &);
   void* param1_;
   P& param2_;
};

template <typename P>
struct data
{
   explicit data(int id, int msecs=0, int flags = 0, bool block = false)
      : m_id(id), m_value(-1), m_msecs(msecs), m_error(no_error), m_flags(flags), m_block(block)
   {}

   int            m_id;
   int            m_value;
   int            m_msecs;
   error_code_t   m_error;
   int            m_flags;
   bool           m_block;
};

int shared_val = 0;
static const unsigned BaseMs = 1000;

}  //namespace test {
}  //namespace interprocess {
}  //namespace boost {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_TEST_UTIL_HEADER
