#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

template <typename Pattern> constexpr bool match(std::string_view input, Pattern pattern) {
	return bool(ctre::match_re(input.begin(), input.end(), pattern, ctre::no_flags{}));
}

template <typename Pattern> constexpr bool search(std::string_view input, Pattern pattern) {
	return bool(ctre::search_re(input.begin(), input.end(), pattern, ctre::no_flags{}));
}

template <typename Pattern> constexpr bool starts_with(std::string_view input, Pattern pattern) {
	return bool(ctre::starts_with_re(input.begin(), input.end(), pattern, ctre::no_flags{}));
}

using namespace std::string_view_literals;

// issue #131
static_assert(match("x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::any>>, ctre::character<'x'>>()));
static_assert(match("1x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::any>>, ctre::character<'x'>>()));
static_assert(match("1x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'> > >, ctre::character<'x'>>()));
static_assert(match("1yx"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'> >, ctre::character<'y'> >, ctre::character<'x'>>()));
static_assert(match("aaab"sv, ctre::sequence<ctre::possessive_plus<ctre::optional<ctre::character<'a'>>>, ctre::character<'b'>>()));
static_assert(match("aaa"sv, ctre::possessive_plus<ctre::select<ctre::empty, ctre::character<'a'>>>()));
static_assert(match("aaaa"sv, ctre::possessive_plus<ctre::lazy_star<ctre::character<'a'>>>()));
static_assert(match("aaaa"sv, ctre::possessive_plus<ctre::lazy_star<ctre::character<'a'>>, ctre::lazy_star<ctre::character<'b'>>>()));

