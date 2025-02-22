// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/memory_resource.hpp>
#include <boost/core/lightweight_test.hpp>
#include <new>
#include <cstddef>

static bool do_allocate_called;
static std::size_t do_allocate_bytes;
static std::size_t do_allocate_alignment;

static bool do_deallocate_called;
static void* do_deallocate_p;
static std::size_t do_deallocate_bytes;
static std::size_t do_deallocate_alignment;

struct R1: public boost::core::memory_resource
{
    void* do_allocate( std::size_t bytes, std::size_t alignment )
    {
        do_allocate_called = true;
        do_allocate_bytes = bytes;
        do_allocate_alignment = alignment;

        return ::operator new( bytes );
    }

    void do_deallocate( void* p, std::size_t bytes, std::size_t alignment )
    {
        do_deallocate_called = true;
        do_deallocate_p = p;
        do_deallocate_bytes = bytes;
        do_deallocate_alignment = alignment;

        ::operator delete( p );
    }

    bool do_is_equal( memory_resource const & /*other*/ ) const BOOST_NOEXCEPT
    {
        return true;
    }
};

struct R2: public boost::core::memory_resource
{
    void* do_allocate( std::size_t bytes, std::size_t /*alignment*/ )
    {
        return ::operator new( bytes );
    }

    void do_deallocate( void* p, std::size_t /*bytes*/, std::size_t /*alignment*/ )
    {
        ::operator delete( p );
    }

    bool do_is_equal( memory_resource const & other ) const BOOST_NOEXCEPT
    {
        return this == &other;
    }
};

int main()
{
    {
        R1 r1;

        do_allocate_called = false;
        do_allocate_bytes = 0;
        do_allocate_alignment = 0;

        void* p = r1.allocate( 31 );

        BOOST_TEST( do_allocate_called );
        BOOST_TEST_EQ( do_allocate_bytes, 31 );
        BOOST_TEST_EQ( do_allocate_alignment, boost::core::max_align );

        do_deallocate_called = false;
        do_deallocate_p = 0;
        do_deallocate_bytes = 0;
        do_deallocate_alignment = 0;

        r1.deallocate( p, 31 );

        BOOST_TEST( do_deallocate_called );
        BOOST_TEST_EQ( do_deallocate_p, p );
        BOOST_TEST_EQ( do_deallocate_bytes, 31 );
        BOOST_TEST_EQ( do_deallocate_alignment, boost::core::max_align );
    }

    {
        R1 r1;

        do_allocate_called = false;
        do_allocate_bytes = 0;
        do_allocate_alignment = 0;

        void* p = r1.allocate( 1, 8 );

        BOOST_TEST( do_allocate_called );
        BOOST_TEST_EQ( do_allocate_bytes, 1 );
        BOOST_TEST_EQ( do_allocate_alignment, 8 );

        do_deallocate_called = false;
        do_deallocate_p = 0;
        do_deallocate_bytes = 0;
        do_deallocate_alignment = 0;

        r1.deallocate( p, 1, 8 );

        BOOST_TEST( do_deallocate_called );
        BOOST_TEST_EQ( do_deallocate_p, p );
        BOOST_TEST_EQ( do_deallocate_bytes, 1 );
        BOOST_TEST_EQ( do_deallocate_alignment, 8 );
    }

    {
        R1 r1;
        R1 r2;

        BOOST_TEST( r1 == r1 );
        BOOST_TEST_NOT( r1 != r1 );

        BOOST_TEST( r1 == r2 );
        BOOST_TEST_NOT( r1 != r2 );
    }

    {
        R2 r1;
        R2 r2;

        BOOST_TEST( r1 == r1 );
        BOOST_TEST_NOT( r1 != r1 );

        BOOST_TEST_NOT( r1 == r2 );
        BOOST_TEST( r1 != r2 );
    }

    return boost::report_errors();
}
