//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#include <boost/container/small_vector.hpp>
#include "vector_test.hpp"
#include "movable_int.hpp"
#include "propagate_allocator_test.hpp"
#include "default_init_test.hpp"
#include "../../intrusive/test/iterator_test.hpp"

#include <boost/container/allocator_traits.hpp>

#include <iostream>

struct boost_container_small_vector;

namespace boost { namespace container {   namespace test {

template<>
struct alloc_propagate_base<boost_container_small_vector>
{
   template <class T, class Allocator>
   struct apply
   {
      typedef boost::container::small_vector<T, 10, Allocator> type;
   };
};

}}}   //namespace boost::container::test


bool test_small_vector_shrink_to_fit()
{
   boost::container::small_vector<int, 5> sm5;
   boost::container::vector<int> v;
   sm5.push_back(1);
   sm5.push_back(2);
   sm5.push_back(3);
   sm5.push_back(4);
   sm5.push_back(5);

   v.push_back(1);
   v.push_back(2);
   v.push_back(3);
   v.push_back(4);
   v.push_back(5);

   if (!sm5.is_small())
      return false;
   if (!boost::container::algo_equal(sm5.begin(), sm5.end(), v.begin(), v.end()))
      return false;

   //Shrinking a when internal storage is used is a no-op
   sm5.shrink_to_fit();

   if (!sm5.is_small())
      return false;
   if (!boost::container::algo_equal(sm5.begin(), sm5.end(), v.begin(), v.end()))
      return false;

   //If dynamic memory is used, shrink_to_fit will move elements to the internal storage
   sm5.push_back(6);
   v.push_back(6);
   if (sm5.is_small())
      return false;
   sm5.pop_back();
   v.pop_back();
   if (sm5.is_small())
      return false;

   sm5.shrink_to_fit();
   if (!sm5.is_small())
      return false;
   if (!boost::container::algo_equal(sm5.begin(), sm5.end(), v.begin(), v.end()))
      return false;

   //If dynamic memory is used, and size is zero the dynamic storage is deallocated
   sm5.push_back(6);
   v.push_back(6);
   if (sm5.is_small())
      return false;
   sm5.clear();
   v.clear();
   if (sm5.is_small())
      return false;

   sm5.shrink_to_fit();
   if (!sm5.is_small())
      return false;
   if (!boost::container::algo_equal(sm5.begin(), sm5.end(), v.begin(), v.end()))
      return false;

   return true;
}

bool test_small_vector_base_test()
{
   typedef boost::container::small_vector_base<int> smb_t;
   {
      typedef boost::container::small_vector<int, 5> sm5_t;
      BOOST_CONTAINER_STATIC_ASSERT(sm5_t::static_capacity == 5);
      sm5_t sm5;
      smb_t &smb = sm5;
      smb.push_back(1);
      sm5_t sm5_copy(sm5);
      sm5_copy.push_back(1);
      if (!boost::container::test::CheckEqualContainers(sm5, smb))
         return false;
   }
   {
      typedef boost::container::small_vector<int, 7> sm7_t;
      BOOST_CONTAINER_STATIC_ASSERT(sm7_t::static_capacity == 7);
      sm7_t sm7;
      smb_t &smb = sm7;
      smb.push_back(2);
      sm7_t sm7_copy(sm7);
      sm7_copy.push_back(2);
      if (!boost::container::test::CheckEqualContainers(sm7, smb))
         return false;
   }
   {
      typedef boost::container::small_vector<int, 5> sm5_t;
      sm5_t sm5;
      smb_t &smb = sm5;
      smb.push_back(1);
      sm5_t sm5_copy(smb);
      if (!boost::container::test::CheckEqualContainers(sm5, sm5_copy))
         return false;
      smb.push_back(2);
      if(smb.size() != 2){
         return false;
      }
      sm5_copy = smb;
      if (!boost::container::test::CheckEqualContainers(sm5, sm5_copy))
         return false;
      sm5_t sm5_move(boost::move(smb));
      smb.clear();
      if (!boost::container::test::CheckEqualContainers(sm5_move, sm5_copy))
         return false;
      smb = sm5_copy;
      sm5_move = boost::move(smb);
      smb.clear();
      if (!boost::container::test::CheckEqualContainers(sm5_move, sm5_copy))
         return false;
   }
   {
      typedef boost::container::small_vector<int, 5> sm5_t;
      typedef boost::container::small_vector<int, 7> sm7_t;

      {
         //Both in static memory
         sm5_t sm5;
         sm5.push_back(1);

         sm7_t sm7;
         sm7.push_back(2);

         sm5_t sm5_copy(sm5);
         sm7_t sm7_copy(sm7);
         smb_t& smb5 = sm5_copy;
         smb_t& smb7 = sm7_copy;
         if (!sm5.is_small() || !sm7.is_small())
            return false;

         smb5.swap(smb7);
         if (!boost::container::test::CheckEqualContainers(sm5_copy, sm7))
            return false;
         if (!boost::container::test::CheckEqualContainers(sm7_copy, sm5))
            return false;

         if (!sm5.is_small() || !sm7.is_small())
            return false;

         smb5.swap(smb7);
         if (!boost::container::test::CheckEqualContainers(sm7_copy, sm7))
            return false;
         if (!boost::container::test::CheckEqualContainers(sm5_copy, sm5))
            return false;

         if (!sm5.is_small() || !sm7.is_small())
            return false;
      }
      {
         //Both in dynamic memory
         sm5_t sm5;
         for(std::size_t i = 0, max = sm5.capacity()+1; i != max; ++i){
            sm5.push_back(int(i));
         }

         sm7_t sm7;
         for(std::size_t i = 0, max = sm7.capacity()+1; i != max; ++i){
            sm7.push_back(int(i));
         }

         sm5_t sm5_copy(sm5);
         sm7_t sm7_copy(sm7);
         smb_t& smb5 = sm5_copy;
         smb_t& smb7 = sm7_copy;

         if (smb5.is_small() || smb7.is_small())
            return false;

         smb5.swap(smb7);
         if (!boost::container::test::CheckEqualContainers(sm5_copy, sm7))
            return false;
         if (!boost::container::test::CheckEqualContainers(sm7_copy, sm5))
            return false;

         if (smb5.is_small() || smb7.is_small())
            return false;

         smb5.swap(smb7);
         if (!boost::container::test::CheckEqualContainers(sm7_copy, sm7))
            return false;
         if (!boost::container::test::CheckEqualContainers(sm5_copy, sm5))
            return false;

         if (smb5.is_small() || smb7.is_small())
            return false;
      }
      {
         //sm7 in dynamic memory
         sm5_t sm5;
         for(std::size_t i = 0, max = sm5.capacity()-1; i != max; ++i){
            sm5.push_back(int(i));
         }

         sm7_t sm7;
         for(std::size_t i = 0, max = sm7.capacity()+1; i != max; ++i){
            sm7.push_back(int(i));
         }

         sm5_t sm5_copy(sm5);
         sm7_t sm7_copy(sm7);
         smb_t& smb5 = sm5_copy;
         smb_t& smb7 = sm7_copy;

         if (!smb5.is_small() || smb7.is_small())
            return false;

         //As small_vector_base is capacity-erased, will make an element-wise swap,
         //remaining smb7 elements won't fit in smb5's internal buffer so both will be dynamic
         smb5.swap(smb7);
         if (!boost::container::test::CheckEqualContainers(sm5_copy, sm7))
            return false;
         if (!boost::container::test::CheckEqualContainers(sm7_copy, sm5))
            return false;

         if (smb5.is_small() || smb7.is_small())
            return false;

         //Swap them again (both dynamic)
         smb5.swap(smb7);
         if (!boost::container::test::CheckEqualContainers(sm7_copy, sm7))
            return false;
         if (!boost::container::test::CheckEqualContainers(sm5_copy, sm5))
            return false;

         if (smb5.is_small() || smb7.is_small())
            return false;

         //Try again with one dynamic, but the reverse option
         //shrink to fit should free the dynamic buffer after clear
         //and both should be small again using the internal buffer
         sm5_copy.clear();
         sm5_copy.shrink_to_fit();
         sm7_copy.clear();
         sm7_copy.shrink_to_fit();
      
         if (!smb5.is_small() || !smb7.is_small())
            return false;

         sm5_copy = sm5;
         sm7_copy = sm7;

         if (!smb5.is_small() || smb7.is_small())
            return false;
      }
   }

   return true;
}

//small vector has internal storage so some special swap cases must be tested
bool test_swap()
{
   typedef boost::container::small_vector<int, 10> vec;
   {  //v bigger than static capacity, w empty
      vec v;
      for(std::size_t i = 0, max = v.capacity()+1; i != max; ++i){
         v.push_back(int(i));
      }
      vec w;
      const std::size_t v_size = v.size();
      const std::size_t w_size = w.size();
      v.swap(w);
      if(v.size() != w_size || w.size() != v_size)
         return false;
   }
   {  //v smaller than static capacity, w empty
      vec v;
      for(std::size_t i = 0, max = v.capacity()-1; i != max; ++i){
         v.push_back(int(i));
      }
      vec w;
      const std::size_t v_size = v.size();
      const std::size_t w_size = w.size();
      v.swap(w);
      if(v.size() != w_size || w.size() != v_size)
         return false;
   }
   {  //v bigger than static capacity, w enough capacity for static
      vec v;
      for (std::size_t i = 0, max = v.capacity() + 1; i != max; ++i) {
         v.push_back(int(i));
      }
      vec w;
      for (std::size_t i = 0, max = w.capacity() / 2; i != max; ++i) {
         w.push_back(int(i));
      }
      const std::size_t v_size = v.size();
      const std::size_t w_size = w.size();
      v.swap(w);
      if (v.size() != w_size || w.size() != v_size)
         return false;
   }
   {  //v & w smaller than static capacity
      vec v;
      for(std::size_t i = 0, max = v.capacity()-1; i != max; ++i){
         v.push_back(int(i));
      }
      vec w;
      for(std::size_t i = 0, max = w.capacity()/2; i != max; ++i){
         w.push_back(int(i));
      }
      const std::size_t v_size = v.size();
      const std::size_t w_size = w.size();
      v.swap(w);
      if(v.size() != w_size || w.size() != v_size)
         return false;
   }
   {  //v & w bigger than static capacity
      vec v;
      for(std::size_t i = 0, max = v.capacity()+1; i != max; ++i){
         v.push_back(int(i));
      }
      vec w;
      for(std::size_t i = 0, max = w.capacity()*2; i != max; ++i){
         w.push_back(int(i));
      }
      const std::size_t v_size = v.size();
      const std::size_t w_size = w.size();
      v.swap(w);
      if(v.size() != w_size || w.size() != v_size)
         return false;
   }

   //Now test internal buffer/dynamic buffer swapping
   {
      typedef boost::container::small_vector<int, 5> sm5_t;

      {
         sm5_t sm5;
         for (std::size_t i = 0, max = sm5.capacity() - 1; i != max; ++i) {
            sm5.push_back(int(i));
         }

         sm5_t sm5_copy(sm5);
         {
            sm5_t sm5_dyn(sm5);
            sm5_dyn.resize(sm5_dyn.capacity() + 1u);
            sm5_dyn.resize(sm5.size());

            if (sm5_dyn != sm5 || sm5_dyn.is_small())
               return false;

            //Swap derived small vector one static one dynamic
            sm5_copy.swap(sm5_dyn);

            if (sm5_dyn != sm5)
               return false;

            //Dynamic buffer should be transferred, the old dynamic should be small now
            if (sm5_copy.is_small() || !sm5_dyn.is_small())
               return false;

            //Swap derived small vector one static one dynamic
            sm5_copy.swap(sm5_dyn);

            if (sm5_dyn != sm5)
               return false;

            //Dynamic buffer should be transferred, the old dynamic should be small now
            if (!sm5_copy.is_small() || sm5_dyn.is_small())
               return false;
         }
         {
            sm5_t sm5_int(sm5);
            if (sm5_int != sm5 || !sm5_int.is_small())
               return false;

            //Swap derived small vector one static one dynamic
            sm5_copy.swap(sm5_int);

            if (sm5_int != sm5)
               return false;

            //No dynamic memory should be present as small capacity is enough
            if (!sm5_copy.is_small() || !sm5_int.is_small())
               return false;

            //Swap derived small vector one static one dynamic
            sm5_copy.swap(sm5_int);

            if (sm5_int != sm5)
               return false;

            //No dynamic memory should be present as small capacity is enough
            if (!sm5_copy.is_small() || !sm5_int.is_small())
               return false;
         }
      }
   }
   return true;
}

template<class VoidAllocator>
struct GetAllocatorCont
{
   template<class ValueType>
   struct apply
   {
      typedef boost::container::small_vector< ValueType, 10
         , typename boost::container::allocator_traits<VoidAllocator>
            ::template portable_rebind_alloc<ValueType>::type
      > type;
   };
};

template<class VoidAllocator>
int test_cont_variants()
{
   using namespace boost::container;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<int>::type MyCont;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<test::movable_int>::type MyMoveCont;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<test::movable_and_copyable_int>::type MyCopyMoveCont;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<test::copyable_int>::type MyCopyCont;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<test::moveconstruct_int>::type MyMoveConstructCont;

   if (test::vector_test<MyCont>())
      return 1;
   if (test::vector_test<MyMoveCont>())
      return 1;
   if (test::vector_test<MyCopyMoveCont>())
      return 1;
   if (test::vector_test<MyCopyCont>())
      return 1;
   if (test::vector_test<MyMoveConstructCont>())
      return 1;

   return 0;
}

int main()
{
   using namespace boost::container;

   if(!test_swap())
      return 1;

   if (test::vector_test< small_vector<int, 0> >())
      return 1;

   if (test::vector_test< small_vector<int, 2000> >())
      return 1;

   if (test_cont_variants< new_allocator<void> >())
      return 1;

   ////////////////////////////////////
   //    Default init test
   ////////////////////////////////////
   if(!test::default_init_test< small_vector<int, 5, test::default_init_allocator<int> > >()){
      std::cerr << "Default init test failed" << std::endl;
      return 1;
   }

   ////////////////////////////////////
   //    Emplace testing
   ////////////////////////////////////
   const test::EmplaceOptions Options = (test::EmplaceOptions)(test::EMPLACE_BACK | test::EMPLACE_BEFORE);
   if(!boost::container::test::test_emplace< small_vector<test::EmplaceInt, 5>, Options>()){
      return 1;
   }

   ////////////////////////////////////
   //    Allocator propagation testing
   ////////////////////////////////////
   if(!boost::container::test::test_propagate_allocator<boost_container_small_vector>()){
      return 1;
   }

   ////////////////////////////////////
   //    Initializer lists testing
   ////////////////////////////////////
   if(!boost::container::test::test_vector_methods_with_initializer_list_as_argument_for
      < boost::container::small_vector<int, 5> >()) {
      return 1;
   }

   ////////////////////////////////////
   //       Small vector base
   ////////////////////////////////////
   if (!test_small_vector_shrink_to_fit()){
      return 1;
   }

   ////////////////////////////////////
   //    Iterator testing
   ////////////////////////////////////
   {
      typedef boost::container::small_vector<int, 10> cont_int;
      for (std::size_t i = 10; i <= 10000; i *= 10) {
         cont_int a;
         for (int j = 0; j < (int)i; ++j)
            a.push_back((int)j);
         boost::intrusive::test::test_iterator_random< cont_int >(a);
         if (boost::report_errors() != 0) {
            return 1;
         }
      }
   }

   ////////////////////////////////////
   //    has_trivial_destructor_after_move testing
   ////////////////////////////////////
   // default allocator
   {
      typedef boost::container::small_vector<int, 0> cont;
      if (boost::has_trivial_destructor_after_move<cont>::value) {
         std::cerr << "has_trivial_destructor_after_move(default allocator) test failed" << std::endl;
         return 1;
      }
   }
   // std::allocator
   {
      typedef boost::container::small_vector<int, 0, std::allocator<int> > cont;
      if (boost::has_trivial_destructor_after_move<cont>::value) {
         std::cerr << "has_trivial_destructor_after_move(std::allocator) test failed" << std::endl;
         return 1;
      }
   }

   return 0;
}
