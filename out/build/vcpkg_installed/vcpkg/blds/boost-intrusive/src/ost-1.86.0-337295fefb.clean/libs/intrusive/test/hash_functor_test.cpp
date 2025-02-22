/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2023-2024.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#include <boost/intrusive/unordered_set.hpp>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cstring>

using namespace boost::intrusive;


//Generic class that stores a key as "first" element
template <class Key>
struct first_key;

template<class Key>
class FirstKeyClass : public unordered_set_base_hook<>
{
public:
   Key first;

   explicit FirstKeyClass() : first() {}
};

template<class T>
struct equal_type
   : boost::intrusive::value_equal<T>
{};

template<class T, std::size_t N>
struct equal_type<T[N]>
{
   bool operator() (T const (&a)[N], T const (&b)[N])
   {
      for (std::size_t i = 0; i != N; ++i) {
         if (a[i] != b[i])
            return false;
      }

      return true;
   }
};

//key_of_value argument when isnerting FirstKeyClass objects in unordered map
template <class Key>
struct first_key
{
   typedef Key type;

   const type& operator()(const FirstKeyClass<Key>& v) const
   {
      return v.first;
   }
};

//Function that instantiates an unordered intrusive container that stores FirstKeyClass objects
template<class Key>
void instantiate_first_key_unordered()
{
   typedef unordered_set< FirstKeyClass<Key>, key_of_value<first_key<Key> >, equal<equal_type<Key> > > UnorderedMap;
   typedef typename UnorderedMap::bucket_type  bucket_type;
   typedef typename UnorderedMap::bucket_traits bucket_traits;
   typedef typename UnorderedMap::value_type   value_type;
   typedef typename UnorderedMap::key_of_value  key_of_value;

   const std::size_t bucket_len = 2;
   bucket_type buckets[bucket_len];

   UnorderedMap u(bucket_traits(buckets, bucket_len));
   value_type v;
   assert(u.find(key_of_value()(v)) == u.end());
   u.insert(v);
   value_type v2;
   assert(u.find(v2.first) != u.end());
   u.clear();
}

//Function that instantiates typical values in an unordered intrusive container
template<class ValueType>
void instantiate_value_unordered()
{
   typedef unordered_set< ValueType > Unordered;
   typedef typename Unordered::bucket_type  bucket_type;
   typedef typename Unordered::bucket_traits bucket_traits;
   typedef typename Unordered::value_type     value_type;

   const std::size_t bucket_len = 2;
   bucket_type buckets[bucket_len];

   Unordered u(bucket_traits(buckets, bucket_len));
   value_type v;
   assert(u.find(v) == u.end());
   u.insert(v);
   value_type v2;
   assert(u.find(v2) != u.end());
   u.clear();
}

enum MyEnum
{
   MyZero,
   MyOne,
   MyTwo
};

//Function that tests all scalar types
void test_first_key_scalar_unordered()
{
   //characters
   instantiate_first_key_unordered<char>();

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
   instantiate_first_key_unordered<wchar_t>();
#endif
#ifndef BOOST_NO_CXX11_CHAR16_T
   instantiate_first_key_unordered<char16_t>();
#endif
#ifndef BOOST_NO_CXX11_CHAR32_T
   instantiate_first_key_unordered<char32_t>();
#endif
#if defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
   instantiate_first_key_unordered<char8_t>();
#endif

   //integers
   instantiate_first_key_unordered<signed char>();
   instantiate_first_key_unordered<unsigned char>();
   instantiate_first_key_unordered<signed short>();
   instantiate_first_key_unordered<unsigned short>();
   instantiate_first_key_unordered<signed int>();
   instantiate_first_key_unordered<unsigned int>();
   instantiate_first_key_unordered<signed long>();
   instantiate_first_key_unordered<unsigned long>();
#ifdef BOOST_HAS_LONG_LONG
   instantiate_first_key_unordered< ::boost::long_long_type>();
   instantiate_first_key_unordered< ::boost::ulong_long_type>();
#endif
#ifdef BOOST_HAS_INT128
   instantiate_first_key_unordered< ::boost::int128_type>();
   instantiate_first_key_unordered< ::boost::uint128_type>();
#endif

   //floating
   instantiate_first_key_unordered<float>();
   instantiate_first_key_unordered<double>();
   instantiate_first_key_unordered<long double>();

   //Array
   instantiate_first_key_unordered<int[2]>();
   instantiate_first_key_unordered<const unsigned[3]>();

   //Pointer
   instantiate_first_key_unordered<void*>();
   instantiate_first_key_unordered<const void*>();

   //std::nullptr_t
#if !defined(BOOST_NO_CXX11_NULLPTR)
   instantiate_first_key_unordered<std::nullptr_t>();
#endif

   //Enum
   instantiate_first_key_unordered<MyEnum>();
}

namespace some_ns {

   template<class T>
   class HashValueDefined : public unordered_set_base_hook<>
   {
   private:
      T val;

   public:
      HashValueDefined() : val() {}

      friend bool operator==(const HashValueDefined& a, const HashValueDefined& b)
      {
         return a.val == b.val;
      }

      friend std::size_t hash_value(const HashValueDefined& a)
      {
         return static_cast<std::size_t>(a.val);
      }
   };

}  // namespace some_ns {

template<class T>
class HashValueDefined2 : public unordered_set_base_hook<>
{
private:
   T val;

public:
   HashValueDefined2() : val() {}

   friend bool operator==(const HashValueDefined2& a, const HashValueDefined2& b)
   {
      return a.val == b.val;
   }

   friend std::size_t hash_value(const HashValueDefined2& a)
   {
      return static_cast<std::size_t>(a.val);
   }
};

void test_value_unordered()
{
   instantiate_value_unordered<some_ns::HashValueDefined<int> >();
   instantiate_value_unordered<some_ns::HashValueDefined<unsigned> >();
   instantiate_value_unordered<some_ns::HashValueDefined<float> >();
   instantiate_value_unordered<HashValueDefined2<int> >();
   instantiate_value_unordered<HashValueDefined2<unsigned> >();
   instantiate_value_unordered<HashValueDefined2<float> >();
}


#if (BOOST_CXX_VERSION >= 201103L)
#include <string>
#include <list>
#include <boost/container_hash/hash.hpp>

template<class StdBase>
class DerivedFromType : public unordered_set_base_hook<>, public StdBase
{
   friend bool operator==(const DerivedFromType& a, const DerivedFromType& b)
   {
      return static_cast<const StdBase&>(a) == static_cast<const StdBase&>(b);
   }
};

void test_value_stdbase_unordered()
{
   instantiate_first_key_unordered<DerivedFromType<std::string> >();
   instantiate_first_key_unordered<DerivedFromType<std::list<int> > >();
   instantiate_value_unordered<DerivedFromType<std::string> >();
   instantiate_value_unordered<DerivedFromType<std::list<int> > >();
}
#endif


int main()
{
   test_first_key_scalar_unordered();
   test_value_unordered();
#if (BOOST_CXX_VERSION >= 201103L)
   test_value_stdbase_unordered();
#endif
   return 0;
}
