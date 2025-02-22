// Copyright 2021, 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/detail/string_view.hpp>
#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX14_CONSTEXPR)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX14_CONSTEXPR is defined" )
int main() {}

#elif BOOST_CXX_VERSION < 201703L

// std::char_traits is not constexpr in C++14

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_CXX_VERSION < 201703L" )
int main() {}

#elif defined(BOOST_LIBSTDCXX_VERSION) && BOOST_LIBSTDCXX_VERSION < 70300

// std::char_traits is not constexpr in libstdc++ 7.2

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_LIBSTDCXX_VERSION < 70300" )
int main() {}

#else

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main()
{
    {
        constexpr char const* s = "";

        constexpr boost::core::string_view sv( s );

        STATIC_ASSERT( sv.data() == s );
        STATIC_ASSERT( sv.size() == 0 );

        STATIC_ASSERT( sv.begin() == sv.data() );
        STATIC_ASSERT( sv.end() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.cbegin() == sv.data() );
        STATIC_ASSERT( sv.cend() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.length() == sv.size() );
        STATIC_ASSERT( sv.empty() == ( sv.size() == 0 ) );

        STATIC_ASSERT( sv.max_size() == boost::core::string_view::npos );
    }

    {
        constexpr char const* s = "123";

        constexpr boost::core::string_view sv( s );

        STATIC_ASSERT( sv.data() == s );
        STATIC_ASSERT( sv.size() == 3 );

        STATIC_ASSERT( sv.begin() == sv.data() );
        STATIC_ASSERT( sv.end() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.cbegin() == sv.data() );
        STATIC_ASSERT( sv.cend() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.length() == sv.size() );
        STATIC_ASSERT( sv.empty() == ( sv.size() == 0 ) );

        STATIC_ASSERT( sv.max_size() == boost::core::string_view::npos );
    }
}

#endif
