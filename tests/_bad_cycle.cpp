#include <ctre.hpp>
#include <string_view>

template <typename Subject, typename Pattern> constexpr bool match(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::match(input);
}

template <typename Subject, typename Pattern> constexpr bool search(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::search(input);
}

template <typename Subject, typename Pattern> constexpr bool starts_with(Subject input, Pattern) {
	return ctre::regular_expression<Pattern>::starts_with(input);
}

using namespace std::string_view_literals;

static_assert(match("ab"sv, ctre::plus<ctre::select<ctre::string<'a','b'>, ctre::character<'a'>>>()), "(ab|a)+");
// this will need fix in different branch as this is a long standing problem
//static_assert(match("ab"sv, ctre::plus<ctre::select<ctre::character<'a'>, ctre::string<'a','b'>>>()), "(a|ab)+");
