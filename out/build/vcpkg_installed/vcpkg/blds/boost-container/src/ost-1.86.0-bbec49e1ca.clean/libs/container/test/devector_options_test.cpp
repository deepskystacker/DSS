//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#include <boost/container/devector.hpp>
#include <boost/container/allocator.hpp>
#include <boost/container/detail/next_capacity.hpp>
#include <boost/core/lightweight_test.hpp>

using namespace boost::container;

template<class Unsigned, class DevectorType>
void test_stored_size_type_impl()
{
   #ifndef BOOST_NO_EXCEPTIONS
   DevectorType v;
   typedef typename DevectorType::size_type    size_type;
   typedef typename DevectorType::value_type   value_type;
   size_type const max = Unsigned(-1);
   v.resize(5);
   v.resize(max);
   BOOST_TEST_THROWS(v.resize(max+1),                    std::exception);
   BOOST_TEST_THROWS(v.push_back(value_type(1)),         std::exception);
   BOOST_TEST_THROWS(v.insert(v.begin(), value_type(1)), std::exception);
   BOOST_TEST_THROWS(v.emplace(v.begin(), value_type(1)),std::exception);
   BOOST_TEST_THROWS(v.reserve(max+1),                   std::exception);
   BOOST_TEST_THROWS(DevectorType v2(max+1),             std::exception);
   #endif
}

template<class Unsigned>
void test_stored_size_type()
{
   #if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
   using options_t = devector_options_t< stored_size<Unsigned> >;
   #else
   typedef typename devector_options
      < stored_size<Unsigned> >::type options_t;
   #endif

   //Test first with a typical allocator
   {
      typedef devector<unsigned char, new_allocator<unsigned char>, options_t> devector_t;
      test_stored_size_type_impl<Unsigned, devector_t>();
   }
   //Test with a V2 allocator
   {
      typedef devector<unsigned char, allocator<unsigned char>, options_t> devector_t;
      test_stored_size_type_impl<Unsigned, devector_t>();
   }
   //Test size reduction
   {
      typedef devector<unsigned char, void, options_t> devector_t;
      BOOST_CONTAINER_STATIC_ASSERT( sizeof(Unsigned) >= sizeof(std::size_t) ||
                           sizeof(devector_t) < sizeof(devector<unsigned char>) );
   }
   
}

void test_growth_factor_50()
{
   #if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
   using options_t = devector_options_t< growth_factor<growth_factor_50> >;
   #else
   typedef devector_options
      < growth_factor<growth_factor_50> >::type options_t;
   #endif

   devector<int, new_allocator<int>, options_t> v(5u, 0);
   std::size_t old_capacity = v.size() + v.back_free_capacity() + v.front_free_capacity();
   v.push_back(0);
   std::size_t new_capacity = v.size() + v.back_free_capacity() + v.front_free_capacity();
   BOOST_TEST(new_capacity == old_capacity + old_capacity/2);
}

void test_growth_factor_60()
{
   #if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
   using options_t = devector_options_t< growth_factor<growth_factor_60> >;
   #else
   typedef devector_options
      < growth_factor<growth_factor_60> >::type options_t;
   #endif

   devector<int, new_allocator<int>, options_t> v(5u, 0);

   std::size_t old_capacity = v.size()+v.back_free_capacity()+v.front_free_capacity();
   v.push_back(0);
   std::size_t new_capacity = v.size() + v.back_free_capacity() + v.front_free_capacity();
   BOOST_TEST(new_capacity == old_capacity + 3*old_capacity/5);
}

void test_growth_factor_100()
{
   #if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
   using options_t = devector_options_t< growth_factor<growth_factor_100> >;
   #else
   typedef devector_options
      < growth_factor<growth_factor_100> >::type options_t;
   #endif

   devector<int, new_allocator<int>, options_t> v(5,0);
   std::size_t old_capacity = v.size() + v.back_free_capacity() + v.front_free_capacity();
   v.push_back(0);
   std::size_t new_capacity = v.size() + v.back_free_capacity() + v.front_free_capacity();
   BOOST_TEST(new_capacity == 2*old_capacity);
}


void  test_stored_reloc_on_66()
{
   #if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
   using options_t = devector_options_t< relocate_on_66 >;
   #else
   typedef devector_options
      < relocate_on_66 >::type options_t;
   #endif
   const std::size_t buffer_size = 32u;
   const std::size_t initial_side = buffer_size/2u;

   devector<int, new_allocator<int>, options_t> v(initial_side, initial_side, reserve_only_tag_t());

   const int* buffer = v.data();
   const int* buffer_start = v.data() - initial_side;
   std::size_t old_cp = v.capacity();
   for ( int i = 0u; i != (int)initial_side; ++i) {
      v.push_back(i);
   }
   BOOST_TEST(v.back_free_capacity() == 0);
   BOOST_TEST(buffer == v.data());
   v.push_back(0);
   BOOST_TEST(v.size() == initial_side + 1u);
   //Relocation -> 9 elements on the left, 8 elements on the right
   BOOST_TEST(v.front_free_capacity() == (buffer_size - v.size())/2u);
   BOOST_TEST(v.data() == buffer_start + v.front_free_capacity());
   BOOST_TEST(v.capacity() == old_cp);

   //Reach the back limit again
   for (int i = 0u, max = (int)v.back_free_capacity(); i != max; ++i) {
      v.push_back(-i);
   }
   BOOST_TEST(v.back_free_capacity() == 0);

   //New insertion should reallocate
   v.push_back(-1);
   BOOST_TEST(v.back_free_capacity() > initial_side/2u);
   BOOST_TEST(v.capacity() > old_cp);
}

void  test_stored_reloc_on_90()
{
   #if !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
   using options_t = devector_options_t< relocate_on_90 >;
   #else
   typedef devector_options< relocate_on_90 >::type options_t;
   #endif

   const std::size_t buffer_size = 32u;
   const std::size_t initial_side = buffer_size/2u;

   devector<int, new_allocator<int>, options_t> v(initial_side, initial_side, reserve_only_tag_t());

   const int* buffer = v.data();
   const int* buffer_start = v.data() - initial_side;
   std::size_t old_cp = v.capacity();
   for ( int i = 0u; i != (int)initial_side; ++i) {
      v.push_back(i);
   }
   BOOST_TEST(v.back_free_capacity() == 0);
   BOOST_TEST(buffer == v.data());
   v.push_back(0);
   BOOST_TEST(v.size() == initial_side + 1u);
   //Relocation -> 9 elements on the left, 8 elements on the right
   BOOST_TEST(v.front_free_capacity() == (buffer_size - v.size())/2u);
   BOOST_TEST(v.data() == buffer_start + v.front_free_capacity());
   BOOST_TEST(v.capacity() == old_cp);

   //Reach the back limit again
   for (int i = 0u, max = (int)v.back_free_capacity(); i != max; ++i) {
      v.push_back(-i);
   }
   BOOST_TEST(v.back_free_capacity() == 0);

   //New insertion should relocate again
   v.push_back(-1);
   BOOST_TEST(v.capacity() == old_cp);
   BOOST_TEST(v.front_free_capacity() == (buffer_size - v.size()) / 2u);
   BOOST_TEST(v.data() == buffer_start + v.front_free_capacity());

   //Reach the back limit again
   for (int i = 0u, max = (int)v.back_free_capacity(); i != max; ++i) {
      v.push_back(-i);
   }
   BOOST_TEST(v.back_free_capacity() == 0);

   //Last insertion should reallocate
   v.push_back(-1);
   BOOST_TEST(v.capacity() > old_cp);
}

int main()
{
   test_growth_factor_50();
   test_growth_factor_60();
   test_growth_factor_100();
   test_stored_size_type<unsigned char>();
   test_stored_size_type<unsigned short>();
   test_stored_reloc_on_66();
   test_stored_reloc_on_90();
   return ::boost::report_errors();
}
