#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

template <typename Subject, typename Pattern> constexpr bool fast_match(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::fast_match(input);
}

template <typename Subject, typename Pattern> constexpr bool fast_search(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::fast_search(input);
}

template <typename Subject, typename Pattern> constexpr bool fast_starts_with(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::fast_starts_with(input);
}

using namespace std::string_view_literals;

static_assert(fast_match("a"sv, ctre::character<'a'>()));
static_assert(fast_search("xxaxx"sv, ctre::character<'a'>()));
static_assert(fast_starts_with("axx"sv, ctre::character<'a'>()));
static_assert(!fast_match("a"sv, ctre::character<'b'>()));
static_assert(!fast_search("xxaxx"sv, ctre::character<'b'>()));
static_assert(!fast_starts_with("axx"sv, ctre::character<'b'>()));
