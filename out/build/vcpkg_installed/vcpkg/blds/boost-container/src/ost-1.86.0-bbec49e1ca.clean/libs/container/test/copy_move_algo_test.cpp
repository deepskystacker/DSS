//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

// the tests trigger deprecation warnings when compiled with msvc in C++17 mode
#if defined(_MSVC_LANG) && _MSVC_LANG > 201402
// warning STL4009: std::allocator<void> is deprecated in C++17
# define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#endif

#include <boost/core/lightweight_test.hpp>
#include <boost/container/detail/copy_move_algo.hpp>
#include <boost/container/detail/advanced_insert_int.hpp>
#include <boost/container/detail/algorithm.hpp>
#include <boost/container/detail/placement_new.hpp>
#include <boost/container/allocator_traits.hpp>
#include <boost/container/new_allocator.hpp>
#include <boost/move/unique_ptr.hpp>
#include <boost/move/make_unique.hpp>
#include <vector>
#include "movable_int.hpp"

using namespace boost::container;

namespace boost {
namespace container {
namespace test {

//This function tests all the possible combinations when
//inserting data in a vector and expanding backwards
template<class ValueType>
void test_expand_backward_forward_and_insert_alloc()
{
   typedef ValueType value_type;

   //Distance old and new buffer
   const unsigned int Offset[] =
   { 350, 300, 250, 200, 150, 100, 150, 100,
      150,  50,  50,  50 };
   //Initial vector size
   const unsigned int InitialSize[] =
   { 200, 200, 200, 200, 200, 200, 200, 200,
      200, 200, 200, 200 };
   //Size of the data to insert
   const unsigned int InsertSize[] =
   { 100, 100, 100, 100, 100, 100, 200, 200,
      300,  25, 100, 200 };
   //Number of tests
   const unsigned int Iterations = sizeof(InsertSize) / sizeof(int);

   //Insert position
   const int Position[] =
   { 0, 100,  200 };

   for (unsigned backmove = 2u; backmove != 0u; ) {
      --backmove;
      for (unsigned int pos = 0; pos < sizeof(Position) / sizeof(Position[0]); ++pos) {
         BOOST_TEST(life_count<value_type>::check(0));

         for (unsigned int iteration = 0; iteration < Iterations; ++iteration)
         {
            typedef std::vector<value_type>  std_vector_val_t;
            typedef boost::container::new_allocator<value_type> allocator_type;
            typedef dtl::insert_range_proxy<allocator_type, typename std_vector_val_t::iterator> proxy_t;
            {

               std_vector_val_t initial_data;
               initial_data.resize(InitialSize[iteration]);
               for (unsigned int i = 0; i < InitialSize[iteration]; ++i) {
                  initial_data[i] = static_cast<value_type>((int)i);
               }
               BOOST_TEST(life_count<value_type>::check(InitialSize[iteration]));

               std_vector_val_t data_to_insert;
               data_to_insert.resize(InsertSize[iteration]);
               for (unsigned int i = 0; i < InsertSize[iteration]; ++i) {
                  data_to_insert[i] = static_cast<value_type>(-(int)i);
               }

               BOOST_TEST(life_count<value_type>::check(InitialSize[iteration] + InsertSize[iteration]));

               const unsigned int BufferSize = InitialSize[iteration] + InsertSize[iteration] + Offset[iteration];
               boost::movelib::unique_ptr<char[]> memptr =
                  boost::movelib::make_unique_definit<char[]>(BufferSize * sizeof(value_type));
               value_type* memory = move_detail::force_ptr<value_type*>(memptr.get());
               allocator_type a;

               value_type* final_memory;
               value_type* initial_memory;
               if(backmove){
                  initial_memory = memory + Offset[iteration];
                  final_memory = memory;
               }
               else{
                  initial_memory = memory + BufferSize - InitialSize[iteration] - Offset[iteration];
                  final_memory = memory + BufferSize - InitialSize[iteration] - InsertSize[iteration];
               }

               for (unsigned int i = 0; i < InitialSize[iteration]; ++i) {
                  allocator_traits<allocator_type>::construct(a, &initial_memory[i], (int)i);
               }

               proxy_t proxy(data_to_insert.begin());
               boost::container::expand_backward_forward_and_insert_alloc
               (initial_memory, initial_data.size(), final_memory, initial_memory + Position[pos]
                  , data_to_insert.size(), proxy, a);

               BOOST_TEST(life_count<value_type>::check(InitialSize[iteration] * 2 + InsertSize[iteration] * 2));

               initial_data.insert(initial_data.begin() + Position[pos]
                  , data_to_insert.begin(), data_to_insert.end());

               //Now check that values are equal
               BOOST_TEST(boost::container::algo_equal(initial_data.begin(), initial_data.end(), final_memory));

               boost::container::destroy_alloc_n(a, final_memory, InitialSize[iteration] + InsertSize[iteration]);
            }
            BOOST_TEST(life_count<value_type>::check(0));
         }
      }
   }
}

}  //namespace test {
}  //namespace container {
}  //namespace boost {

using namespace boost::container;

int main()
{
   test::test_expand_backward_forward_and_insert_alloc<test::movable_and_copyable_int>();
   return boost::report_errors();
}
