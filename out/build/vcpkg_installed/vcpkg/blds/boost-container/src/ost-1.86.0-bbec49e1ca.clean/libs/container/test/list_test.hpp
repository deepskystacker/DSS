////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
////////////////////////////////////////

#ifndef BOOST_CONTAINER_TEST_LIST_TEST_HEADER
#define BOOST_CONTAINER_TEST_LIST_TEST_HEADER

#include <boost/container/detail/config_begin.hpp>
#include <boost/container/detail/iterator.hpp>
#include "check_equal_containers.hpp"
#include "print_container.hpp"
#include "input_from_forward_iterator.hpp"
#include <boost/move/utility_core.hpp>
#include <boost/move/iterator.hpp>
#include <boost/move/make_unique.hpp>

#include <list>
#include <functional>   //std::greater
#include <cstddef>   //std::size_t

namespace boost{
namespace container {
namespace test{

template<class V1, class V2>
bool list_copyable_only(V1 &, V2 &, boost::container::dtl::false_type)
{
   return true;
}

//Function to check if both sets are equal
template<class V1, class V2>
bool list_copyable_only(V1 &boostlist, V2 &stdlist, boost::container::dtl::true_type)
{
   typedef typename V1::value_type IntType;
   boostlist.insert(boostlist.end(), 50u, IntType(1));
   stdlist.insert(stdlist.end(), 50u, 1);
   if(!test::CheckEqualContainers(boostlist, stdlist)) return false;

   {
      IntType move_me(1);
      boostlist.insert(boostlist.begin(), 50u, boost::move(move_me));
      stdlist.insert(stdlist.begin(), 50u, 1);
      if(!test::CheckEqualContainers(boostlist, stdlist)) return false;
   }
   {
      IntType move_me(2);
      boostlist.assign(boostlist.size()/2, boost::move(move_me));
      stdlist.assign(stdlist.size()/2, 2);
      if(!test::CheckEqualContainers(boostlist, stdlist)) return false;
   }
   {
      IntType move_me(3);
      boostlist.assign(boostlist.size()*3-1, boost::move(move_me));
      stdlist.assign(stdlist.size()*3-1, 3);
      if(!test::CheckEqualContainers(boostlist, stdlist)) return false;
   }

   {
      IntType copy_me(3);
      const IntType ccopy_me(3);
      boostlist.push_front(copy_me);
      stdlist.push_front(int(3));
      boostlist.push_front(ccopy_me);
      stdlist.push_front(int(3));
      if(!test::CheckEqualContainers(boostlist, stdlist)) return false;
   }
   {  //List(const List &)
      ::boost::movelib::unique_ptr<V1> const pv1 = ::boost::movelib::make_unique<V1>(boostlist);
      ::boost::movelib::unique_ptr<V2> const pv2 = ::boost::movelib::make_unique<V2>(stdlist);

      V1 &v1 = *pv1;
      V2 &v2 = *pv2;

      boostlist.clear();
      stdlist.clear();
      boostlist.assign(v1.begin(), v1.end());
      stdlist.assign(v2.begin(), v2.end());
      if(!test::CheckEqualContainers(boostlist, stdlist)) return 1;
   }
   {  //List(const List &, alloc)
      ::boost::movelib::unique_ptr<V1> const pv1 = ::boost::movelib::make_unique<V1>(boostlist, typename V1::allocator_type());
      ::boost::movelib::unique_ptr<V2> const pv2 = ::boost::movelib::make_unique<V2>(stdlist);

      V1 &v1 = *pv1;
      V2 &v2 = *pv2;

      boostlist.clear();
      stdlist.clear();
      boostlist.assign(v1.begin(), v1.end());
      stdlist.assign(v2.begin(), v2.end());
      if(!test::CheckEqualContainers(boostlist, stdlist)) return 1;
   }

   return true;
}

template<bool DoublyLinked>
struct list_push_data_function
{
   template<class MyBoostList, class MyStdList>
   static int execute(std::size_t max, MyBoostList &boostlist, MyStdList &stdlist)
   {
      typedef typename MyBoostList::value_type IntType;
      for(std::size_t i = 0; i < max; ++i){
         IntType move_me((int)i);
         boostlist.push_back(boost::move(move_me));
         stdlist.push_back((int)i);
         boostlist.push_front(IntType(int(i)));
         stdlist.push_front(int(i));
      }
      if(!CheckEqualContainers(boostlist, stdlist))
         return 1;
      return 0;
   }
};

template<>
struct list_push_data_function<false>
{
   template<class MyBoostList, class MyStdList>
   static int execute(std::size_t max, MyBoostList &boostlist, MyStdList &stdlist)
   {
      typedef typename MyBoostList::value_type IntType;
      for(std::size_t i = 0; i < max; ++i){
         IntType move_me((int)i);
         boostlist.push_front(boost::move(move_me));
         stdlist.push_front((int)i);
         boostlist.push_front(IntType(int(i)));
         stdlist.push_front(int(i));
      }
      if(!CheckEqualContainers(boostlist, stdlist))
         return 1;
      return 0;
   }
};

template<bool DoublyLinked>
struct list_pop_back_function
{
   template<class MyStdList, class MyBoostList>
   static int execute(MyBoostList &boostlist, MyStdList &stdlist)
   {
      boostlist.pop_back();
      stdlist.pop_back();
      if(!CheckEqualContainers(boostlist, stdlist))
         return 1;
      return 0;
   }
};

template<>
struct list_pop_back_function<false>
{
   template<class MyStdList, class MyBoostList>
   static int execute(MyBoostList &boostlist, MyStdList &stdlist)
   {
      (void)boostlist; (void)stdlist;
      return 0;
   }
};

template<class
        , bool>
int list_move_assignable_only(boost::container::dtl::false_type)
{
   return 0;
}

//Function to check if both sets are equal
template < class MyBoostList
         , bool  DoublyLinked >
int list_move_assignable_only(boost::container::dtl::true_type)
{
   typedef std::list<int> MyStdList;
   typedef typename MyBoostList::value_type IntType;
   const std::size_t max = 100u;
   typedef list_push_data_function<DoublyLinked> push_data_t;

   {
      ::boost::movelib::unique_ptr<MyStdList> const stdlistp = ::boost::movelib::make_unique<MyStdList>(100u);
      ::boost::movelib::unique_ptr<MyBoostList> const boostlistp = ::boost::movelib::make_unique<MyBoostList>(100u);
      MyBoostList& boostlist = *boostlistp;
      MyStdList& stdlist = *stdlistp;

      if (push_data_t::execute(max, boostlist, stdlist)) {
         return 1;
      }

      IntType aux_vect[50];
      for (int i = 0; i < 50; ++i) {
         aux_vect[i] = -1;
      }
      int aux_vect2[50];
      for (int i = 0; i < 50; ++i) {
         aux_vect2[i] = -1;
      }
      boostlist.assign(boost::make_move_iterator(&aux_vect[0])
         , boost::make_move_iterator(&aux_vect[50]));
      stdlist.assign(&aux_vect2[0], &aux_vect2[50]);
      if (!CheckEqualContainers(boostlist, stdlist)) return 1;

      for (int i = 0; i < 50; ++i) {
         aux_vect[i] = -1;
      }

      for (int i = 0; i < 50; ++i) {
         aux_vect2[i] = -1;
      }
      boostlist.assign(boost::make_move_iterator(make_input_from_forward_iterator(&aux_vect[0]))
         , boost::make_move_iterator(make_input_from_forward_iterator(&aux_vect[50])));
      stdlist.assign(&aux_vect2[0], &aux_vect2[50]);
      if (!CheckEqualContainers(boostlist, stdlist)) return 1;
   }
   return 0;
}

template<class MyBoostList
        ,bool  DoublyLinked>
int list_test (bool copied_allocators_equal = true)
{
   typedef std::list<int> MyStdList;
   typedef typename MyBoostList::value_type IntType;
   const std::size_t max = 100u;
   typedef list_push_data_function<DoublyLinked> push_data_t;

   {  //List(n)
      ::boost::movelib::unique_ptr<MyBoostList> const pboostlist = ::boost::movelib::make_unique<MyBoostList>(100u);
      ::boost::movelib::unique_ptr<MyStdList> const pstdlist = ::boost::movelib::make_unique<MyStdList>(100u);
      if(!test::CheckEqualContainers(*pboostlist, *pstdlist)) return 1;
   }
   {  //List(n, alloc)
      ::boost::movelib::unique_ptr<MyBoostList> const pboostlist = ::boost::movelib::make_unique<MyBoostList>(100u, typename MyBoostList::allocator_type());
      ::boost::movelib::unique_ptr<MyStdList> const pstdlist = ::boost::movelib::make_unique<MyStdList>(100u);
      if(!test::CheckEqualContainers(*pboostlist, *pstdlist)) return 1;
   }
   {  //List(List &&)
      ::boost::movelib::unique_ptr<MyStdList> const stdlistp = ::boost::movelib::make_unique<MyStdList>(100u);
      ::boost::movelib::unique_ptr<MyBoostList> const boostlistp = ::boost::movelib::make_unique<MyBoostList>(100u);
      ::boost::movelib::unique_ptr<MyBoostList> const boostlistp2 = ::boost::movelib::make_unique<MyBoostList>(::boost::move(*boostlistp));
      if(!test::CheckEqualContainers(*boostlistp2, *stdlistp)) return 1;
   }
   {  //List(List &&, alloc)
      ::boost::movelib::unique_ptr<MyStdList> const stdlistp = ::boost::movelib::make_unique<MyStdList>(100u);
      ::boost::movelib::unique_ptr<MyBoostList> const boostlistp = ::boost::movelib::make_unique<MyBoostList>(100u);
      ::boost::movelib::unique_ptr<MyBoostList> const boostlistp2 = ::boost::movelib::make_unique<MyBoostList>
         (::boost::move(*boostlistp), typename MyBoostList::allocator_type());
      if(!test::CheckEqualContainers(*boostlistp2, *stdlistp)) return 1;
   }
   {  //List operator=(List &&)
      ::boost::movelib::unique_ptr<MyStdList> const stdlistp = ::boost::movelib::make_unique<MyStdList>(100u);
      ::boost::movelib::unique_ptr<MyBoostList> const boostlistp = ::boost::movelib::make_unique<MyBoostList>(100u);
      ::boost::movelib::unique_ptr<MyBoostList> const boostlistp2 = ::boost::movelib::make_unique<MyBoostList>();
      *boostlistp2 = ::boost::move(*boostlistp);
      if (!test::CheckEqualContainers(*boostlistp2, *stdlistp)) return 1;
   }


   ::boost::movelib::unique_ptr<MyBoostList> const pboostlist = ::boost::movelib::make_unique<MyBoostList>();
   ::boost::movelib::unique_ptr<MyStdList>   const pstdlist   = ::boost::movelib::make_unique<MyStdList>();

   MyBoostList &boostlist = *pboostlist;
   MyStdList  &stdlist    = *pstdlist;

   if(push_data_t::execute(max, boostlist, stdlist)){
      return 1;
   }

   boostlist.erase(boostlist.begin()++);
   stdlist.erase(stdlist.begin()++);
   if(!CheckEqualContainers(boostlist, stdlist)) return 1;

   if(list_pop_back_function<DoublyLinked>::execute(boostlist, stdlist)){
      return 1;
   }

   boostlist.pop_front();
   stdlist.pop_front();
   if(!CheckEqualContainers(boostlist, stdlist)) return 1;

   if (0 != list_move_assignable_only<MyBoostList, DoublyLinked>(dtl::bool_<boost::container::test::is_move_assignable<IntType>::value>()))
      return 1;
   
   if(copied_allocators_equal){
      boostlist.sort();
      stdlist.sort();
      if(!CheckEqualContainers(boostlist, stdlist)) return 1;
   }

   boostlist.reverse();
   stdlist.reverse();
   if(!CheckEqualContainers(boostlist, stdlist)) return 1;

   boostlist.reverse();
   stdlist.reverse();
   if(!CheckEqualContainers(boostlist, stdlist)) return 1;

   {
      IntType aux_vect[50];
      for(int i = 0; i < 50; ++i){
         aux_vect[i] = -1;
      }
      int aux_vect2[50];
      for(int i = 0; i < 50; ++i){
         aux_vect2[i] = -1;
      }
      typename MyBoostList::iterator old_begin = boostlist.begin();
      typename MyBoostList::iterator it_insert =
         boostlist.insert(boostlist.begin()
                     ,boost::make_move_iterator(&aux_vect[0])
                     ,boost::make_move_iterator(&aux_vect[50]));
      if(it_insert != boostlist.begin() || boost::container::iterator_distance(it_insert, old_begin) != 50)
         return 1;

      stdlist.insert(stdlist.begin(), &aux_vect2[0], &aux_vect2[50]);
      if(!CheckEqualContainers(boostlist, stdlist))
         return 1;

      for(int i = 0; i < 50; ++i){
         aux_vect[i] = -1;
      }

      for(int i = 0; i < 50; ++i){
         aux_vect2[i] = -1;
      }

      old_begin = boostlist.begin();
      it_insert = boostlist.insert(boostlist.end()
                     ,boost::make_move_iterator(make_input_from_forward_iterator(&aux_vect[0]))
                     ,boost::make_move_iterator(make_input_from_forward_iterator(&aux_vect[50])));
      if(boost::container::iterator_distance(it_insert, boostlist.end()) != 50)
         return 1;
      stdlist.insert(stdlist.end(), &aux_vect2[0], &aux_vect2[50]);
      if(!CheckEqualContainers(boostlist, stdlist))
         return 1;
   }

   boostlist.unique();
   stdlist.unique();
   if(!CheckEqualContainers(boostlist, stdlist))
      return 1;

   if(copied_allocators_equal){
      boostlist.sort(std::greater<IntType>());
      stdlist.sort(std::greater<int>());
      if(!CheckEqualContainers(boostlist, stdlist))
         return 1;
   }

   for(std::size_t i = 0; i < max; ++i){
      IntType new_int((int)i);
      boostlist.insert(boostlist.end(), boost::move(new_int));
      stdlist.insert(stdlist.end(), (int)i);
      if(!test::CheckEqualContainers(boostlist, stdlist)) return 1;
   }
   if(!test::CheckEqualContainers(boostlist, stdlist)) return 1;

   boostlist.resize(25);
   stdlist.resize(25);
   boostlist.resize(50);
   stdlist.resize(50);
   boostlist.resize(0);
   stdlist.resize(0);
   if(!CheckEqualContainers(boostlist, stdlist))
      return 1;

   //some comparison operators
   if(!(boostlist == boostlist))
      return 1;
   if(boostlist != boostlist)
      return 1;
   if(boostlist < boostlist)
      return 1;
   if(boostlist > boostlist)
      return 1;
   if(!(boostlist <= boostlist))
      return 1;
   if(!(boostlist >= boostlist))
      return 1;

   if(push_data_t::execute(max, boostlist, stdlist)){
      return 1;
   }
   {
      MyBoostList otherboostlist(boostlist.get_allocator());
      MyStdList otherstdlist;

      std::size_t listsize = boostlist.size();

      if(push_data_t::execute(listsize, boostlist, stdlist)){
         return 1;
      }

      if(copied_allocators_equal){
         boostlist.splice(boostlist.begin(), otherboostlist);
         stdlist.splice(stdlist.begin(), otherstdlist);
         if(!CheckEqualContainers(boostlist, stdlist))
            return 1;
      }

      listsize = boostlist.size();

      if(push_data_t::execute(listsize, boostlist, stdlist)){
         return 1;
      }

      if(push_data_t::execute(listsize, otherboostlist, otherstdlist)){
         return 1;
      }

      if(copied_allocators_equal){
         boostlist.sort(std::greater<IntType>());
         stdlist.sort(std::greater<int>());
         if(!CheckEqualContainers(boostlist, stdlist))
            return 1;

         otherboostlist.sort(std::greater<IntType>());
         otherstdlist.sort(std::greater<int>());
         if(!CheckEqualContainers(otherboostlist, otherstdlist))
            return 1;

         boostlist.merge(otherboostlist, std::greater<IntType>());
         stdlist.merge(otherstdlist, std::greater<int>());
         if(!CheckEqualContainers(boostlist, stdlist))
            return 1;
      }

      if(!list_copyable_only(boostlist, stdlist
                     ,dtl::bool_<boost::container::test::is_copyable<IntType>::value>())){
         return 1;
      }
   }
   return 0;
}

template<class List>
bool test_list_methods_with_initializer_list_as_argument_for()
{
#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   const std::initializer_list<int> il = {5, 10, 15};
   const List expected_list(il.begin(), il.end());
   {
      List sl = il;
      if(sl != expected_list)
         return false;
   }

   {
      List sl = {1, 2};
      sl = il;
      if(sl != expected_list)
         return false;
   }
   {
      List sl({ 1, 2 }, typename List::allocator_type());
      sl = il;
      if (sl != expected_list)
         return false;
   }
   {
      List sl = {4, 5};
      sl.assign(il);
      if(sl != expected_list)
         return false;
   }

   {
      List sl = {15};
      sl.insert(sl.cbegin(), {5, 10});
      if(sl != expected_list)
         return false;
   }

   {
       List sl = {5};
       sl.insert_after(sl.cbegin(), {10, 15});
       if(sl != expected_list)
          return false;
   }
   return true;
#endif
   return true;
}


}  //namespace test{
}  //namespace container {
}  //namespace boost{

#include <boost/container/detail/config_end.hpp>

#endif
