#ifndef CTRE__PATTERN_LENGTH__HPP
#define CTRE__PATTERN_LENGTH__HPP

#include "utf8.hpp"
#include "atoms.hpp"
#include "atoms_characters.hpp"
#include "atoms_unicode.hpp"
#include "utility.hpp"
#include "return_type.hpp"
#include "first.hpp"
#include "find_captures.hpp"

namespace ctre {

constexpr uint32_t utf8_length(int32_t utf8_codepoint) noexcept {
	if (utf8_codepoint < 0x80) {
		return 1;
	} else if (utf8_codepoint < 0x800) {   // 00000yyy yyxxxxxx
		return 2;
	} else if (utf8_codepoint < 0x10000) {  // zzzzyyyy yyxxxxxx
		return 3;
	} else if (utf8_codepoint < 0x200000) { // 000uuuuu zzzzyyyy yyxxxxxx
		return 4;
	} else { //invalid utf8 codepoint... assume we're writing out 0xff
		return 1;
	}
}

constexpr char* utf8_encode(uint32_t code, char* ptr) noexcept {
	if (code < 0x80) {
		ptr[0] = code;
		return ptr + 1;
	} else if (code < 0x800) {   // 00000yyy yyxxxxxx
		ptr[0] = (0b11000000 | (code >> 6));
		ptr[1] = (0b10000000 | (code & 0x3f));
		return ptr + 2;
	} else if (code < 0x10000) {  // zzzzyyyy yyxxxxxx
		ptr[0] = (0b11100000 | (code >> 12));         // 1110zzz
		ptr[1] = (0b10000000 | ((code >> 6) & 0x3f)); // 10yyyyy
		ptr[2] = (0b10000000 | (code & 0x3f));        // 10xxxxx
		return ptr + 3;
	} else if (code < 0x200000) { // 000uuuuu zzzzyyyy yyxxxxxx
		ptr[0] = (0b11110000 | (code >> 18));          // 11110uuu
		ptr[1] = (0b10000000 | ((code >> 12) & 0x3f)); // 10uuzzzz
		ptr[2] = (0b10000000 | ((code >> 6) & 0x3f)); // 10yyyyyy
		ptr[3] = (0b10000000 | (code & 0x3f));         // 10xxxxxx
		return ptr + 4;
	} else {
		ptr[0] = (char)0xff; //invalid start byte
		return ptr + 1;
	}
}

constexpr CTRE_FORCE_INLINE size_t pattern_infinite_size() noexcept {
	return size_t{ 0 } - 1;
}

constexpr CTRE_FORCE_INLINE size_t pattern_finite_size() noexcept {
	return size_t{ 0 } - 2;
}

constexpr CTRE_FORCE_INLINE size_t pattern_length_add(const size_t lhs, const size_t rhs) noexcept {
	const constexpr size_t inf = size_t{ 0 } - 1;
	const constexpr size_t lim = size_t{ 0 } - 2;
	size_t ret = inf;
	if (lhs == inf || rhs == inf) {
		return ret;
	} else {
		ret = lhs + rhs;
		ret = ret < lhs ? lim : ret == inf ? lim : ret;
		return ret;
	}
}

constexpr CTRE_FORCE_INLINE size_t pattern_length_mul(const size_t lhs, const size_t rhs) noexcept {
	const constexpr size_t inf = size_t{ 0 } - 1;
	const constexpr size_t lim = size_t{ 0 } - 2;
	size_t ret = inf;
	if (lhs == inf || rhs == inf) {
		return ret;
	} else if (lhs == 0 || rhs == 0) {
		return ret = 0;
	} else {
		if (lhs > (inf / rhs))
			return ret = lim;
		ret = lhs * rhs;
		ret = ret == inf ? lim : ret;
		return ret;
	}
}

constexpr CTRE_FORCE_INLINE size_t pattern_length_min(const size_t lhs, const size_t rhs) noexcept {
	return (lhs < rhs) ? lhs : rhs;
}

constexpr CTRE_FORCE_INLINE size_t pattern_length_max(const size_t lhs, const size_t rhs) noexcept {
	return (rhs > lhs) ? rhs : lhs;
}

struct pattern_length_result {
	size_t low = 0;
	size_t high = 0;
		
	constexpr pattern_length_result() = default;

	constexpr pattern_length_result(size_t low, size_t high) : low(low), high(high) {}
	constexpr pattern_length_result(size_t low_high) : low(low_high), high(low_high) {}
	constexpr pattern_length_result CTRE_FORCE_INLINE operator+(const pattern_length_result other) noexcept {
		return pattern_length_result(pattern_length_add(low, other.low), pattern_length_add(high, other.high));
	}
	constexpr pattern_length_result CTRE_FORCE_INLINE operator||(const pattern_length_result other) noexcept {
		return pattern_length_result(pattern_length_min(low, other.low), pattern_length_max(high, other.high));
	}
	constexpr pattern_length_result CTRE_FORCE_INLINE operator*(const pattern_length_result other) noexcept {
		return pattern_length_result(pattern_length_mul(low, other.low), pattern_length_mul(high, other.high));
	}
	constexpr pattern_length_result CTRE_FORCE_INLINE operator+(const pattern_length_result other) const noexcept {
		return pattern_length_result(pattern_length_add(low, other.low), pattern_length_add(high, other.high));
	}
	constexpr pattern_length_result CTRE_FORCE_INLINE operator||(const pattern_length_result other) const noexcept {
		return pattern_length_result(pattern_length_min(low, other.low), pattern_length_max(high, other.high));
	}
	constexpr pattern_length_result CTRE_FORCE_INLINE operator*(const pattern_length_result other) const noexcept {
		return pattern_length_result(pattern_length_mul(low, other.low), pattern_length_mul(high, other.high));
	}
};

//one application of this length must not be matched by multiple applications of this length
//eg: {2, 3} twice -> {4, 6}, {4, 6} doesn't overlap {2, 3}, ergo {2, 3} is "atomic"
//however* {2, 4} twice -> {4, 8}, {4, 8} does overlap {2, 4}, both may match 4 characters, ergo {2, 4} is not "atomic"
//a single match of {2,4} is not necessarily distinguishable from two potentially ranging from {4, 8} characters
constexpr bool atomic_pattern_length(pattern_length_result r) noexcept {
	return (r.low > 0) && ((r.low * 2) > r.high);
}
// checks if an atom is a backreference, these are the only parts of the pattern which need captures to make sense of
template<typename Name>
constexpr bool has_backreference(back_reference_with_name<Name>) noexcept {
	return true;
}

template<size_t Index>
constexpr bool has_backreference(back_reference<Index>) noexcept {
	return true;
}

template<typename Pattern>
constexpr bool has_backreference(Pattern) noexcept {
	return false;
}

// if the atom immediately ends pattern matching (we can discard what follows)
template<typename Pattern>
constexpr bool is_terminator(Pattern) noexcept {
	if constexpr (std::is_same_v<Pattern, reject> || std::is_same_v<Pattern, end_cycle_mark>) {
		return true;
	} else {
		return false;
	}
}

template<typename... Pattern>
constexpr bool has_backreference(ctll::list<Pattern...>) noexcept {
	bool is_accepting = true; //short circuit when we hit a terminator
	return ((is_accepting && (is_accepting = !is_terminator(Pattern{})) && has_backreference(ctll::list<Pattern>())) || ...);
}

template<typename... Pattern>
constexpr bool has_backreference(sequence<Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<typename... Pattern>
constexpr bool has_backreference(atomic_group<Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<size_t Index, typename... Pattern>
constexpr bool has_backreference(capture<Index, Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<size_t Index, typename Name, typename... Pattern>
constexpr bool has_backreference(capture_with_name<Index, Name, Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<size_t A, size_t B, typename... Pattern>
constexpr bool has_backreference(repeat<A, B, Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<size_t A, size_t B, typename... Pattern>
constexpr bool has_backreference(lazy_repeat<A, B, Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<size_t A, size_t B, typename... Pattern>
constexpr bool has_backreference(possessive_repeat<A, B, Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<typename... Pattern>
constexpr bool has_backreference(lookahead_positive<Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<typename... Pattern>
constexpr bool has_backreference(lookahead_negative<Pattern...>) noexcept {
	return has_backreference(ctll::list<Pattern...>());
}

template<typename... Pattern>
constexpr bool has_backreference(select<Pattern...>) noexcept {
	return (has_backreference(ctll::list<Pattern>()) || ...);
}

//try to minimize number of function signatures we're dealing with across multiple regexs eg: (?:a|b) should memolize if inside (?:(?:a|b)c) and (?:(?:a|b)d)
template<typename Iterator, typename Pattern>
constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(Pattern) noexcept {
	using captures_list = decltype(find_captures(Pattern{}));
	using return_type = decltype(regex_results(std::declval<std::basic_string_view<char>::iterator>(), captures_list()));
#if __cpp_char8_t >= 201811
	if constexpr (::std::is_same_v<Iterator, utf8_iterator>) {
		if constexpr (size(captures_list{}) && has_backreference(ctll::list<Pattern>()))
			return pattern_length<Iterator>(ctll::list<Pattern>(), return_type{}); //we have captures, need these as context
		else
			return pattern_length<Iterator>(ctll::list<Pattern>(), int{});
	} else {
		if constexpr (size(captures_list{}) && has_backreference(ctll::list<Pattern>()))
			return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Pattern>(), return_type{}); //we have captures, need these as context
		else
			return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Pattern>(), int{});
	}
#else
	if constexpr (size(captures_list{}) && has_backreference(ctll::list<Pattern>()))
		return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Pattern>(), return_type{}); //we have captures, need these as context
	else
		return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Pattern>(), int{});
#endif
}

template<typename Iterator, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(ctll::list<Patterns...>) noexcept {
	using captures_list = decltype(find_captures(ctll::list<Patterns...>(), ctll::list<>()));
	using return_type = decltype(regex_results(std::declval<std::basic_string_view<char>::iterator>(), captures_list()));
#if __cpp_char8_t >= 201811
	if constexpr (::std::is_same_v<Iterator, utf8_iterator>) {
		if constexpr (size(captures_list{}) && has_backreference(ctll::list<Patterns...>()))
			return pattern_length<Iterator>(ctll::list<Patterns...>(), return_type{}); //we have captures, need these as context
		else
			return pattern_length<Iterator>(ctll::list<Patterns...>(), int{});
	} else {
		if constexpr (size(captures_list{}) && has_backreference(ctll::list<Patterns...>()))
			return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Patterns...>(), return_type{}); //we have captures, need these as context
		else
			return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Patterns...>(), int{});
	}
#else
	if constexpr (size(captures_list{}) && has_backreference(ctll::list<Patterns...>()))
		return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Patterns...>(), return_type{}); //we have captures, need these as context
	else
		return pattern_length<typename std::basic_string_view<char>::iterator>(ctll::list<Patterns...>(), int{});
#endif
}

template<typename Iterator, typename R, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(ctll::list<Patterns...>, R) noexcept {
	bool accepting = true;
	pattern_length_result ret{ 0,0 };
	if constexpr (sizeof...(Patterns)) {
		ret = (((pattern_length<Iterator>(Patterns{}, R{}) * pattern_length_result {
			size_t{ (accepting && (accepting = !is_terminator(Patterns{}))) }
		})) + ... + pattern_length_result{ 0,0 });
	}
	return ret;
}

template<typename Iterator, typename R, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(sequence<Patterns...>, R) noexcept {
	bool accepting = true;
	pattern_length_result ret{ 0,0 };
	if constexpr (sizeof...(Patterns)) {
		ret = (((pattern_length<Iterator>(Patterns{}, R{}) * pattern_length_result {
			size_t{ (accepting && (accepting = !is_terminator(Patterns{}))) }
		})) + ... + pattern_length_result{ 0,0 });
	}
	return ret;
}

template<typename Iterator, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(sequence<Patterns...>) noexcept {
	bool accepting = true;
	pattern_length_result ret{ 0,0 };
	if constexpr (sizeof...(Patterns)) {
		ret = (((pattern_length<Iterator>(Patterns{}, int{}) * pattern_length_result {
			size_t{ (accepting && (accepting = !is_terminator(Patterns{}))) }
		})) + ... + pattern_length_result{ 0,0 });
	}
	return ret;
}
	
template<typename Iterator, size_t Index, typename R, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(capture<Index, Patterns...>, R) noexcept {
	return pattern_length<Iterator>(ctll::list<Patterns...>(), R{});
}

template<typename Iterator, size_t Index, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(capture<Index, Patterns...>) noexcept {
	return pattern_length<Iterator>(ctll::list<Patterns...>(), int{});
}

template<typename Iterator, typename R, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(atomic_group<Patterns...>, R) noexcept {
	return pattern_length<Iterator>(ctll::list<Patterns...>(), R{});
}

template<typename Iterator, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(atomic_group<Patterns...>) noexcept {
	return pattern_length<Iterator>(ctll::list<Patterns...>(), int{});
}

template<typename Iterator, size_t Index, typename Name, typename R, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(capture_with_name<Index, Name, Patterns...>, R) noexcept {
	return pattern_length<Iterator>(ctll::list<Patterns...>(), R{});
}

template<typename Iterator, size_t Index, typename Name, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(capture_with_name<Index, Name, Patterns...>) noexcept {
	return pattern_length<Iterator>(ctll::list<Patterns...>(), int{});
}

//needs context of captures
template<typename Iterator, size_t Index, typename R>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(back_reference<Index>, R) noexcept {
	constexpr auto ref = R{}.template get<Index>();
	if constexpr (size(ref.get_expression())) {
		return pattern_length<Iterator>(ref.get_expression(), R{});
	} else {
		return pattern_length_result{ 0ULL, 0ULL };
	}
}

template<typename Iterator, typename Name, typename R>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(back_reference_with_name<Name>, R) noexcept {
	constexpr auto ref = R{}.template get<Name>();
	if constexpr (size(ref.get_expression())) {
		return pattern_length<Iterator>(ref.get_expression(), R{});
	} else {
		return pattern_length_result{ 0ULL, 0ULL };
	}
}
// these functions can't possibly make sense w/o context, alternatively we may have them span infinite range
template<typename Iterator, typename Name>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(back_reference_with_name<Name>) noexcept = delete;

template<typename Iterator, size_t Index>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(back_reference<Index>) noexcept = delete;

template<typename Iterator, auto... Str, typename R>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(string<Str...>, R) noexcept {
#if __cpp_char8_t >= 201811
	if constexpr (::std::is_same_v<Iterator, utf8_iterator>) {
		constexpr size_t len = ((utf8_length(Str)) + ... + 0ULL);
		return pattern_length_result{ len, len };
	} else {
#endif
		return pattern_length_result{ sizeof...(Str), sizeof...(Str) };
#if __cpp_char8_t >= 201811
	}
#endif
}

template<typename Iterator, auto... Str>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(string<Str...>) noexcept {
#if __cpp_char8_t >= 201811
	if constexpr (::std::is_same_v<Iterator, utf8_iterator>) {
		constexpr size_t len = ((utf8_length(Str)) + ... + 0ULL);
		return pattern_length_result{ len, len };
	} else {
#endif
		return pattern_length_result{ sizeof...(Str), sizeof...(Str) };
#if __cpp_char8_t >= 201811
	}
#endif
}

template<typename Iterator, typename R, typename CharacterLike, typename = std::enable_if_t<MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<std::string_view::iterator>())>, void>>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(CharacterLike, R) noexcept {
	constexpr size_t capacity = calculate_size_of_first(ctll::list<CharacterLike>{});
	point_set<capacity> set;
	set.populate(ctll::list<CharacterLike>{});
	if (set.size())
		return pattern_length_result{ utf8_length(set.begin()->low), utf8_length((set.end() - 1)->high) };
	else //unclear how nothing should be handled
		return pattern_length_result{ 1, 4 };
}

template<typename Iterator, typename CharacterLike, typename = std::enable_if_t<MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<std::string_view::iterator>())>, void>>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(CharacterLike) noexcept {
	constexpr size_t capacity = calculate_size_of_first(ctll::list<CharacterLike>{});
	point_set<capacity> set;
	set.populate(ctll::list<CharacterLike>{});
	if (set.size())
		return pattern_length_result{ utf8_length(set.begin()->low), utf8_length((set.end() - 1)->high) };
	else //unclear how nothing should be handled
		return pattern_length_result{ 1, 4 };
}

template<typename Iterator, typename R, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(set<Content...>, R) noexcept {
	return pattern_length<Iterator>(select<Content...>{}, R{});
}

template<typename Iterator, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(set<Content...>) noexcept {
	return pattern_length<Iterator>(select<Content...>{}, int{});
}

template<typename Iterator, typename R, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(select<Patterns...>, R) noexcept {
	return ((pattern_length<Iterator>(Patterns{}, R{})) || ...);
}

template<typename Iterator, typename R, typename... Patterns>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(select<Patterns...>) noexcept {
	return ((pattern_length<Iterator>(Patterns{}, int{})) || ...);
}

template<typename Iterator, size_t A, size_t B, typename R, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(repeat<A, B, Content...>, R) noexcept {
	pattern_length_result ret{ 0ULL, 0ULL };
	if constexpr (sizeof...(Content)) {
		constexpr size_t MinA = A;
		constexpr size_t MaxA = (B) ? (B) : (size_t{0}-1); // 0 in the second part means inf
		ret = pattern_length<Iterator>(ctll::list<Content...>(), R{}) * pattern_length_result { MinA, MaxA };
	}
	return ret;
}

template<typename Iterator, size_t A, size_t B, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(repeat<A, B, Content...>) noexcept {
	pattern_length_result ret{ 0ULL, 0ULL };
	if constexpr (sizeof...(Content)) {
		constexpr size_t MinA = A;
		constexpr size_t MaxA = (B) ? (B) : (size_t{ 0 } - 1); // 0 in the second part means inf
		constexpr pattern_length_result mul{ MinA, MaxA };
		ret = pattern_length<Iterator>(ctll::list<Content...>(), int{}) * pattern_length_result { MinA, MaxA };
	}
	return ret;
}

template<typename Iterator, size_t A, size_t B, typename R, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(lazy_repeat<A, B, Content...>, R) noexcept {
	return pattern_length<Iterator>(repeat<A, B, Content...>(), R{});
}

template<typename Iterator, size_t A, size_t B, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(lazy_repeat<A, B, Content...>) noexcept {
	return pattern_length<Iterator>(repeat<A, B, Content...>(), int{});
}

template<typename Iterator, size_t A, size_t B, typename R, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(possessive_repeat<A, B, Content...>, R) noexcept {
	return pattern_length<Iterator>(repeat<A, B, Content...>(), R{});
}

template<typename Iterator, size_t A, size_t B, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(possessive_repeat<A, B, Content...>) noexcept {
	return pattern_length<Iterator>(repeat<A, B, Content...>(), int{});
}

//technically these don't add characters that may / may not match, they exclude certain patterns from matching
template<typename Iterator, typename R, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(lookahead_positive<Content...>, R) noexcept {
	return pattern_length_result{ 0ULL, 0ULL };
}

template<typename Iterator, typename... Content>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(lookahead_positive<Content...>) noexcept {
	return pattern_length_result{ 0ULL, 0ULL };
}
	
//sink for any atom that shouldn't impact the result
template<typename Iterator, typename Pattern, typename R, typename = std::enable_if_t<!MatchesCharacter<Pattern>::template value<decltype(*std::declval<std::string_view::iterator>())>, void>>
static constexpr CTRE_FORCE_INLINE pattern_length_result pattern_length(Pattern, R) noexcept {
	return pattern_length_result{ 0ULL, 0ULL };
}

}

#endif