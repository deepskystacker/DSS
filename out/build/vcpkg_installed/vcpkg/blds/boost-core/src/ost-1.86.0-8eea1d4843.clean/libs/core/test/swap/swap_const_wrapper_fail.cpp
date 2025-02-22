// Copyright 2018 Andrzej Krzemieński
// Copyright 2018 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.

#include <boost/core/invoke_swap.hpp>

namespace boost
{

template<class T> struct Wrapper
{
    T value;
};

template<class T> inline void swap( Wrapper<T> & w, Wrapper<T> & v )
{
    boost::core::invoke_swap( w, v );
}

} // namespace boost

int main()
{
    boost::Wrapper<int> const w = { 2 };
    boost::Wrapper<int> const v = { 3 };

    swap( w, v );
}
