// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(__clang__) && defined(__has_warning)
# if __has_warning( "-Wdeprecated-copy" )
#  pragma clang diagnostic ignored "-Wdeprecated-copy"
# endif
#endif

#include <boost/core/serialization.hpp>

struct X
{
    int a, b;

private:

    friend class boost::serialization::access;

    template<class Ar> void load( Ar& ar, unsigned /*v*/ )
    {
        ar >> a;
        ar >> b;
    }

    template<class Ar> void save( Ar& ar, unsigned /*v*/ ) const
    {
        ar << a;
        ar << b;
    }

    template<class Ar> void serialize( Ar& ar, unsigned v )
    {
        boost::core::split_member( ar, *this, v );
    }
};

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/core/lightweight_test.hpp>
#include <sstream>
#include <string>

int main()
{
    std::ostringstream os;

    X x1 = { 7, 11 };

    {
        boost::archive::text_oarchive ar( os );
        ar << x1;
    }

    std::string s = os.str();

    X x2 = { 0, 0 };

    {
        std::istringstream is( s );
        boost::archive::text_iarchive ar( is );
        ar >> x2;
    }

    BOOST_TEST_EQ( x1.a, x2.a );
    BOOST_TEST_EQ( x1.b, x2.b );

    return boost::report_errors();
}
