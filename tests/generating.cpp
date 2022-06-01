#include <ctre.hpp>

void empty_symbol() { }

template <typename... T> struct id_type;

using namespace ctre::test_literals;

#if !CTRE_CNTTP_COMPILER_CHECK
#define CTRE_GEN(pattern) (pattern ## _ctre_gen)
#else


template <ctll::fixed_string input> constexpr auto gen() {
	constexpr auto _input = input;
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<ctre::pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type::stack_type();
}


#define CTRE_GEN(pattern) gen<pattern>()

#endif

template <typename A, typename B> constexpr auto same(A,B) -> std::false_type { return {}; }

template <typename A> constexpr auto same(A,A) -> std::true_type { return {}; }

template <typename A, typename B> constexpr bool same_f(A a,B b) {
	return same(ctll::front(a), b);
}

// basics
static_assert(same_f(CTRE_GEN(""), ctre::empty()));
static_assert(same_f(CTRE_GEN("y"), ctre::character<'y'>()));

// unicode
#if __cpp_char8_t
static_assert(same_f(CTRE_GEN(u8"😍"), ctre::character<U'😍'>()));
static_assert(same_f(CTRE_GEN(u8"[😍]"), ctre::set<ctre::character<U'😍'>>()));
#endif
static_assert(same_f(CTRE_GEN(U"😍"), ctre::character<U'😍'>()));
static_assert(same_f(CTRE_GEN(U"[😍]"), ctre::set<ctre::character<U'😍'>>()));

// stringification
static_assert(ctll::size(CTRE_GEN("abc")) == 1);
static_assert(same_f(CTRE_GEN("abc"), ctre::string<'a','b','c'>()));
static_assert(same_f(CTRE_GEN("(?:abc)"), ctre::string<'a','b','c'>()));

// support for hexdec
static_assert(same_f(CTRE_GEN("\\x40"), ctre::character<char{0x40}>()));
static_assert(same_f(CTRE_GEN("\\x7F"), ctre::character<char{0x7F}>()));
// only characters with value < 128 are char otherwise they are internally char32_t
static_assert(same_f(CTRE_GEN("\\x80"), ctre::character<char32_t{0x80}>()));
static_assert(same_f(CTRE_GEN("\\xFF"), ctre::character<char32_t{0xFF}>()));
static_assert(same_f(CTRE_GEN("\\x{FF}"), ctre::character<char32_t{0xFF}>()));
static_assert(same_f(CTRE_GEN("\\x{FFF}"), ctre::character<char32_t{0xFFF}>()));
static_assert(same_f(CTRE_GEN("\\x{ABCD}"), ctre::character<char32_t{0xABCD}>()));

// anything
static_assert(same_f(CTRE_GEN("."), ctre::any()));

// sequence
static_assert(same_f(CTRE_GEN("x.ab"), ctre::sequence<ctre::character<'x'>,ctre::any,ctre::string<'a','b'>>()));

// character class
static_assert(same_f(CTRE_GEN("\\d"), ctre::set<ctre::digit_chars>()));
static_assert(same_f(CTRE_GEN("\\D"), ctre::negative_set<ctre::digit_chars>()));
static_assert(same_f(CTRE_GEN("\\n"), ctre::character<'\n'>()));
static_assert(same_f(CTRE_GEN("\\N"), ctre::negative_set<ctre::character<'\n'>>()));
static_assert(same_f(CTRE_GEN("\\w"), ctre::set<ctre::word_chars>()));
static_assert(same_f(CTRE_GEN("\\W"), ctre::negative_set<ctre::word_chars>()));
static_assert(same_f(CTRE_GEN("\\s"), ctre::set<ctre::space_chars>()));
static_assert(same_f(CTRE_GEN("\\S"), ctre::negative_set<ctre::space_chars>()));

// set support
static_assert(same_f(CTRE_GEN("[a]"), ctre::set<ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("[^a]"), ctre::negative_set<ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("[[:digit:]]"), ctre::set<ctre::digit_chars>()));
static_assert(same_f(CTRE_GEN("[ab]"), ctre::set<ctre::character<'a'>,ctre::character<'b'>>()));
static_assert(same_f(CTRE_GEN("[^ab]"), ctre::negative_set<ctre::character<'a'>,ctre::character<'b'>>()));
static_assert(same_f(CTRE_GEN("[[:digit:][:digit:]]"), ctre::set<ctre::digit_chars>()));
static_assert(same_f(CTRE_GEN("[^[:punct:]]"), ctre::negative_set<ctre::punct_chars>()));
static_assert(same_f(CTRE_GEN("[[:^digit:]]"), ctre::set<ctre::negate<ctre::digit_chars>>()));
static_assert(same_f(CTRE_GEN("[[:^digit:][:^alpha:]]"), ctre::set<ctre::negate<ctre::digit_chars>, ctre::negate<ctre::alpha_chars>>()));
static_assert(same_f(CTRE_GEN("[[:digit:][:alpha:]]"), ctre::set<ctre::digit_chars, ctre::alpha_chars>()));
static_assert(same_f(CTRE_GEN("[[:digit:][:^alpha:]]"), ctre::set<ctre::digit_chars, ctre::negate<ctre::alpha_chars>>()));
static_assert(same_f(CTRE_GEN("[a-z]"), ctre::set<ctre::char_range<'a','z'>>()));
static_assert(same_f(CTRE_GEN("[a-z0-9]"), ctre::set<ctre::char_range<'a','z'>,ctre::char_range<'0','9'>>()));
static_assert(same_f(CTRE_GEN("[^a-z]"), ctre::negative_set<ctre::char_range<'a','z'>>()));
static_assert(same_f(CTRE_GEN("[^a-z0-9]"), ctre::negative_set<ctre::char_range<'a','z'>,ctre::char_range<'0','9'>>()));
static_assert(same_f(CTRE_GEN("[a-z[:digit:]]"), ctre::set<ctre::char_range<'a','z'>,ctre::digit_chars>()));
static_assert(same_f(CTRE_GEN("[a-z98]"), ctre::set<ctre::char_range<'a','z'>,ctre::character<'9'>,ctre::character<'8'>>()));
static_assert(same_f(CTRE_GEN("[\\w]"), ctre::set<ctre::set<ctre::word_chars>>()));
static_assert(same_f(CTRE_GEN("[\\x30-\\x39]"), ctre::set<ctre::char_range<'\x30','\x39'>>()));

// alternation
static_assert(same_f(CTRE_GEN("(?:abc|def)"), ctre::select<ctre::string<'a','b','c'>,ctre::string<'d','e','f'>>()));
static_assert(same_f(CTRE_GEN("(?:abc|def|ghi)"), ctre::select<ctre::string<'a','b','c'>,ctre::string<'d','e','f'>,ctre::string<'g','h','i'>>()));
static_assert(same_f(CTRE_GEN("(?:a|b|c|d)"), ctre::select<ctre::character<'a'>,ctre::character<'b'>,ctre::character<'c'>,ctre::character<'d'>>()));
static_assert(same_f(CTRE_GEN("(?:a|b|c|)"), ctre::select<ctre::character<'a'>,ctre::character<'b'>,ctre::character<'c'>,ctre::empty>()));

// optional
static_assert(same_f(CTRE_GEN("xx?"), ctre::sequence<ctre::character<'x'>,ctre::optional<ctre::character<'x'>>>()));
static_assert(same_f(CTRE_GEN("xx?+"), ctre::sequence<ctre::character<'x'>,ctre::possessive_optional<ctre::character<'x'>>>()));
static_assert(same_f(CTRE_GEN("xx??"), ctre::sequence<ctre::character<'x'>,ctre::lazy_optional<ctre::character<'x'>>>()));
static_assert(same_f(CTRE_GEN("(?:abc)?"), ctre::optional<ctre::string<'a','b','c'>>()));
static_assert(same_f(CTRE_GEN("(?:x)?"), ctre::optional<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("(?:x?)?"), ctre::optional<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("(?:x?)??"), ctre::lazy_optional<ctre::character<'x'>>()));
// the string is split due trigraph warning
static_assert(same_f(CTRE_GEN("(?:x??" ")?"), ctre::lazy_optional<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("(?:x??" ")??"), ctre::lazy_optional<ctre::character<'x'>>()));

// repeat
static_assert(same_f(CTRE_GEN("x+"), ctre::plus<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("(?:abc)+"), ctre::plus<ctre::string<'a','b','c'>>()));
static_assert(same_f(CTRE_GEN("x*"), ctre::star<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("(?:abc)*"), ctre::star<ctre::string<'a','b','c'>>()));
static_assert(same_f(CTRE_GEN("x{1}"), ctre::repeat<1,1,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{1,}"), ctre::repeat<1,0,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{198}"), ctre::repeat<198,198,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{198,}"), ctre::repeat<198,0,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{1,2}"), ctre::repeat<1,2,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{1,1234}"), ctre::repeat<1,1234,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{567,1234}"), ctre::repeat<567,1234,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("[x]{567,1234}"), ctre::repeat<567,1234,ctre::set<ctre::character<'x'>>>()));
static_assert(same_f(CTRE_GEN("[^x]{567,1234}"), ctre::repeat<567,1234,ctre::negative_set<ctre::character<'x'>>>()));
static_assert(same_f(CTRE_GEN("(?:abc){3,42}"), ctre::repeat<3,42,ctre::string<'a','b','c'>>()));

// combine repeat
static_assert(same_f(CTRE_GEN("x*x*"), ctre::star<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x+x+"), ctre::repeat<2,0,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x+x*"), ctre::plus<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x*x+"), ctre::plus<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{1,3}x{2,4}"), ctre::repeat<3,7,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("a?a?a?"), ctre::repeat<0,3,ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a?a?b"), ctre::sequence<ctre::repeat<0,2,ctre::character<'a'>>, ctre::character<'b'>>()));
static_assert(same(CTRE_GEN("a?a?b"), CTRE_GEN("(?:a?a?)b")));

// combine lazy repeat
static_assert(same_f(CTRE_GEN("x*?x*?"), ctre::lazy_star<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x+?x+?"), ctre::lazy_repeat<2,0,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x+?x*?"), ctre::lazy_plus<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x*?x+?"), ctre::lazy_plus<ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("x{1,3}?x{2,4}?"), ctre::lazy_repeat<3,7,ctre::character<'x'>>()));
static_assert(same_f(CTRE_GEN("a??a??a??"), ctre::lazy_repeat<0,3,ctre::character<'a'>>()));

// combine possessive
static_assert(same_f(CTRE_GEN("a{0,0}+a{0,0}+"), ctre::possessive_repeat<0,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,0}+a{0,0}+"), ctre::possessive_repeat<1,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{0,0}+a{1,0}+"), ctre::sequence<ctre::reject, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,0}+a{1,0}+"), ctre::sequence<ctre::reject, ctre::character<'a'>>()));

static_assert(same_f(CTRE_GEN("a{0,1}+a{0,0}+"), ctre::possessive_repeat<0,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,1}+a{0,0}+"), ctre::possessive_repeat<1,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{0,1}+a{1,0}+"), ctre::possessive_repeat<2,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,1}+a{1,0}+"), ctre::possessive_repeat<2,0, ctre::character<'a'>>()));

static_assert(same_f(CTRE_GEN("a{0,0}+a{0,1}+"), ctre::possessive_repeat<0,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,0}+a{0,1}+"), ctre::possessive_repeat<1,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{0,0}+a{1,1}+"), ctre::sequence<ctre::reject, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,0}+a{1,1}+"), ctre::sequence<ctre::reject, ctre::character<'a'>>()));

static_assert(same_f(CTRE_GEN("a{0,1}+a{0,1}+"), ctre::possessive_repeat<0,2, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,1}+a{0,1}+"), ctre::possessive_repeat<1,2, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{0,1}+a{1,1}+"), ctre::possessive_repeat<2,2, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,1}+a{1,1}+"), ctre::possessive_repeat<2,2, ctre::character<'a'>>()));

static_assert(same_f(CTRE_GEN("a{1,2}+a{2,0}+"), ctre::possessive_repeat<4,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,2}+a{1,2}+"), ctre::possessive_repeat<3,4, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,1}+a{2,0}+"), ctre::possessive_repeat<3,0, ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("a{1,1}+a{1,2}+"), ctre::possessive_repeat<2,3, ctre::character<'a'>>()));


// don't combine different types of repeats
static_assert(same_f(CTRE_GEN("x+?x+"), ctre::sequence<ctre::lazy_plus<ctre::character<'x'>>, ctre::plus<ctre::character<'x'>>>()));


// possessive/lazy
static_assert(same_f(CTRE_GEN("c++"), ctre::possessive_plus<ctre::character<'c'>>()));
static_assert(same_f(CTRE_GEN("c+?"), ctre::lazy_plus<ctre::character<'c'>>()));
static_assert(same_f(CTRE_GEN("g*+"), ctre::possessive_star<ctre::character<'g'>>()));
static_assert(same_f(CTRE_GEN("g*?"), ctre::lazy_star<ctre::character<'g'>>()));
static_assert(same_f(CTRE_GEN("i??"), ctre::lazy_optional<ctre::character<'i'>>()));
static_assert(same_f(CTRE_GEN("l{1,2}+"), ctre::possessive_repeat<1,2,ctre::character<'l'>>()));
static_assert(same_f(CTRE_GEN("l{1,2}?"), ctre::lazy_repeat<1,2,ctre::character<'l'>>()));
static_assert(same_f(CTRE_GEN("q{4,4}+"), ctre::possessive_repeat<4,4,ctre::character<'q'>>()));
static_assert(same_f(CTRE_GEN("q{4,4}?"), ctre::lazy_repeat<4,4,ctre::character<'q'>>()));
// note: there is no possessive/lazy evaluationg for fixed number of repeats

// captures
static_assert(same_f(CTRE_GEN("(a)"), ctre::capture<1,ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("(x[cd])"), ctre::capture<1,ctre::character<'x'>, ctre::set<ctre::character<'c'>, ctre::character<'d'>>>())); 
static_assert(same_f(CTRE_GEN("(x[cd])(ab)"), ctre::sequence<ctre::capture<1,ctre::character<'x'>, ctre::set<ctre::character<'c'>, ctre::character<'d'>>>,ctre::capture<2,ctre::string<'a','b'>>>())); 
static_assert(same_f(CTRE_GEN("(x[cd])(ab)+"), ctre::sequence<ctre::capture<1,ctre::character<'x'>, ctre::set<ctre::character<'c'>, ctre::character<'d'>>>,ctre::plus<ctre::capture<2,ctre::string<'a','b'>>>>())); 
static_assert(same_f(CTRE_GEN("(?<n>x)"), ctre::capture_with_name<1,ctre::id<'n'>,ctre::character<'x'>>())); 
static_assert(same_f(CTRE_GEN("(?<name>x)"), ctre::capture_with_name<1,ctre::id<'n','a','m','e'>,ctre::character<'x'>>())); 
static_assert(same_f(CTRE_GEN("(?<name>xy)"), ctre::capture_with_name<1,ctre::id<'n','a','m','e'>,ctre::string<'x','y'>>())); 
static_assert(same_f(CTRE_GEN("(?<name>x|y)"), ctre::capture_with_name<1,ctre::id<'n','a','m','e'>,ctre::select<ctre::character<'x'>,ctre::character<'y'>>>())); 
static_assert(same_f(CTRE_GEN("(?<xy>[x]y)"), ctre::capture_with_name<1,ctre::id<'x','y'>,ctre::set<ctre::character<'x'>>,ctre::character<'y'>>())); 
static_assert(same_f(CTRE_GEN("(?<xy>[x]y)(a)"), ctre::sequence<ctre::capture_with_name<1,ctre::id<'x','y'>,ctre::set<ctre::character<'x'>>,ctre::character<'y'>>, ctre::capture<2,ctre::character<'a'>>>())); 

static_assert(same_f(CTRE_GEN("()"), ctre::capture<1,ctre::empty>()));
static_assert(same_f(CTRE_GEN("(a)(b)"), ctre::sequence<ctre::capture<1,ctre::character<'a'>>,ctre::capture<2,ctre::character<'b'>>>()));
static_assert(same_f(CTRE_GEN("((a)(b))"), ctre::capture<1,ctre::capture<2,ctre::character<'a'>>,ctre::capture<3,ctre::character<'b'>>>()));
static_assert(same_f(CTRE_GEN("(((a)(b)))"),ctre::capture<1, ctre::capture<2,ctre::capture<3,ctre::character<'a'>>,ctre::capture<4,ctre::character<'b'>>>>()));

static_assert(same_f(CTRE_GEN("((?:a)(b))"), ctre::capture<1,ctre::character<'a'>,ctre::capture<2,ctre::character<'b'>>>()));


// backreference
static_assert(same_f(CTRE_GEN("(a)\\g{1}"), ctre::sequence<ctre::capture<1,ctre::character<'a'>>, ctre::back_reference<1>>()));
static_assert(same_f(CTRE_GEN("(?<ab>a)\\g{ab}"), ctre::sequence<ctre::capture_with_name<1,ctre::id<'a','b'>,ctre::character<'a'>>, ctre::back_reference_with_name<ctre::id<'a','b'>>>()));
static_assert(same_f(CTRE_GEN("((a)\\g{2})"), ctre::capture<1, ctre::capture<2,ctre::character<'a'>>, ctre::back_reference<2>>()));

// asserts
static_assert(same_f(CTRE_GEN("^"), ctre::assert_line_begin()));
static_assert(same_f(CTRE_GEN("$"), ctre::assert_line_end()));
static_assert(same_f(CTRE_GEN("^$"), ctre::sequence<ctre::assert_line_begin, ctre::assert_line_end>()));
static_assert(same_f(CTRE_GEN("\\A"), ctre::assert_subject_begin()));
static_assert(same_f(CTRE_GEN("\\Z"), ctre::assert_subject_end_line()));
static_assert(same_f(CTRE_GEN("\\z"), ctre::assert_subject_end()));

static_assert(same_f(CTRE_GEN("^abc$"), ctre::sequence<ctre::assert_line_begin, ctre::string<'a','b','c'>, ctre::assert_line_end>()));
static_assert(same_f(CTRE_GEN("abc?"), ctre::sequence<ctre::string<'a','b'>, ctre::optional<ctre::character<'c'>>>()));
static_assert(same_f(CTRE_GEN("abc."), ctre::sequence<ctre::string<'a','b','c'>, ctre::any>()));
static_assert(same_f(CTRE_GEN("abc.?"), ctre::sequence<ctre::string<'a','b','c'>, ctre::optional<ctre::any>>()));
static_assert(same_f(CTRE_GEN("abc.def"), ctre::sequence<ctre::string<'a','b','c'>, ctre::any, ctre::string<'d','e','f'>>()));

static_assert(same_f(CTRE_GEN("^a|b$"), ctre::select<ctre::sequence<ctre::assert_line_begin, ctre::character<'a'>>, ctre::sequence<ctre::character<'b'>, ctre::assert_line_end>>()));

// atomic group
static_assert(same_f(CTRE_GEN("(?>a)"), ctre::atomic_group<ctre::character<'a'>>())); 
static_assert(same_f(CTRE_GEN("(?>ab)"), ctre::atomic_group<ctre::string<'a','b'>>())); 
static_assert(same_f(CTRE_GEN("(?>a+b)"), ctre::atomic_group<ctre::plus<ctre::character<'a'>>,ctre::character<'b'>>())); 

// lookahead positive/negative
static_assert(same_f(CTRE_GEN("(?=a)"), ctre::lookahead_positive<ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("(?=ax)"), ctre::lookahead_positive<ctre::string<'a','x'>>()));
static_assert(same_f(CTRE_GEN("(?=[a]x)"), ctre::lookahead_positive<ctre::set<ctre::character<'a'>>,ctre::character<'x'>>()));

static_assert(same_f(CTRE_GEN("(?!a)"), ctre::lookahead_negative<ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("(?!ax)"), ctre::lookahead_negative<ctre::string<'a','x'>>()));
static_assert(same_f(CTRE_GEN("(?![a]x)"), ctre::lookahead_negative<ctre::set<ctre::character<'a'>>,ctre::character<'x'>>()));

static_assert(same_f(CTRE_GEN("^(?=(a))$"), ctre::sequence<ctre::assert_line_begin, ctre::lookahead_positive<ctre::capture<1,ctre::character<'a'>>>, ctre::assert_line_end>()));

static_assert(same_f(CTRE_GEN("^(?=.*(a))$"), ctre::sequence<ctre::assert_line_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>>, ctre::assert_line_end>()));

static_assert(same_f(CTRE_GEN("^(?=.*(a).*)$"), ctre::sequence<ctre::assert_line_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>, ctre::star<ctre::any>>, ctre::assert_line_end>()));

static_assert(same_f(CTRE_GEN("^(?=.*(a)\\g{1}.*)$"), ctre::sequence<ctre::assert_line_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>, ctre::back_reference<1>, ctre::star<ctre::any>>, ctre::assert_line_end>()));

static_assert(same_f(CTRE_GEN("^(?=.*(a)\\g{1}.*)[a-z]$"), ctre::sequence<ctre::assert_line_begin, ctre::lookahead_positive<ctre::star<ctre::any>,ctre::capture<1,ctre::character<'a'>>, ctre::back_reference<1>, ctre::star<ctre::any>>, ctre::set<ctre::char_range<'a','z'>>, ctre::assert_line_end>()));

// lookbehind positive/negative
static_assert(same_f(CTRE_GEN("(?<=a)"), ctre::lookbehind_positive<ctre::character<'a'>>()));
static_assert(same_f(CTRE_GEN("(?<!a)"), ctre::lookbehind_negative<ctre::character<'a'>>()));

static_assert(same_f(CTRE_GEN("(?<=ab)"), ctre::lookbehind_positive<ctre::string<'b','a'>>()));
static_assert(same_f(CTRE_GEN("(?<!ab)"), ctre::lookbehind_negative<ctre::string<'b','a'>>()));

static_assert(same_f(CTRE_GEN("(?<=(ab))"), ctre::lookbehind_positive<ctre::capture<1, ctre::string<'b','a'>>>()));
static_assert(same_f(CTRE_GEN("(?<!(ab))"), ctre::lookbehind_negative<ctre::capture<1, ctre::string<'b','a'>>>()));
