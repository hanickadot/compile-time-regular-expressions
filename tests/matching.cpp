#include <ctre.hpp>
#include <string_view>

template <typename Pattern> constexpr bool match(std::string_view input, Pattern pattern) {
	return ctre::evaluate(input.begin(), input.end(), pattern);
}

using namespace std::string_view_literals;

static_assert(match("a"sv, ctre::character<'a'>()));
static_assert(match("abc"sv, ctre::character<'a'>())); // only match from start
static_assert(!match("abc"sv, ctre::character<'b'>()));
static_assert(!match("a"sv, ctre::character<'b'>()));
static_assert(match("a"sv, ctre::any()));
static_assert(match("a"sv, ctre::set<ctre::range<'a','z'>>()));
static_assert(match("f"sv, ctre::set<ctre::range<'a','z'>>()));
static_assert(match("z"sv, ctre::set<ctre::range<'a','z'>>()));
static_assert(!match("Z"sv, ctre::set<ctre::range<'a','z'>>()));
static_assert(match("0"sv, ctre::set<ctre::range<'a','z'>, ctre::range<'0','9'>>()));
static_assert(!match("A"sv, ctre::set<ctre::range<'a','z'>, ctre::range<'0','9'>>()));
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
