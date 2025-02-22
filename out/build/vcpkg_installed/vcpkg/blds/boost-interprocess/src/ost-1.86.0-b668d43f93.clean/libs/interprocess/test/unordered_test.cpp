//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//boost::unordered is supported only in C++11 and newer
#include <boost/config.hpp>

#if BOOST_CXX_VERSION >=201103L

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include "get_process_id_name.hpp"

//<-
//Shield against external warnings
#include <boost/interprocess/detail/config_external_begin.hpp>
//->

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

//<-
#include <boost/interprocess/detail/config_external_end.hpp>
//->

#include <functional> //std::equal_to
#include <boost/container_hash/hash.hpp> //boost::hash

namespace bip = boost::interprocess;

typedef bip::allocator<int, bip::managed_shared_memory::segment_manager> ShmemAllocator;
typedef boost::unordered_set<int, boost::hash<int>, std::equal_to<int>, ShmemAllocator> MyUnorderedSet;
typedef boost::unordered_multiset<int, boost::hash<int>, std::equal_to<int>, ShmemAllocator> MyUnorderedMultiSet;

int main()
{
   //Remove any other old shared memory from the system
   bip::shared_memory_object::remove(bip::test::get_process_id_name());
   BOOST_TRY {
      bip::managed_shared_memory shm(bip::create_only, bip::test::get_process_id_name(), 65536);

      //Elements to be inserted in unordered containers
      const int elements[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
      const std::size_t elements_size = sizeof(elements)/sizeof(elements[0]);

      MyUnorderedSet *myset  =
         shm.construct<MyUnorderedSet>(bip::anonymous_instance)
            ( elements_size
            , MyUnorderedSet::hasher()
            , MyUnorderedSet::key_equal()
            , shm.get_allocator<int>());
      MyUnorderedMultiSet *mymset =
         shm.construct<MyUnorderedMultiSet>(bip::anonymous_instance)
            ( elements_size
            , MyUnorderedSet::hasher()
            , MyUnorderedSet::key_equal()
            , shm.get_allocator<int>());

      //Insert elements and check sizes
      myset->insert((&elements[0]), (&elements[std::ptrdiff_t(elements_size)]));
      myset->insert((&elements[0]), (&elements[std::ptrdiff_t(elements_size)]));
      mymset->insert((&elements[0]), (&elements[std::ptrdiff_t(elements_size)]));
      mymset->insert((&elements[0]), (&elements[std::ptrdiff_t(elements_size)]));

      if(myset->size() != elements_size)
         return 1;
      if(mymset->size() != elements_size*2u)
         return 1;

      //Destroy elements and check sizes
      myset->clear();
      mymset->clear();

      if(!myset->empty())
         return 1;
      if(!mymset->empty())
         return 1;

      //Destroy elements and check if memory has been deallocated
      shm.destroy_ptr(myset);
      shm.destroy_ptr(mymset);

      shm.shrink_to_fit_indexes();
      if(!shm.all_memory_deallocated())
         return 1;

   }
   BOOST_CATCH(...){
      //Remove shared memory from the system
      bip::shared_memory_object::remove(bip::test::get_process_id_name());
      BOOST_RETHROW
   } BOOST_CATCH_END
   //Remove shared memory from the system
   bip::shared_memory_object::remove(bip::test::get_process_id_name());
   return 0;
}

#else
int main()
{
   return 0;
}
#endif //#if BOOST_CXX_VERSION >=201103L
