#include <ctre.hpp>

using namespace ctre::test_literals;

#ifdef DEBUG

template <typename... T> struct id_type;

template <typename... T> constexpr auto id(T...) {
	return id_type<T...>{};
};

#define ID(expr) auto i = id(ctll::front(expr));
#define ID_ALL(expr) auto i = id(expr);

#endif // DEBUG

template <typename A, typename B> constexpr auto same(A,B) -> std::false_type { return {}; }

template <typename A> constexpr auto same(A,A) -> std::true_type { return {}; }

template <typename A, typename B> constexpr bool same_f(A a,B b) {
	return same(ctll::front(a), b);
}

// basics
static_assert(same_f(""_ctre_gen, ctre::empty()));
static_assert(same_f("y"_ctre_gen, ctre::character<'y'>()));

// stringification
static_assert(ctll::size("abc"_ctre_gen) == 1);
static_assert(same_f("abc"_ctre_gen, ctre::string<'a','b','c'>()));
static_assert(same_f("(?:abc)"_ctre_gen, ctre::string<'a','b','c'>()));

// support for hexdec
static_assert(same_f("\\xFF"_ctre_gen, ctre::character<'\xFF'>()));
static_assert(same_f("\\x{FF}"_ctre_gen, ctre::character<'\xFF'>()));
static_assert(same_f("\\x{FFF}"_ctre_gen, ctre::character<(size_t)0xFFF>()));
static_assert(same_f("\\x{ABCD}"_ctre_gen, ctre::character<(size_t)0xABCD>()));

// anything
static_assert(same_f("."_ctre_gen, ctre::any()));

// sequence
static_assert(same_f("x.ab"_ctre_gen, ctre::sequence<ctre::character<'x'>,ctre::any,ctre::string<'a','b'>>()));

// character class
static_assert(same_f("\\d"_ctre_gen, ctre::set<ctre::digit_chars>()));
static_assert(same_f("\\D"_ctre_gen, ctre::negative_set<ctre::digit_chars>()));
static_assert(same_f("\\n"_ctre_gen, ctre::character<'\n'>()));
static_assert(same_f("\\N"_ctre_gen, ctre::negative_set<ctre::character<'\n'>>()));
static_assert(same_f("\\w"_ctre_gen, ctre::set<ctre::word_chars>()));
static_assert(same_f("\\W"_ctre_gen, ctre::negative_set<ctre::word_chars>()));
static_assert(same_f("\\s"_ctre_gen, ctre::set<ctre::space_chars>()));
static_assert(same_f("\\S"_ctre_gen, ctre::negative_set<ctre::space_chars>()));

// set support
static_assert(same_f("[a]"_ctre_gen, ctre::set<ctre::character<'a'>>()));
static_assert(same_f("[^a]"_ctre_gen, ctre::negative_set<ctre::character<'a'>>()));
static_assert(same_f("[[:digit:]]"_ctre_gen, ctre::set<ctre::digit_chars>()));
static_assert(same_f("[ab]"_ctre_gen, ctre::set<ctre::character<'a'>,ctre::character<'b'>>()));
static_assert(same_f("[^ab]"_ctre_gen, ctre::negative_set<ctre::character<'a'>,ctre::character<'b'>>()));
static_assert(same_f("[[:digit:][:digit:]]"_ctre_gen, ctre::set<ctre::digit_chars>()));
static_assert(same_f("[^[:punct:]]"_ctre_gen, ctre::negative_set<ctre::punct_chars>()));
static_assert(same_f("[[:^digit:]]"_ctre_gen, ctre::set<ctre::negate<ctre::digit_chars>>()));
static_assert(same_f("[[:^digit:][:^alpha:]]"_ctre_gen, ctre::set<ctre::negate<ctre::digit_chars>, ctre::negate<ctre::alpha_chars>>()));
static_assert(same_f("[[:digit:][:alpha:]]"_ctre_gen, ctre::set<ctre::digit_chars, ctre::alpha_chars>()));
static_assert(same_f("[[:digit:][:^alpha:]]"_ctre_gen, ctre::set<ctre::digit_chars, ctre::negate<ctre::alpha_chars>>()));
static_assert(same_f("[a-z]"_ctre_gen, ctre::set<ctre::char_range<'a','z'>>()));
static_assert(same_f("[a-z0-9]"_ctre_gen, ctre::set<ctre::char_range<'a','z'>,ctre::char_range<'0','9'>>()));
static_assert(same_f("[^a-z]"_ctre_gen, ctre::negative_set<ctre::char_range<'a','z'>>()));
static_assert(same_f("[^a-z0-9]"_ctre_gen, ctre::negative_set<ctre::char_range<'a','z'>,ctre::char_range<'0','9'>>()));
static_assert(same_f("[a-z[:digit:]]"_ctre_gen, ctre::set<ctre::char_range<'a','z'>,ctre::digit_chars>()));
static_assert(same_f("[a-z98]"_ctre_gen, ctre::set<ctre::char_range<'a','z'>,ctre::character<'9'>,ctre::character<'8'>>()));
static_assert(same_f("[\\w]"_ctre_gen, ctre::set<ctre::set<ctre::word_chars>>()));
static_assert(same_f("[\\x30-\\x39]"_ctre_gen, ctre::set<ctre::char_range<'\x30','\x39'>>()));

// alternation
static_assert(same_f("(?:abc|def)"_ctre_gen, ctre::select<ctre::string<'a','b','c'>,ctre::string<'d','e','f'>>()));
static_assert(same_f("(?:abc|def|ghi)"_ctre_gen, ctre::select<ctre::string<'a','b','c'>,ctre::string<'d','e','f'>,ctre::string<'g','h','i'>>()));
static_assert(same_f("(?:a|b|c|d)"_ctre_gen, ctre::select<ctre::character<'a'>,ctre::character<'b'>,ctre::character<'c'>,ctre::character<'d'>>()));
static_assert(same_f("(?:a|b|c|)"_ctre_gen, ctre::select<ctre::character<'a'>,ctre::character<'b'>,ctre::character<'c'>,ctre::empty>()));

// optional
static_assert(same_f("xx?"_ctre_gen, ctre::sequence<ctre::character<'x'>,ctre::optional<ctre::character<'x'>>>()));
static_assert(same_f("(?:abc)?"_ctre_gen, ctre::optional<ctre::string<'a','b','c'>>()));

// repeat
static_assert(same_f("x+"_ctre_gen, ctre::plus<ctre::character<'x'>>()));
static_assert(same_f("(?:abc)+"_ctre_gen, ctre::plus<ctre::string<'a','b','c'>>()));
static_assert(same_f("x*"_ctre_gen, ctre::star<ctre::character<'x'>>()));
static_assert(same_f("(?:abc)*"_ctre_gen, ctre::star<ctre::string<'a','b','c'>>()));
static_assert(same_f("x{1}"_ctre_gen, ctre::repeat<1,1,ctre::character<'x'>>()));
static_assert(same_f("x{1,}"_ctre_gen, ctre::repeat<1,0,ctre::character<'x'>>()));
static_assert(same_f("x{198}"_ctre_gen, ctre::repeat<198,198,ctre::character<'x'>>()));
static_assert(same_f("x{198,}"_ctre_gen, ctre::repeat<198,0,ctre::character<'x'>>()));
static_assert(same_f("x{1,2}"_ctre_gen, ctre::repeat<1,2,ctre::character<'x'>>()));
static_assert(same_f("x{1,1234}"_ctre_gen, ctre::repeat<1,1234,ctre::character<'x'>>()));
static_assert(same_f("x{567,1234}"_ctre_gen, ctre::repeat<567,1234,ctre::character<'x'>>()));
static_assert(same_f("[x]{567,1234}"_ctre_gen, ctre::repeat<567,1234,ctre::set<ctre::character<'x'>>>()));
static_assert(same_f("[^x]{567,1234}"_ctre_gen, ctre::repeat<567,1234,ctre::negative_set<ctre::character<'x'>>>()));
static_assert(same_f("(?:abc){3,42}"_ctre_gen, ctre::repeat<3,42,ctre::string<'a','b','c'>>()));

// possessive/lazy
static_assert(same_f("c++"_ctre_gen, ctre::possessive_plus<ctre::character<'c'>>()));
static_assert(same_f("c+?"_ctre_gen, ctre::lazy_plus<ctre::character<'c'>>()));
static_assert(same_f("g*+"_ctre_gen, ctre::possessive_star<ctre::character<'g'>>()));
static_assert(same_f("g*?"_ctre_gen, ctre::lazy_star<ctre::character<'g'>>()));
static_assert(same_f("i??"_ctre_gen, ctre::lazy_optional<ctre::character<'i'>>()));
static_assert(same_f("l{1,2}+"_ctre_gen, ctre::possessive_repeat<1,2,ctre::character<'l'>>()));
static_assert(same_f("l{1,2}?"_ctre_gen, ctre::lazy_repeat<1,2,ctre::character<'l'>>()));
static_assert(same_f("q{4,4}+"_ctre_gen, ctre::possessive_repeat<4,4,ctre::character<'q'>>()));
static_assert(same_f("q{4,4}?"_ctre_gen, ctre::lazy_repeat<4,4,ctre::character<'q'>>()));
// note: there is no possessive/lazy evaluationg for fixed number of repeats

// captures
static_assert(same_f("(a)"_ctre_gen, ctre::capture<1,ctre::character<'a'>>()));
static_assert(same_f("(x[cd])"_ctre_gen, ctre::capture<1,ctre::character<'x'>, ctre::set<ctre::character<'c'>, ctre::character<'d'>>>())); 
static_assert(same_f("(x[cd])(ab)"_ctre_gen, ctre::sequence<ctre::capture<1,ctre::character<'x'>, ctre::set<ctre::character<'c'>, ctre::character<'d'>>>,ctre::capture<2,ctre::string<'a','b'>>>())); 
static_assert(same_f("(x[cd])(ab)+"_ctre_gen, ctre::sequence<ctre::capture<1,ctre::character<'x'>, ctre::set<ctre::character<'c'>, ctre::character<'d'>>>,ctre::plus<ctre::capture<2,ctre::string<'a','b'>>>>())); 
static_assert(same_f("(?<n>x)"_ctre_gen, ctre::capture_with_name<1,ctre::id<'n'>,ctre::character<'x'>>())); 
static_assert(same_f("(?<name>x)"_ctre_gen, ctre::capture_with_name<1,ctre::id<'n','a','m','e'>,ctre::character<'x'>>())); 
static_assert(same_f("(?<name>xy)"_ctre_gen, ctre::capture_with_name<1,ctre::id<'n','a','m','e'>,ctre::string<'x','y'>>())); 
static_assert(same_f("(?<name>x|y)"_ctre_gen, ctre::capture_with_name<1,ctre::id<'n','a','m','e'>,ctre::select<ctre::character<'x'>,ctre::character<'y'>>>())); 
static_assert(same_f("(?<xy>[x]y)"_ctre_gen, ctre::capture_with_name<1,ctre::id<'x','y'>,ctre::set<ctre::character<'x'>>,ctre::character<'y'>>())); 
static_assert(same_f("(?<xy>[x]y)(a)"_ctre_gen, ctre::sequence<ctre::capture_with_name<1,ctre::id<'x','y'>,ctre::set<ctre::character<'x'>>,ctre::character<'y'>>, ctre::capture<2,ctre::character<'a'>>>())); 

static_assert(same_f("()"_ctre_gen, ctre::capture<1,ctre::empty>()));
static_assert(same_f("(a)(b)"_ctre_gen, ctre::sequence<ctre::capture<1,ctre::character<'a'>>,ctre::capture<2,ctre::character<'b'>>>()));
static_assert(same_f("((a)(b))"_ctre_gen, ctre::capture<1,ctre::capture<2,ctre::character<'a'>>,ctre::capture<3,ctre::character<'b'>>>()));
static_assert(same_f("(((a)(b)))"_ctre_gen,ctre::capture<1, ctre::capture<2,ctre::capture<3,ctre::character<'a'>>,ctre::capture<4,ctre::character<'b'>>>>()));

static_assert(same_f("((?:a)(b))"_ctre_gen, ctre::capture<1,ctre::character<'a'>,ctre::capture<2,ctre::character<'b'>>>()));


// backreference
static_assert(same_f("(a)\\g{1}"_ctre_gen, ctre::sequence<ctre::capture<1,ctre::character<'a'>>, ctre::back_reference<1>>()));
static_assert(same_f("(?<ab>a)\\g{ab}"_ctre_gen, ctre::sequence<ctre::capture_with_name<1,ctre::id<'a','b'>,ctre::character<'a'>>, ctre::back_reference_with_name<ctre::id<'a','b'>>>()));
static_assert(same_f("((a)\\g{2})"_ctre_gen, ctre::capture<1, ctre::capture<2,ctre::character<'a'>>, ctre::back_reference<2>>()));

// asserts
static_assert(same_f("^"_ctre_gen, ctre::assert_begin()));
static_assert(same_f("$"_ctre_gen, ctre::assert_end()));
static_assert(same_f("^$"_ctre_gen, ctre::sequence<ctre::assert_begin, ctre::assert_end>()));

// TODO change this to string
static_assert(same_f("^abc$"_ctre_gen, ctre::sequence<ctre::assert_begin, ctre::character<'a'>,ctre::character<'b'>,ctre::character<'c'>, ctre::assert_end>()));

static_assert(same_f("^a|b$"_ctre_gen, ctre::select<ctre::sequence<ctre::assert_begin, ctre::character<'a'>>, ctre::sequence<ctre::character<'b'>, ctre::assert_end>>()));

// lookahead positive/negative
static_assert(same_f("(?=a)"_ctre_gen, ctre::lookahead_positive<ctre::character<'a'>>()));
static_assert(same_f("(?=ax)"_ctre_gen, ctre::lookahead_positive<ctre::string<'a','x'>>()));
static_assert(same_f("(?=[a]x)"_ctre_gen, ctre::lookahead_positive<ctre::set<ctre::character<'a'>>,ctre::character<'x'>>()));

static_assert(same_f("(?!a)"_ctre_gen, ctre::lookahead_negative<ctre::character<'a'>>()));
static_assert(same_f("(?!ax)"_ctre_gen, ctre::lookahead_negative<ctre::string<'a','x'>>()));
static_assert(same_f("(?![a]x)"_ctre_gen, ctre::lookahead_negative<ctre::set<ctre::character<'a'>>,ctre::character<'x'>>()));

static_assert(same_f("^(?=(a))$"_ctre_gen, ctre::sequence<ctre::assert_begin, ctre::lookahead_positive<ctre::capture<1,ctre::character<'a'>>>, ctre::assert_end>()));

static_assert(same_f("^(?=.*(a))$"_ctre_gen, ctre::sequence<ctre::assert_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>>, ctre::assert_end>()));

static_assert(same_f("^(?=.*(a).*)$"_ctre_gen, ctre::sequence<ctre::assert_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>, ctre::star<ctre::any>>, ctre::assert_end>()));

static_assert(same_f("^(?=.*(a)\\g{1}.*)$"_ctre_gen, ctre::sequence<ctre::assert_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>, ctre::back_reference<1>, ctre::star<ctre::any>>, ctre::assert_end>()));

static_assert(same_f("^(?=.*(a)\\g{1}.*)[a-z]$"_ctre_gen, ctre::sequence<ctre::assert_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>, ctre::back_reference<1>, ctre::star<ctre::any>>, ctre::set<ctre::char_range<'a','z'>>, ctre::assert_end>()));


