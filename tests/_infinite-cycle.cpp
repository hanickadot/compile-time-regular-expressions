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


// problems with infinite cycles
 static_assert(match(""sv, ctre::possessive_repeat<0,0,ctre::assert_end>()));
static_assert(match(""sv, ctre::greedy_repeat<0,0,ctre::assert_end>())); // problem
 static_assert(match(""sv, ctre::lazy_repeat<0,0,ctre::assert_end>()));

 static_assert(match(""sv, ctre::possessive_repeat<0,0,ctre::empty>()));
static_assert(match(""sv, ctre::greedy_repeat<0,0,ctre::empty>())); // problem
 static_assert(match(""sv, ctre::lazy_repeat<0,0,ctre::empty>()));

 static_assert(match("aaa"sv, ctre::lazy_repeat<0,0,ctre::character<'a'>>()));
 static_assert(match("aaa"sv, ctre::greedy_repeat<0,0,ctre::character<'a'>>()));
 static_assert(match("aaa"sv, ctre::possessive_repeat<0,0,ctre::character<'a'>>()));

static_assert(match("aaa"sv, ctre::greedy_repeat<0,0,ctre::greedy_repeat<0,0,ctre::character<'a'>>>())); // problem
static_assert(match("aaa"sv, ctre::greedy_repeat<0,0,ctre::possessive_repeat<0,0,ctre::character<'a'>>>())); // problem
static_assert(match("aaa"sv, ctre::greedy_repeat<0,0,ctre::lazy_repeat<0,0,ctre::character<'a'>>>())); // problem

 static_assert(match("aaa"sv, ctre::lazy_repeat<0,0,ctre::greedy_repeat<0,0,ctre::character<'a'>>>()));
 static_assert(match("aaa"sv, ctre::lazy_repeat<0,0,ctre::possessive_repeat<0,0,ctre::character<'a'>>>()));
static_assert(match("aaa"sv, ctre::lazy_repeat<0,0,ctre::lazy_repeat<0,0,ctre::character<'a'>>>())); // problem

 static_assert(match("aaa"sv, ctre::possessive_repeat<0,0,ctre::greedy_repeat<0,0,ctre::character<'a'>>>()));
 static_assert(match("aaa"sv, ctre::possessive_repeat<0,0,ctre::possessive_repeat<0,0,ctre::character<'a'>>>()));
static_assert(match("aaa"sv, ctre::possessive_repeat<0,0,ctre::lazy_repeat<0,0,ctre::character<'a'>>>())); // problem
