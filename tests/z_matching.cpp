#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

template <typename Pattern> constexpr bool match(std::string_view input, Pattern) {
	return bool(ctre::regular_expression<Pattern>::match(input.begin(), input.end()));
}

template <typename Pattern> constexpr bool search(std::string_view input, Pattern) {
	return bool(ctre::regular_expression<Pattern>::search(input.begin(), input.end()));
}

template <typename Pattern> constexpr bool starts_with(std::string_view input, Pattern) {
	return bool(ctre::regular_expression<Pattern>::starts_with(input.begin(), input.end()));
}

using namespace std::string_view_literals;

// issue #131
static_assert(match("x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::any>>, ctre::character<'x'>>()));
static_assert(match("1x"sv, ctre::sequence<ctre::lazy_optional<ctre::lazy_star<ctre::any>>, ctre::character<'x'>>()));
static_assert(match("1x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::any>>, ctre::character<'x'>>()));
static_assert(match("1x"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'> > >, ctre::character<'x'>>()));
static_assert(match("1yx"sv, ctre::sequence<ctre::optional<ctre::lazy_star<ctre::character<'1'> >, ctre::character<'y'> >, ctre::character<'x'>>()));
static_assert(match("aaab"sv, ctre::sequence<ctre::possessive_plus<ctre::optional<ctre::character<'a'>>>, ctre::character<'b'>>()));
static_assert(match("aaa"sv, ctre::possessive_plus<ctre::select<ctre::empty, ctre::character<'a'>>>()));
static_assert(match("aaa"sv, ctre::lazy_plus<ctre::select<ctre::empty, ctre::character<'a'>>>()));
static_assert(match("aaaa"sv, ctre::possessive_plus<ctre::lazy_star<ctre::character<'a'>>>()));
static_assert(match(""sv, ctre::lazy_star<ctre::character<'b'>>()));
static_assert(match("aaaa"sv, ctre::possessive_plus<ctre::lazy_star<ctre::character<'a'>>, ctre::lazy_star<ctre::character<'b'>>>()));
static_assert(match("aaaab"sv, ctre::possessive_plus<ctre::lazy_star<ctre::character<'a'>>, ctre::lazy_star<ctre::character<'b'>>>()));

