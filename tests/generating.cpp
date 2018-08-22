#include <ctre.hpp>

using namespace ctre::test_literals;

template <typename... T> struct id_type;

template <typename... T> constexpr auto id(T...) {
	return id_type<T...>{};
};

template <typename A, typename B> constexpr auto same(A,B) -> std::false_type { return {}; }

template <typename A> constexpr auto same(A,A) -> std::true_type { return {}; }

// basics
static_assert(same(ctll::front("y"_pcre_gen), ctre::character<'y'>()));

// stringification
static_assert(ctll::size("abc"_pcre_gen) == 1);
static_assert(same(ctll::front("abc"_pcre_gen), ctre::string<'a','b','c'>()));

// support for hexdec
static_assert(same(ctll::front("\\xFF"_pcre_gen), ctre::character<'\xFF'>()));
static_assert(same(ctll::front("\\x{FF}"_pcre_gen), ctre::character<'\xFF'>()));
static_assert(same(ctll::front("\\x{FFF}"_pcre_gen), ctre::character<(wchar_t)0xFFF>()));
static_assert(same(ctll::front("\\x{ABCD}"_pcre_gen), ctre::character<(wchar_t)0xABCD>()));

// anything
static_assert(same(ctll::front("."_pcre_gen), ctre::any()));

// sequence
static_assert(same(ctll::front("x.ab"_pcre_gen), ctre::sequence<ctre::character<'x'>,ctre::any,ctre::string<'a','b'>>()));

// character class
static_assert(same(ctll::front("\\d"_pcre_gen), ctre::set<ctre::range<'0','9'>>()));
static_assert(same(ctll::front("\\D"_pcre_gen), ctre::negative_set<ctre::range<'0','9'>>()));
static_assert(same(ctll::front("\\n"_pcre_gen), ctre::character<'\n'>()));
static_assert(same(ctll::front("\\N"_pcre_gen), ctre::negative_set<ctre::character<'\n'>>()));
static_assert(same(ctll::front("\\w"_pcre_gen), ctre::set<ctre::word_chars>()));
static_assert(same(ctll::front("\\W"_pcre_gen), ctre::negative_set<ctre::word_chars>()));
static_assert(same(ctll::front("\\s"_pcre_gen), ctre::set<ctre::space_chars>()));
static_assert(same(ctll::front("\\S"_pcre_gen), ctre::negative_set<ctre::space_chars>()));
