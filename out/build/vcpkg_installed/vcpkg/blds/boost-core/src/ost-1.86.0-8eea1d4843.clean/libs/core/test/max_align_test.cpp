// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/max_align.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/type_traits.hpp>
#include <boost/config.hpp>
#include <cstddef>

#if defined(BOOST_LIBSTDCXX_VERSION) && BOOST_LIBSTDCXX_VERSION < 40900
# define BOOST_NO_STD_MAX_ALIGN_T
#endif

struct X
{
};

int main()
{
    BOOST_TEST_EQ( boost::core::max_align, boost::alignment_of<boost::core::max_align_t>::value );

    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<char>::value );
    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<short>::value );
    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<int>::value );
    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<long>::value );

#if !defined(BOOST_NO_LONG_LONG)

    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<boost::long_long_type>::value );

#endif

#if defined(BOOST_HAS_INT128)

    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<boost::int128_type>::value );

#endif

    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<float>::value );
    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<double>::value );
    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<long double>::value );

#if defined(BOOST_CORE_HAS_FLOAT128)

    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<__float128>::value );

#endif

    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<void*>::value );
    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<void(*)()>::value );

    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<int X::*>::value );
    BOOST_TEST_GE( boost::core::max_align, boost::alignment_of<void (X::*)()>::value );

#if !defined(BOOST_NO_CXX11_ALIGNOF) && !defined(BOOST_NO_STD_MAX_ALIGN_T)

    BOOST_TEST_GE( boost::core::max_align, alignof( std::max_align_t ) );

#endif

    return boost::report_errors();
}
