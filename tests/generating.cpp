#include <ctre.hpp>

using namespace ctre::test_literals;

template <typename... T> struct id_type;

template <typename... T> constexpr auto id(T...) {
	return id_type<T...>{};
};

#define ID(expr) auto i = id(ctll::front(expr));
#define ID_ALL(expr) auto i = id(expr);

template <typename A, typename B> constexpr auto same(A,B) -> std::false_type { return {}; }

template <typename A> constexpr auto same(A,A) -> std::true_type { return {}; }

template <typename A, typename B> constexpr bool same_f(A a,B b) {
	return same(ctll::front(a), b);
}

// basics
static_assert(same_f("y"_pcre_gen, ctre::character<'y'>()));

// stringification
static_assert(ctll::size("abc"_pcre_gen) == 1);
static_assert(same_f("abc"_pcre_gen, ctre::string<'a','b','c'>()));
static_assert(same_f("(?:abc)"_pcre_gen, ctre::string<'a','b','c'>()));

// support for hexdec
static_assert(same_f("\\xFF"_pcre_gen, ctre::character<'\xFF'>()));
static_assert(same_f("\\x{FF}"_pcre_gen, ctre::character<'\xFF'>()));
static_assert(same_f("\\x{FFF}"_pcre_gen, ctre::character<(wchar_t)0xFFF>()));
static_assert(same_f("\\x{ABCD}"_pcre_gen, ctre::character<(wchar_t)0xABCD>()));

// anything
static_assert(same_f("."_pcre_gen, ctre::any()));

// sequence
static_assert(same_f("x.ab"_pcre_gen, ctre::sequence<ctre::character<'x'>,ctre::any,ctre::string<'a','b'>>()));

// character class
static_assert(same_f("\\d"_pcre_gen, ctre::set<ctre::digit_chars>()));
static_assert(same_f("\\D"_pcre_gen, ctre::negative_set<ctre::digit_chars>()));
static_assert(same_f("\\n"_pcre_gen, ctre::character<'\n'>()));
static_assert(same_f("\\N"_pcre_gen, ctre::negative_set<ctre::character<'\n'>>()));
static_assert(same_f("\\w"_pcre_gen, ctre::set<ctre::word_chars>()));
static_assert(same_f("\\W"_pcre_gen, ctre::negative_set<ctre::word_chars>()));
static_assert(same_f("\\s"_pcre_gen, ctre::set<ctre::space_chars>()));
static_assert(same_f("\\S"_pcre_gen, ctre::negative_set<ctre::space_chars>()));

// set support
static_assert(same_f("[a]"_pcre_gen, ctre::set<ctre::character<'a'>>()));
static_assert(same_f("[^a]"_pcre_gen, ctre::negative_set<ctre::character<'a'>>()));
static_assert(same_f("[[:digit:]]"_pcre_gen, ctre::set<ctre::digit_chars>()));
static_assert(same_f("[ab]"_pcre_gen, ctre::set<ctre::character<'a'>,ctre::character<'b'>>()));
static_assert(same_f("[^ab]"_pcre_gen, ctre::negative_set<ctre::character<'a'>,ctre::character<'b'>>()));
static_assert(same_f("[[:digit:][:digit:]]"_pcre_gen, ctre::set<ctre::digit_chars>()));
static_assert(same_f("[^[:digit:]]"_pcre_gen, ctre::negative_set<ctre::digit_chars>()));
static_assert(same_f("[[:^digit:]]"_pcre_gen, ctre::set<ctre::negate<ctre::digit_chars>>()));
static_assert(same_f("[[:^digit:][:^alpha:]]"_pcre_gen, ctre::set<ctre::negate<ctre::digit_chars>, ctre::negate<ctre::alpha_chars>>()));
static_assert(same_f("[[:digit:][:alpha:]]"_pcre_gen, ctre::set<ctre::digit_chars, ctre::alpha_chars>()));
static_assert(same_f("[[:digit:][:^alpha:]]"_pcre_gen, ctre::set<ctre::digit_chars, ctre::negate<ctre::alpha_chars>>()));

// alternation
static_assert(same_f("(?:abc|def)"_pcre_gen, ctre::select<ctre::string<'a','b','c'>,ctre::string<'d','e','f'>>()));
static_assert(same_f("(?:abc|def|ghi)"_pcre_gen, ctre::select<ctre::string<'a','b','c'>,ctre::string<'d','e','f'>,ctre::string<'g','h','i'>>()));
static_assert(same_f("(?:a|b|c|d)"_pcre_gen, ctre::select<ctre::character<'a'>,ctre::character<'b'>,ctre::character<'c'>,ctre::character<'d'>>()));
static_assert(same_f("(?:a|b|c|)"_pcre_gen, ctre::select<ctre::character<'a'>,ctre::character<'b'>,ctre::character<'c'>,ctre::empty>()));

// optional
static_assert(same_f("xx?"_pcre_gen, ctre::sequence<ctre::character<'x'>,ctre::optional<ctre::character<'x'>>>()));
static_assert(same_f("(?:abc)?"_pcre_gen, ctre::optional<ctre::string<'a','b','c'>>()));

// repeat
static_assert(same_f("x+"_pcre_gen, ctre::plus<ctre::character<'x'>>()));
static_assert(same_f("(?:abc)+"_pcre_gen, ctre::plus<ctre::string<'a','b','c'>>()));
static_assert(same_f("x*"_pcre_gen, ctre::star<ctre::character<'x'>>()));
static_assert(same_f("(?:abc)*"_pcre_gen, ctre::star<ctre::string<'a','b','c'>>()));
static_assert(same_f("x{1}"_pcre_gen, ctre::repeat<1,1,ctre::character<'x'>>()));
static_assert(same_f("x{1,}"_pcre_gen, ctre::repeat<1,0,ctre::character<'x'>>()));
static_assert(same_f("x{198}"_pcre_gen, ctre::repeat<198,198,ctre::character<'x'>>()));
static_assert(same_f("x{198,}"_pcre_gen, ctre::repeat<198,0,ctre::character<'x'>>()));
static_assert(same_f("x{1,2}"_pcre_gen, ctre::repeat<1,2,ctre::character<'x'>>()));
static_assert(same_f("x{1,1234}"_pcre_gen, ctre::repeat<1,1234,ctre::character<'x'>>()));
static_assert(same_f("x{567,1234}"_pcre_gen, ctre::repeat<567,1234,ctre::character<'x'>>()));
static_assert(same_f("[x]{567,1234}"_pcre_gen, ctre::repeat<567,1234,ctre::set<ctre::character<'x'>>>()));
static_assert(same_f("[^x]{567,1234}"_pcre_gen, ctre::repeat<567,1234,ctre::negative_set<ctre::character<'x'>>>()));
static_assert(same_f("(?:abc){3,42}"_pcre_gen, ctre::repeat<3,42,ctre::string<'a','b','c'>>()));

//auto i = "(?:abc){1,2}"_pcre_gen;