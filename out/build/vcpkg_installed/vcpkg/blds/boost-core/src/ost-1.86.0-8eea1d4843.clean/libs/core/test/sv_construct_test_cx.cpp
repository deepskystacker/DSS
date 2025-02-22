// Copyright 2021, 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/detail/string_view.hpp>
#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX14_CONSTEXPR)

BOOST_PRAGMA_MESSAGE( "Test skipped because BOOST_NO_CXX14_CONSTEXPR is defined" )
int main() {}

#else

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
# include <string_view>
#endif

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main()
{
    {
        constexpr boost::core::string_view sv;

        STATIC_ASSERT( sv.data() == nullptr );
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

        constexpr boost::core::string_view sv( s, 0 );

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

        constexpr boost::core::string_view sv( s, 2 );

        STATIC_ASSERT( sv.data() == s );
        STATIC_ASSERT( sv.size() == 2 );

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

        constexpr boost::core::string_view sv( s, s );

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

        constexpr boost::core::string_view sv( s, s + 2 );

        STATIC_ASSERT( sv.data() == s );
        STATIC_ASSERT( sv.size() == 2 );

        STATIC_ASSERT( sv.begin() == sv.data() );
        STATIC_ASSERT( sv.end() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.cbegin() == sv.data() );
        STATIC_ASSERT( sv.cend() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.length() == sv.size() );
        STATIC_ASSERT( sv.empty() == ( sv.size() == 0 ) );

        STATIC_ASSERT( sv.max_size() == boost::core::string_view::npos );
    }

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)

    {
        constexpr std::string_view str( "123", 3 );

        constexpr boost::core::string_view sv( str );

        STATIC_ASSERT( sv.data() == str.data() );
        STATIC_ASSERT( sv.size() == str.size() );

        STATIC_ASSERT( sv.begin() == sv.data() );
        STATIC_ASSERT( sv.end() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.cbegin() == sv.data() );
        STATIC_ASSERT( sv.cend() == sv.data() + sv.size() );

        STATIC_ASSERT( sv.length() == sv.size() );
        STATIC_ASSERT( sv.empty() == ( sv.size() == 0 ) );

        STATIC_ASSERT( sv.max_size() == boost::core::string_view::npos );
    }

#endif
}

#endif
