//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2022-2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
//[doc_custom_devector
#include <boost/container/devector.hpp>

//Make sure assertions are active
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

int main ()
{
   using namespace boost::container;

   ////////////////////////////////////////////////
   //          'stored_size' option
   ////////////////////////////////////////////////
   //Specify that a devector will use "unsigned char" as the type to store size/capacity
   typedef devector_options< stored_size<unsigned char> >::type size_option_t;

   //Size-optimized devector is smaller than the default one.
   typedef devector<int, new_allocator<int>, size_option_t > size_optimized_devector_t;
   assert(( sizeof(size_optimized_devector_t) < sizeof(devector<int>) ));

   //Requesting capacity for more elements than representable by "unsigned char" is an error
   bool exception_thrown = false;
   /*<-*/ 
   #ifndef BOOST_NO_EXCEPTIONS
   BOOST_CONTAINER_TRY{ size_optimized_devector_t v(256); } BOOST_CONTAINER_CATCH(...){ exception_thrown = true; } BOOST_CONTAINER_CATCH_END
   #else
   exception_thrown = true;
   #endif   //BOOST_NO_EXCEPTIONS
   /*->*/
   //=try       { size_optimized_devector_t v(256); }
   //=catch(...){ exception_thrown = true;        }
   assert(exception_thrown == true);

   ////////////////////////////////////////////////
   //          'growth_factor' option
   ////////////////////////////////////////////////
   //Specify that a devector will increase its capacity 50% when reallocating
   typedef devector_options< growth_factor<growth_factor_50> >::type growth_50_option_t;

   //Fill the devector until full capacity is reached
   devector<int, new_allocator<int>, growth_50_option_t > growth_50_dv(5, 0);
   std::size_t old_cap = growth_50_dv.capacity();
   growth_50_dv.resize(old_cap);

   //Now insert an additional item and check the new buffer is 50% bigger
   growth_50_dv.push_back(1);
   assert(growth_50_dv.capacity() == old_cap*3/2);

   ////////////////////////////////////////////////
   //          'relocate_on' option
   ////////////////////////////////////////////////

   //Specifies that a devector will not reallocate but relocate elements if the free space
   //at one end is exhausted and the total load factor is below the 66% threshold.
   typedef devector_options< relocate_on_66 >::type relocation_66_option_t;

   //Configure devector to have equal free space at both ends
   devector<int, new_allocator<int>, relocation_66_option_t > reloc_66_dv
      (16u, 16u, reserve_only_tag_t());
   old_cap = reloc_66_dv.capacity();
   const std::size_t front_free_cap = reloc_66_dv.front_free_capacity();

   //Fill vector at the back end
   while (reloc_66_dv.back_free_capacity() > 0)
      reloc_66_dv.push_back(0);

   //Front free capacity is intact
   assert(reloc_66_dv.front_free_capacity() == front_free_cap);

   //Now insert new element, values should relocated to the middle as the
   //load factor is near 50%
   reloc_66_dv.push_back(0);
   assert(reloc_66_dv.capacity() == old_cap);
   assert(reloc_66_dv.front_free_capacity() < front_free_cap);

   //Fill the back end again
   while (reloc_66_dv.back_free_capacity() > 0)
      reloc_66_dv.push_back(1);

   //New insertion should reallocate as load factor is higher than 66%
   reloc_66_dv.push_back(-1);
   assert(reloc_66_dv.capacity() > old_cap);

   return 0;
}
//]
