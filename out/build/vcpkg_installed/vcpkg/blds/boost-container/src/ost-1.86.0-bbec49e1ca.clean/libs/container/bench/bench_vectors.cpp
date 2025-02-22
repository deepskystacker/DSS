//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <deque>
#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/devector.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/container/stable_vector.hpp>
#include <iomanip>

#include <memory>    //std::allocator
#include <iostream>  //std::cout, std::endl
#include <cstring>   //std::strcmp
#include <boost/move/detail/nsec_clock.hpp>
#include <typeinfo>

#if defined(BOOST_GCC) && (BOOST_GCC >= 40600)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#endif

//capacity
#define BOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME capacity
#define BOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEG namespace boost { namespace container { namespace test {
#define BOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define BOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MIN 0
#define BOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MAX 0
#include <boost/intrusive/detail/has_member_function_callable_with.hpp>

//#pragma GCC diagnostic ignored "-Wunused-result"
#if defined(BOOST_GCC) && (BOOST_GCC >= 40600)
#pragma GCC diagnostic pop
#endif

using boost::move_detail::cpu_timer;
using boost::move_detail::cpu_times;
using boost::move_detail::nanosecond_type;

namespace bc = boost::container;

class MyInt
{
   int int_;

   public:
   inline explicit MyInt(int i = 0)
      : int_(i)
   {}

   inline MyInt(const MyInt &other)
      :  int_(other.int_)
   {}

   inline MyInt & operator=(const MyInt &other)
   {
      int_ = other.int_;
      return *this;
   }

   inline ~MyInt()
   {
      int_ = 0;
   }
};

template<class C, bool = boost::container::test::
         has_member_function_callable_with_capacity<C>::value>
struct capacity_wrapper
{
   inline static typename C::size_type get_capacity(const C &c)
   {  return c.capacity(); }

   inline static void set_reserve(C &c, typename C::size_type cp)
   {  c.reserve(cp); }
};

template<class C>
struct capacity_wrapper<C, false>
{
   inline static typename C::size_type get_capacity(const C &)
   {  return 0u; }

   inline static void set_reserve(C &, typename C::size_type )
   { }
};

const std::size_t RangeSize = 5;

struct insert_end_range
{
   inline std::size_t capacity_multiplier() const
   {  return RangeSize;  }

   template<class C>
   inline void operator()(C &c, int)
   {  c.insert(c.end(), &a[0], &a[0]+RangeSize); }

   const char *name() const
   {  return "insert_end_range"; }

   MyInt a[RangeSize];
};

struct insert_end_repeated
{
   inline std::size_t capacity_multiplier() const
   {  return RangeSize;  }

   template<class C>
   inline void operator()(C &c, int i)
   {  c.insert(c.end(), RangeSize, MyInt(i)); }

   inline const char *name() const
   {  return "insert_end_repeated"; }

   MyInt a[RangeSize];
};

struct push_back
{
   inline std::size_t capacity_multiplier() const
   {  return 1;  }

   template<class C>
   inline void operator()(C &c, int i)
   {  c.push_back(MyInt(i)); }

   inline const char *name() const
   {  return "push_back"; }
};

struct emplace_back
{
   inline std::size_t capacity_multiplier() const
   {  return 1;  }

   template<class C>
   inline void operator()(C &c, int i)
   {  c.emplace_back(i); }

   inline const char *name() const
   {  return "emplace_back"; }
};

struct insert_near_end_repeated
{
   inline std::size_t capacity_multiplier() const
   {  return RangeSize;  }

   template<class C>
   inline void operator()(C &c, int i)
   {  c.insert(c.size() >= 2*RangeSize ? c.end()-2*RangeSize : c.begin(), RangeSize, MyInt(i)); }

   inline const char *name() const
   {  return "insert_near_end_repeated"; }
};

struct insert_near_end_range
{
   inline std::size_t capacity_multiplier() const
   {  return RangeSize;  }

   template<class C>
   inline void operator()(C &c, int)
   {
      c.insert(c.size() >= 2*RangeSize ? c.end()-2*RangeSize : c.begin(), &a[0], &a[0]+RangeSize);
   }

   inline const char *name() const
   {  return "insert_near_end_range"; }

   MyInt a[RangeSize];
};

struct insert_near_end
{
   inline std::size_t capacity_multiplier() const
   {  return 1;  }

   template<class C>
   inline void operator()(C &c, int i)
   {
      typedef typename C::iterator it_t;
      it_t it (c.end());
      it -= static_cast<typename C::difference_type>(c.size() >= 2)*2;
      c.insert(it, MyInt(i));
   }

   inline const char *name() const
   {  return "insert_near_end"; }
};

struct emplace_near_end
{
   inline std::size_t capacity_multiplier() const
   {
      return 1;
   }

   template<class C>
   inline void operator()(C& c, int i)
   {
      typedef typename C::iterator it_t;
      it_t it(c.end());
      it -= static_cast<typename C::difference_type>(c.size() >= 2) * 2;
      c.emplace(it, i);
   }

   inline const char* name() const
   {
      return "emplace_near_end";
   }
};

template<class Container, class Operation>
void vector_test_template(std::size_t num_iterations, std::size_t num_elements, const char *cont_name, bool prereserve = true)
{
   typedef capacity_wrapper<Container> cpw_t;

   Operation op;
   const typename Container::size_type multiplier = op.capacity_multiplier();
   Container c;
   if (prereserve) {
      cpw_t::set_reserve(c, num_elements);
   }

   cpu_timer timer;

   const std::size_t max = num_elements/multiplier;
   for(std::size_t r = 0; r != num_iterations; ++r){

      //Unroll the loop to avoid noise from loop code
      int i = 0;
      if (r > 0)  //Exclude first iteration to avoid noise
         timer.resume();
      for(std::size_t e = 0; e < max/16; ++e){
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
         op(c, static_cast<int>(i++));
      }

      if (r > 0)
         timer.stop();
      c.clear();
   }

   timer.stop();

   std::size_t capacity = cpw_t::get_capacity(c);

   nanosecond_type nseconds = timer.elapsed().wall;

   std::cout   << cont_name << "->" << op.name() <<" ns: "
               << std::setw(8)
               << float(nseconds)/float((num_iterations-1)*num_elements)
               << '\t'
               << "Capacity: " << capacity
               << std::endl;
}

template<class Operation>
void test_vectors()
{
   //#define SINGLE_TEST
   #define SIMPLE_IT
   #ifdef SINGLE_TEST
      #ifdef NDEBUG
      std::size_t numit [] = { 1000 };
      #else
      std::size_t numit [] = { 20 };
      #endif
      std::size_t numele [] = { 100000 };
   #elif defined SIMPLE_IT
      std::size_t numit [] = { 10 };
      std::size_t numele [] = { 100000 };
   #else
      #ifdef NDEBUG
      unsigned int numit []  = { 1000, 10000, 100000, 1000000 };
      #else
      unsigned int numit []  = { 100, 1000, 10000, 100000 };
      #endif
      unsigned int numele [] = { 10000, 1000,   100,     10       };
   #endif

   //#define PRERESERVE_ONLY
   #ifdef PRERESERVE_ONLY
   #define P_INIT 1
   #else
   #define P_INIT 0
   #endif

   for (unsigned p = P_INIT; p != 2; ++p) {
      std::cout << Operation().name() << ", prereserve: " << (p ? "1" : "0") << "\n" << std::endl;
      const bool bp =p != 0;
      for(unsigned int i = 0; i < sizeof(numele)/sizeof(numele[0]); ++i){
         vector_test_template< std::vector<MyInt, std::allocator<MyInt> >, Operation >(numit[i], numele[i], "std::vector  ", bp);
         vector_test_template< bc::vector<MyInt, std::allocator<MyInt> >, Operation >(numit[i], numele[i]        ,  "vector       ", bp);
         vector_test_template< bc::small_vector<MyInt, 0, std::allocator<MyInt> >, Operation >(numit[i], numele[i], "small_vector ", bp);
         vector_test_template< bc::devector<MyInt, std::allocator<MyInt> >, Operation >(numit[i], numele[i],        "devector     ", bp);
         //vector_test_template< std::deque<MyInt, std::allocator<MyInt> >, Operation >(numit[i], numele[i],          "std::deque   ", bp);
         vector_test_template< bc::deque<MyInt, std::allocator<MyInt> >, Operation >(numit[i], numele[i],           "deque        ", bp);
      }
      std::cout << "---------------------------------\n---------------------------------\n";
   }
}

int main()
{
   //end
   test_vectors<push_back>();
   test_vectors<insert_end_range>();
   test_vectors<insert_end_repeated>();
   //near end
   test_vectors<insert_near_end>();
   test_vectors<insert_near_end_range>();
   test_vectors<insert_near_end_repeated>();
   #if BOOST_CXX_VERSION  >= 201103L 
   test_vectors<emplace_back>();
   test_vectors<emplace_near_end>();
   #endif

   return 0;
}
