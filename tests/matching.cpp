#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

template <typename Subject, typename Pattern> constexpr bool match(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::match(input);
}

template <typename Subject, typename Pattern> constexpr bool search(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::search(input);
}

template <typename Subject, typename Pattern> constexpr bool starts_with(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::starts_with(input);
}

template <typename Subject, typename Pattern> constexpr auto multiline_match(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::multiline_match(input);
}

template <typename Subject, typename Pattern> constexpr auto multiline_search(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::multiline_search(input);
}

template <typename Subject, typename Pattern> constexpr auto multiline_starts_with(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::multiline_starts_with(input);
}

using namespace std::string_view_literals;

static_assert(match("a"sv, ctre::character<'a'>()));
static_assert(search("abc"sv, ctre::character<'a'>()));
static_assert(search("abc"sv, ctre::character<'b'>())); 
static_assert(search("abc"sv, ctre::character<'c'>())); 
static_assert(starts_with("abc"sv, ctre::character<'a'>())); 
static_assert(!starts_with("abc"sv, ctre::character<'b'>()));
static_assert(!match("abc"sv, ctre::character<'b'>()));
static_assert(!match("a"sv, ctre::character<'b'>()));
static_assert(match("a"sv, ctre::any()));
static_assert(match("a"sv, ctre::set<ctre::char_range<'a','z'>>()));
static_assert(match("f"sv, ctre::set<ctre::char_range<'a','z'>>()));
static_assert(match("z"sv, ctre::set<ctre::char_range<'a','z'>>()));
static_assert(!match("Z"sv, ctre::set<ctre::char_range<'a','z'>>()));
static_assert(match("0"sv, ctre::set<ctre::char_range<'a','z'>, ctre::char_range<'0','9'>>()));
static_assert(!match("A"sv, ctre::set<ctre::char_range<'a','z'>, ctre::char_range<'0','9'>>()));
static_assert(match("0"sv, ctre::set<ctre::xdigit_chars>()));
static_assert(match("9"sv, ctre::set<ctre::xdigit_chars>()));
static_assert(match("a"sv, ctre::set<ctre::xdigit_chars>()));
static_assert(match("f"sv, ctre::set<ctre::xdigit_chars>()));
static_assert(!match("g"sv, ctre::set<ctre::xdigit_chars>()));
static_assert(match("abcdef"sv, ctre::string<'a','b','c','d','e','f'>()));
static_assert(!match("abcgef"sv, ctre::string<'a','b','c','d','e','f'>()));
static_assert(match(""sv, ctre::string<>()));
static_assert(match("a"sv, ctre::select<ctre::character<'a'>, ctre::character<'b'>, ctre::character<'c'>>()));
static_assert(match("b"sv, ctre::select<ctre::character<'a'>, ctre::character<'b'>, ctre::character<'c'>>()));
static_assert(match("c"sv, ctre::select<ctre::character<'a'>, ctre::character<'b'>, ctre::character<'c'>>()));
static_assert(!match("d"sv, ctre::select<ctre::character<'a'>, ctre::character<'b'>, ctre::character<'c'>>()));
static_assert(match("xy"sv, ctre::optional<ctre::string<'x','y'>>()));
static_assert(match(""sv, ctre::optional<ctre::string<'x','y'>>()));
static_assert(match("abc"sv, ctre::sequence<ctre::assert_subject_begin, ctre::string<'a','b','c'>>()));
static_assert(match("def"sv, ctre::sequence<ctre::assert_subject_begin, ctre::string<'d','e','f'>, ctre::assert_subject_end>()));
static_assert(!match("a"sv, ctre::sequence<ctre::character<'a'>, ctre::assert_subject_begin>()));
static_assert(!match("a"sv, ctre::sequence<ctre::assert_subject_end, ctre::character<'a'>>()));

static_assert(match("aaax"sv, ctre::sequence<ctre::lazy_plus<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(match("ax"sv, ctre::sequence<ctre::lazy_plus<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(!match("x"sv, ctre::sequence<ctre::lazy_plus<ctre::character<'a'>>, ctre::character<'x'>>()));

static_assert(match("aaax"sv, ctre::sequence<ctre::possessive_plus<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(match("ax"sv, ctre::sequence<ctre::possessive_plus<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(!match("x"sv, ctre::sequence<ctre::possessive_plus<ctre::character<'a'>>, ctre::character<'x'>>()));

static_assert(match("aaax"sv, ctre::sequence<ctre::lazy_star<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(match("ax"sv, ctre::sequence<ctre::lazy_star<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(match("x"sv, ctre::sequence<ctre::lazy_star<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(!match("y"sv, ctre::sequence<ctre::lazy_star<ctre::character<'a'>>, ctre::character<'x'>>()));

static_assert(match("aaax"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(match("ax"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(match("x"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::character<'x'>>()));
static_assert(!match("y"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::character<'x'>>()));

static_assert(!match("aaab"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(!match("aaab"sv, ctre::sequence<ctre::possessive_plus<ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(!match("ab"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(!match("aab"sv, ctre::sequence<ctre::possessive_plus<ctre::character<'a'>>, ctre::string<'a','b'>>()));

static_assert(match("aaba"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::string<'b','a'>>()));
static_assert(match("aaba"sv, ctre::sequence<ctre::possessive_plus<ctre::character<'a'>>, ctre::string<'b','a'>>()));
static_assert(match("ba"sv, ctre::sequence<ctre::possessive_star<ctre::character<'a'>>, ctre::string<'b','a'>>()));
static_assert(match("aba"sv, ctre::sequence<ctre::possessive_plus<ctre::character<'a'>>, ctre::string<'b','a'>>()));

static_assert(match("aaax"sv, ctre::sequence<ctre::lazy_repeat<3,0,ctre::character<'a'>>, ctre::character<'x'>>()));

static_assert(search("aaaaaa"sv, ctre::repeat<0,5,ctre::character<'a'>>()));
static_assert(!match("aaaaaa"sv, ctre::sequence<ctre::repeat<0,5,ctre::character<'a'>>, ctre::assert_subject_end>()));
static_assert(match("aaaaa"sv, ctre::sequence<ctre::repeat<0,5,ctre::character<'a'>>, ctre::assert_subject_end>()));

static_assert(match("aaa"sv, ctre::star<ctre::character<'a'>>()));
static_assert(match("aaa"sv, ctre::plus<ctre::character<'a'>>()));
static_assert(match(""sv, ctre::star<ctre::character<'a'>>()));
static_assert(match("a"sv, ctre::plus<ctre::character<'a'>>()));

static_assert(match("aaxb"sv, ctre::sequence<ctre::star<ctre::character<'a'>>, ctre::string<'x','b'>>()));
static_assert(match("aaxb"sv, ctre::sequence<ctre::plus<ctre::character<'a'>>, ctre::string<'x','b'>>()));
static_assert(match("xb"sv, ctre::sequence<ctre::star<ctre::character<'a'>>, ctre::string<'x','b'>>()));
static_assert(match("axb"sv, ctre::sequence<ctre::plus<ctre::character<'a'>>, ctre::string<'x','b'>>()));

static_assert(match("aaab"sv, ctre::sequence<ctre::star<ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(match("aaab"sv, ctre::sequence<ctre::plus<ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(match("ab"sv, ctre::sequence<ctre::star<ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(match("aab"sv, ctre::sequence<ctre::plus<ctre::character<'a'>>, ctre::string<'a','b'>>()));

static_assert(!match("aab"sv, ctre::sequence<ctre::repeat<2,5,ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(match("aaab"sv, ctre::sequence<ctre::repeat<2,5,ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(match("aaaab"sv, ctre::sequence<ctre::repeat<2,5,ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(match("aaaaab"sv, ctre::sequence<ctre::repeat<2,5,ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(match("aaaaaab"sv, ctre::sequence<ctre::repeat<2,5,ctre::character<'a'>>, ctre::string<'a','b'>>()));
static_assert(!match("aaaaaaab"sv, ctre::sequence<ctre::repeat<2,5,ctre::character<'a'>>, ctre::string<'a','b'>>()));

// issue #131
static_assert(match("x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'>>>, ctre::character<'x'>>()));

static_assert(match("1x"sv, ctre::sequence<ctre::optional<ctre::lazy_optional<ctre::character<'1'>>>, ctre::character<'x'>>()));
static_assert(match("1x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'>>>, ctre::character<'x'>>()));
static_assert(match("1x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'> > >, ctre::character<'x'>>()));
static_assert(match("aaab"sv, ctre::sequence<ctre::possessive_plus<ctre::optional<ctre::character<'a'>>>, ctre::character<'b'>>()));

static_assert(match("1yx"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'> >, ctre::character<'y'> >, ctre::character<'x'>>()));
static_assert(match("aaa"sv, ctre::possessive_plus<ctre::select<ctre::empty, ctre::character<'a'>>>()));

static_assert(match("x"sv, ctre::repeat<1,0,ctre::repeat<0,0,ctre::character<'x'>>>()));
static_assert(match("x"sv, ctre::repeat<1,0,ctre::lazy_repeat<0,0,ctre::character<'x'>>>()));
static_assert(match("x"sv, ctre::repeat<1,0,ctre::possessive_repeat<0,0,ctre::character<'x'>>>()));

static_assert(match("x"sv, ctre::lazy_repeat<1,0,ctre::repeat<0,0,ctre::character<'x'>>>()));
static_assert(match("x"sv, ctre::lazy_repeat<1,0,ctre::lazy_repeat<0,0,ctre::character<'x'>>>()));
static_assert(match("x"sv, ctre::lazy_repeat<1,0,ctre::possessive_repeat<0,0,ctre::character<'x'>>>()));

static_assert(match("x"sv, ctre::possessive_repeat<1,0,ctre::repeat<0,0,ctre::character<'x'>>>()));
static_assert(match("x"sv, ctre::possessive_repeat<1,0,ctre::lazy_repeat<0,0,ctre::character<'x'>>>()));
static_assert(match("x"sv, ctre::possessive_repeat<1,0,ctre::possessive_repeat<0,0,ctre::character<'x'>>>()));

static_assert(match("xy"sv, ctre::sequence<ctre::repeat<1,0,ctre::repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::repeat<1,0,ctre::lazy_repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::repeat<1,0,ctre::possessive_repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));

static_assert(match("xy"sv, ctre::sequence<ctre::lazy_repeat<1,0,ctre::repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::lazy_repeat<1,0,ctre::lazy_repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::lazy_repeat<1,0,ctre::possessive_repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));

static_assert(match("xy"sv, ctre::sequence<ctre::possessive_repeat<1,0,ctre::repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::possessive_repeat<1,0,ctre::lazy_repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::possessive_repeat<1,0,ctre::possessive_repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));


static_assert(match("xy"sv, ctre::sequence<ctre::repeat<0,0,ctre::repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::repeat<0,0,ctre::lazy_repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::repeat<0,0,ctre::possessive_repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));

static_assert(match("xy"sv, ctre::sequence<ctre::lazy_repeat<0,0,ctre::repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::lazy_repeat<0,0,ctre::lazy_repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::lazy_repeat<0,0,ctre::possessive_repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));

static_assert(match("xy"sv, ctre::sequence<ctre::possessive_repeat<0,0,ctre::repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::possessive_repeat<0,0,ctre::lazy_repeat<1,0,ctre::character<'x'>>>, ctre::character<'y'>>()));
static_assert(match("xy"sv, ctre::sequence<ctre::possessive_repeat<1,0,ctre::possessive_repeat<0,0,ctre::character<'x'>>>, ctre::character<'y'>>()));

// word boundary
static_assert(starts_with("a "sv, ctre::sequence<ctre::character<'a'>, ctre::boundary<ctre::word_chars>>()));
static_assert(starts_with(u8"a "sv, ctre::sequence<ctre::character<'a'>, ctre::boundary<ctre::word_chars>>()));
static_assert(starts_with("aaaa "sv, ctre::sequence<ctre::plus<ctre::character<'a'>>, ctre::boundary<ctre::word_chars>, ctre::any>()));
static_assert(starts_with("  aaaa"sv, ctre::sequence<ctre::plus<ctre::any>, ctre::boundary<ctre::word_chars>>()));
static_assert(starts_with(u8"aaaa "sv, ctre::sequence<ctre::plus<ctre::character<'a'>>, ctre::boundary<ctre::word_chars>, ctre::any>()));
static_assert(starts_with(u8"  aaaa"sv, ctre::sequence<ctre::plus<ctre::any>, ctre::boundary<ctre::word_chars>>()));
static_assert(match("a ", ctre::sequence<ctre::character<'a'>, ctre::word_boundary, ctre::character<' '>>() ));
static_assert(match("ab", ctre::sequence<ctre::character<'a'>, ctre::not_word_boundary, ctre::character<'b'>>() ));

// multiline
static_assert(multiline_match("", ctre::assert_subject_begin()));
static_assert(multiline_match("", ctre::assert_subject_end()));
static_assert(multiline_match("", ctre::assert_line_begin()));
static_assert(multiline_match("", ctre::assert_line_end()));
static_assert(multiline_match("", ctre::assert_subject_end_line()));
static_assert(!multiline_match("\n", ctre::any()));
static_assert(multiline_starts_with("aaa\nbbb", ctre::plus<ctre::any>()));
static_assert(multiline_starts_with("aaa\nbbb", ctre::plus<ctre::any>()).size() == 3);
static_assert(multiline_starts_with("aaa\nbbb", ctre::sequence<ctre::plus<ctre::any>,ctre::assert_line_end,ctre::character<'\n'>>()));
static_assert(multiline_starts_with("aaa\nbbb", ctre::sequence<ctre::plus<ctre::any>,ctre::assert_line_end,ctre::character<'\n'>>()).size() == 4);
