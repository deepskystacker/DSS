// Copyright 2021 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/type_name.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/config.hpp>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <utility>
#include <cstddef>
#include <iosfwd>

#if !defined(BOOST_NO_CXX11_HDR_UNORDERED_SET)
# include <unordered_set>
#endif

#if !defined(BOOST_NO_CXX11_HDR_UNORDERED_MAP)
# include <unordered_map>
#endif

#if !defined(BOOST_NO_CXX11_HDR_ARRAY)
# include <array>
#endif

#if !defined(BOOST_NO_CXX11_HDR_TUPLE)
# include <tuple>
#endif

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
# include <string_view>
#endif

//

#define TEST(...) BOOST_TEST_EQ((boost::core::type_name<__VA_ARGS__>()), std::string(#__VA_ARGS__))

struct A
{
};

class B
{
};

class C;

template<class T1, class T2> struct X
{
};

template<class T1, class T2> struct Y;

class W
{
public:

    virtual void f() = 0;
};

enum E1
{
    e1
};

#if !defined(BOOST_NO_CXX11_SCOPED_ENUMS)

enum class E2
{
    e2
};

enum class E3;

#endif

struct Ch
{
};

int main()
{
    TEST(signed char);
    TEST(unsigned char);
    TEST(short);
    TEST(unsigned short);
    TEST(int);
    TEST(unsigned);
    TEST(long);
    TEST(unsigned long);
    TEST(long long);
    TEST(unsigned long long);

#if defined(BOOST_HAS_INT128)
    TEST(__int128);
    TEST(unsigned __int128);
#endif

    TEST(char);
    TEST(wchar_t);
#if !defined(BOOST_NO_CXX11_CHAR16_T)
    TEST(char16_t);
#endif
#if !defined(BOOST_NO_CXX11_CHAR16_T)
    TEST(char32_t);
#endif
#if defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
    TEST(char8_t);
#endif
#if defined(__cpp_lib_byte) && __cpp_lib_byte >= 201603L
    TEST(std::byte);
#endif

    TEST(bool);

    TEST(float);
    TEST(double);
    TEST(long double);

    TEST(void);
    TEST(void const);
    TEST(void volatile);
    TEST(void const volatile);

    TEST(A);
    TEST(B);
    TEST(C);
    TEST(W);

    TEST(E1);

#if !defined(BOOST_NO_CXX11_SCOPED_ENUMS)

    TEST(E2);
    TEST(E3);

#endif

    TEST(A const);
    TEST(A volatile);
    TEST(A const volatile);

    TEST(B&);
    TEST(B const&);

    TEST(C volatile);
    TEST(C&);
    TEST(C const&);

    TEST(W const);
    TEST(W&);

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    TEST(B&&);
    TEST(B const&&);

#endif

    TEST(A*);
    TEST(B const* volatile*);

    TEST(C*);
    TEST(C const* volatile*);

    TEST(W volatile*);

    TEST(void*);
    TEST(void const* volatile*);

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

    TEST(void());
    TEST(int(float, A, B*));

    TEST(void(*)());
    TEST(void(**)());
    TEST(void(***)());

    TEST(void(* const* const*)());
    TEST(void(* const* const&)());

    TEST(void(&)());

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    TEST(void(&&)());

#endif

#if !defined(BOOST_MSVC) || BOOST_MSVC >= 1900

    TEST(void() const);
    TEST(void() volatile);
    TEST(void() const volatile);

#endif

#if !defined(BOOST_NO_CXX11_REF_QUALIFIERS)

    TEST(void() &);
    TEST(void() const &);
    TEST(void() volatile &);
    TEST(void() const volatile &);

    TEST(void() &&);
    TEST(void() const &&);
    TEST(void() volatile &&);
    TEST(void() const volatile &&);

#endif

#if defined( __cpp_noexcept_function_type ) || defined( _NOEXCEPT_TYPES_SUPPORTED )

    TEST(void() noexcept);
    TEST(void() const noexcept);
    TEST(void() volatile noexcept);
    TEST(void() const volatile noexcept);

    TEST(void() & noexcept);
    TEST(void() const & noexcept);
    TEST(void() volatile & noexcept);
    TEST(void() const volatile & noexcept);

    TEST(void() && noexcept);
    TEST(void() const && noexcept);
    TEST(void() volatile && noexcept);
    TEST(void() const volatile && noexcept);

#endif

#endif // #if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

    TEST(A[]);
    TEST(A const[]);
    TEST(A volatile[]);
    TEST(A const volatile[]);

#if !defined(BOOST_MSVC) || BOOST_MSVC >= 1700
    TEST(A(&)[]);
#endif
    TEST(A const(***)[]);

    TEST(B[1]);
    TEST(B const[1]);
    TEST(B volatile[1]);
    TEST(B const volatile[1]);

    TEST(B(&)[1]);
    TEST(B const(***)[1]);

    TEST(A[][2][3]);
    TEST(A const[][2][3]);

#if !defined(BOOST_MSVC) || BOOST_MSVC >= 1700
    TEST(A(&)[][2][3]);
#endif
    TEST(A const(***)[][2][3]);

    TEST(B[1][2][3]);
    TEST(B const volatile[1][2][3]);

    TEST(B(&)[1][2][3]);
    TEST(B const volatile(***)[1][2][3]);

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || !defined(BOOST_MSVC)

    TEST(int A::*);
    TEST(int const B::*);

#else

    boost::core::type_name<int A::*>();
    boost::core::type_name<int const B::*>();

#endif

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

    TEST(void(A::*)());
    TEST(void(A::*)() const);
    TEST(void(A::*)() volatile);
    TEST(void(A::*)() const volatile);

#else

    boost::core::type_name<void(A::*)()>();
    boost::core::type_name<void(A::*)() const>();
    boost::core::type_name<void(A::*)() volatile>();
    boost::core::type_name<void(A::*)() const volatile>();

#endif

#if !defined(BOOST_NO_CXX11_REF_QUALIFIERS)

    TEST(void(A::*)() &);
    TEST(void(A::*)() const &&);

#endif

#if defined( __cpp_noexcept_function_type ) || defined( _NOEXCEPT_TYPES_SUPPORTED )

    TEST(void(A::*)() volatile & noexcept);
    TEST(void(A::*)() const volatile && noexcept);

#endif

#if !defined(BOOST_NO_CXX11_NULLPTR)

    TEST(std::nullptr_t);

#endif

    TEST(std::pair<A, B>);
    TEST(std::pair<A const*, B*> volatile&);

    TEST(std::pair<C, C>);

    TEST(std::pair<W, W>);

    TEST(std::pair<void, void>);
    TEST(std::pair<std::pair<void, void>, void>);

#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 160000
// std::char_traits<Ch> is deprecated for non-char types
#else
    TEST(std::basic_string<Ch>);
#endif

    TEST(std::string);
    TEST(std::wstring);

#if BOOST_CXX_VERSION >= 201100L

    TEST(std::u16string);
    TEST(std::u32string);

#endif

#if BOOST_CXX_VERSION >= 202000L

    TEST(std::u8string);

#endif

    TEST(X<A, B>);
    TEST(X<A const&, B&> volatile&);

    TEST(X<C, C>);
    TEST(X<W, W>);

    TEST(Y<A, B>);
    TEST(Y<C, C>);
    TEST(Y<W, W>);

    TEST(X<std::pair<void, void>, void>);

    TEST(std::vector<int>);
    TEST(std::vector<A>);
    TEST(std::vector<std::string>);

    TEST(std::list<int>);
    TEST(std::list<B>);
    TEST(std::list<std::wstring>);

    TEST(std::deque<int>);
    TEST(std::deque<A>);
    TEST(std::deque<std::string>);

    TEST(std::set<int>);
    TEST(std::set<B>);
    TEST(std::set<std::string>);

    TEST(std::map<int, A>);
    TEST(std::map<std::string, B>);
    TEST(std::map<std::wstring, std::vector<std::string> const*> const&);

#if !defined(BOOST_NO_CXX11_HDR_UNORDERED_SET)

    TEST(std::unordered_set<int>);
    TEST(std::unordered_set<std::string>);

#endif

#if !defined(BOOST_NO_CXX11_HDR_UNORDERED_MAP)

    TEST(std::unordered_map<int, A>);
    TEST(std::unordered_map<std::string, B>);
    TEST(std::unordered_map<std::wstring, std::set<std::string>*>);

#endif

#if !defined(BOOST_NO_CXX11_HDR_ARRAY)

    TEST(std::array<std::string, 7>);
    TEST(std::array<std::wstring const*, 0> const&);

#endif

#if !defined(BOOST_NO_CXX11_HDR_TUPLE) && ( !defined(BOOST_MSVC) || BOOST_MSVC >= 1700 )

    TEST(std::tuple<>);
    TEST(std::tuple<int>);
    TEST(std::tuple<int, float>);
    TEST(std::tuple<int, float, std::string>);

    TEST(std::tuple<void>);
    TEST(std::tuple<void, void>);
    TEST(std::tuple<void, void, void>);

    TEST(std::tuple<std::tuple<void, void>, void>);

    TEST(X<std::tuple<void>, void>);

#endif

    TEST(std::ostream);
    TEST(std::basic_ostream<wchar_t>);

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)

#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 160000
// std::char_traits<Ch> is deprecated for non-char types
#else
    TEST(std::basic_string_view<Ch>);
#endif

    TEST(std::string_view);
    TEST(std::wstring_view);

#if BOOST_CXX_VERSION >= 201100L

    TEST(std::u16string_view);
    TEST(std::u32string_view);

#endif

#if BOOST_CXX_VERSION >= 202000L

    TEST(std::u8string_view);

#endif

#endif

    return boost::report_errors();
}
