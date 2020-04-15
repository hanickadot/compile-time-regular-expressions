#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

template <typename Pattern> constexpr bool match(std::string_view input, Pattern pattern) {
	return bool(ctre::match_re(input.begin(), input.end(), pattern));
}

template <typename Pattern> constexpr bool search(std::string_view input, Pattern pattern) {
	return bool(ctre::search_re(input.begin(), input.end(), pattern));
}

using namespace std::string_view_literals;

static_assert(match("a"sv, ctre::character<'a'>()));
static_assert(search("abc"sv, ctre::character<'a'>())); // only match from start
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
static_assert(match("abc"sv, ctre::sequence<ctre::assert_begin, ctre::string<'a','b','c'>>()));
static_assert(match("def"sv, ctre::sequence<ctre::assert_begin, ctre::string<'d','e','f'>, ctre::assert_end>()));
static_assert(!match("a"sv, ctre::sequence<ctre::character<'a'>, ctre::assert_begin>()));
static_assert(!match("a"sv, ctre::sequence<ctre::assert_end, ctre::character<'a'>>()));

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
static_assert(!match("aaaaaa"sv, ctre::sequence<ctre::repeat<0,5,ctre::character<'a'>>, ctre::assert_end>()));
static_assert(match("aaaaa"sv, ctre::sequence<ctre::repeat<0,5,ctre::character<'a'>>, ctre::assert_end>()));

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
