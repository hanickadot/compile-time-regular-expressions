#ifndef CTRE__EVALUATION__HPP
#define CTRE__EVALUATION__HPP

#include "flags_and_modes.hpp"
#include "atoms.hpp"
#include "atoms_unicode.hpp"
#include "starts_with_anchor.hpp"
#include "utility.hpp"
#include "return_type.hpp"
#include "find_captures.hpp"
#include "first.hpp"
#include <iterator>

// remove me when MSVC fix the constexpr bug
#ifdef _MSC_VER
#ifndef CTRE_MSVC_GREEDY_WORKAROUND
#define CTRE_MSVC_GREEDY_WORKAROUND
#endif
#endif

namespace ctre {

template <size_t Limit> constexpr CTRE_FORCE_INLINE bool less_than_or_infinite(size_t i) {
	if constexpr (Limit == 0) {
		// infinite
		return true;
	} else {
		return i < Limit;
	}
}

template <size_t Limit> constexpr CTRE_FORCE_INLINE bool less_than(size_t i) {
	if constexpr (Limit == 0) {
		// infinite
		return false;
	} else {
		return i < Limit;
	}
}

constexpr bool is_bidirectional(const std::bidirectional_iterator_tag &) { return true; }
constexpr bool is_bidirectional(...) { return false; }

// sink for making the errors shorter
template <typename R, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R, ...) noexcept = delete;

// if we found "accept" object on stack => ACCEPT
template <typename R, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R captures, ctll::list<accept>) noexcept {
	return captures.matched();
}

// if we found "reject" object on stack => REJECT
template <typename R, typename... Rest, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R, ctll::list<reject, Rest...>) noexcept {
	return not_matched;
}

// mark start of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<start_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.set_start_mark(current), ctll::list<Tail...>());
}

// mark end of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<end_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.set_end_mark(current), ctll::list<Tail...>());
}

// mark end of cycle
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator current, const EndIterator, [[maybe_unused]] const flags & f, R captures, ctll::list<end_cycle_mark>) noexcept {
	if (cannot_be_empty_match(f)) {
		return not_matched;
	}
	
	return captures.set_end_mark(current).matched();
}

// matching everything which behave as a one character matcher

template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (current == end) return not_matched;
	if (!CharacterLike::match_char(*current)) return not_matched;
	
	return evaluate(begin, ++current, end, consumed_something(f), captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<any, Tail...>) noexcept {
	if (current == end) return not_matched;
	
	if (multiline_mode(f)) {
		// TODO add support for different line ending and unicode (in a future unicode mode)
		if (*current == '\n') return not_matched;
	}
	return evaluate(begin, ++current, end, consumed_something(f), captures, ctll::list<Tail...>());
}

// matching strings in patterns

template <typename Iterator> struct string_match_result {
	Iterator position;
	bool match;
};

template <typename CharT, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE bool compare_character(CharT c, Iterator & it, const EndIterator & end) {
	if (it != end) {
		using char_type = decltype(*it);
		return *it++ == static_cast<char_type>(c);
	}
	return false;
}

struct zero_terminated_string_end_iterator;
template <auto... String, size_t... Idx, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> evaluate_match_string(Iterator current, [[maybe_unused]] const EndIterator end, std::index_sequence<Idx...>) noexcept {
#if __cpp_char8_t >= 201811
	if constexpr (sizeof...(String) && !std::is_same_v<Iterator, utf8_iterator> && is_random_accessible(typename std::iterator_traits<Iterator>::iterator_category{}) && !std::is_same_v<EndIterator, ctre::zero_terminated_string_end_iterator>) {
#else
	if constexpr (sizeof...(String) && is_random_accessible(typename std::iterator_traits<Iterator>::iterator_category{}) && !std::is_same_v<EndIterator, ctre::zero_terminated_string_end_iterator>) {
#endif
		using char_type = decltype(*current);
		bool same = ((size_t)std::distance(current, end) >= sizeof...(String)) && ((static_cast<char_type>(String) == *(current + Idx)) && ...);
		if (same) {
			return { current += sizeof...(String), same };
		} else {
			return { current, same };
		}
	} else {
		bool same = (compare_character(String, current, end) && ... && true);
		return { current, same };
	}
}

template <typename R, typename Iterator, typename EndIterator, auto... String, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] const flags & f, R captures, ctll::list<string<String...>, Tail...>) noexcept {
	auto result = evaluate_match_string<String...>(current, end, std::make_index_sequence<sizeof...(String)>());
	
	if (!result.match) {
		return not_matched;
	}
	
	return evaluate(begin, result.position, end, consumed_something(f, sizeof...(String) > 0), captures, ctll::list<Tail...>());
}

// matching select in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) noexcept {
	if (auto r = evaluate(begin, current, end, f, captures, ctll::list<HeadOptions, Tail...>())) {
		return r;
	} else {
		return evaluate(begin, current, end, f, captures, ctll::list<select<TailOptions...>, Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R, ctll::list<select<>, Tail...>) noexcept {
	// no previous option was matched => REJECT
	return not_matched;
}

// matching sequence in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailContent) > 0) {
		return evaluate(begin, current, end, f, captures, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
	} else {
		return evaluate(begin, current, end, f, captures, ctll::list<HeadContent, Tail...>());
	}
}

// matching empty in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<empty, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

// matching asserts
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<assert_subject_begin, Tail...>) noexcept {
	if (begin != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<assert_subject_end, Tail...>) noexcept {
	if (end != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<assert_subject_end_line, Tail...>) noexcept {
	if (multiline_mode(f)) {
		if (end == current) {
			return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
		} else if (*current == '\n' && std::next(current) == end) {
			return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
		} else {
			return not_matched;
		}
	} else {
		if (end != current) {
			return not_matched;
		}
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<assert_line_begin, Tail...>) noexcept {
	if (multiline_mode(f)) {
		if (begin == current) {
			return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
		} else if (*std::prev(current) == '\n') {
			return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
		} else {
			return not_matched;
		}
	} else {
		if (begin != current) {
			return not_matched;
		}
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<assert_line_end, Tail...>) noexcept {
	if (multiline_mode(f)) {
		if (end == current) {
			return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
		} else if (*current == '\n') {
			return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
		} else {
			return not_matched;
		}
	} else {
		if (end != current) {
			return not_matched;
		}
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	}
	
	// TODO properly match line end
	if (end != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

// matching boundary
template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<boundary<CharacterLike>, Tail...>) noexcept {
	
	// reason why I need bidirectional iterators or some clever hack
	bool before = false;
	bool after = false;
	
	static_assert(is_bidirectional(typename std::iterator_traits<Iterator>::iterator_category{}), "To use boundary in regex you need to provide bidirectional iterator or range.");
	
	if (end != current) {
		after = CharacterLike::match_char(*current);
	}
	if (begin != current) {
		before = CharacterLike::match_char(*std::prev(current));
	}
	
	if (before == after) return not_matched;
	
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

// matching not_boundary
template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<not_boundary<CharacterLike>, Tail...>) noexcept {
	
	// reason why I need bidirectional iterators or some clever hack
	bool before = false;
	bool after = false;
	
	static_assert(is_bidirectional(typename std::iterator_traits<Iterator>::iterator_category{}), "To use boundary in regex you need to provide bidirectional iterator or range.");
	
	if (end != current) {
		after = CharacterLike::match_char(*current);
	}
	if (begin != current) {
		before = CharacterLike::match_char(*std::prev(current));
	}
	
	if (before != after) return not_matched;
	
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

// lazy repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] const flags & f, R captures, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) noexcept {

	if constexpr (B != 0 && A > B) {
		return not_matched;
	}
	
	const Iterator backup_current = current;
	
	size_t i{0};
	
	while (less_than<A>(i)) {
		auto outer_result = evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
		if (!outer_result) return not_matched;
		
		captures = outer_result.unmatch();
		current = outer_result.get_end_position();
		
		++i;
	}
	
	if (auto outer_result = evaluate(begin, current, end, consumed_something(f, backup_current != current), captures, ctll::list<Tail...>())) {
		return outer_result;
	}
	
	while (less_than_or_infinite<B>(i)) {
		auto inner_result = evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
		if (!inner_result) return not_matched;
		
		auto outer_result = evaluate(begin, inner_result.get_end_position(), end, consumed_something(f), inner_result.unmatch(), ctll::list<Tail...>());
		
		if (outer_result) {
			return outer_result;
		}
		
		captures = inner_result.unmatch();
		current = inner_result.get_end_position();
		
		++i;
	}
	
	// rest of regex
	return evaluate(begin, current, end, consumed_something(f), captures, ctll::list<Tail...>());
}

// possessive repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] const flags & f, R captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {

	if constexpr ((B != 0) && (A > B)) {
		return not_matched;
	}
	
	const auto backup_current = current;

	for (size_t i{0}; less_than_or_infinite<B>(i); ++i) {
		// try as many of inner as possible and then try outer once
		auto inner_result = evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
		if (!inner_result) {
			if (!less_than<A>(i)) break;
			return not_matched;
		}
		
		captures = inner_result.unmatch();
		current = inner_result.get_end_position();
	}
	
	return evaluate(begin, current, end, consumed_something(f, backup_current != current), captures, ctll::list<Tail...>());
}

// (gready) repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
#ifdef CTRE_MSVC_GREEDY_WORKAROUND
constexpr inline void evaluate_recursive(R & result, size_t i, const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] const flags & f, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
#else
constexpr inline R evaluate_recursive(size_t i, const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] const flags & f, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
#endif
	if (less_than_or_infinite<B>(i)) {
		 
		// a*ab
		// aab
		
		if (auto inner_result = evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>())) {
			// TODO MSVC issue:
			// if I uncomment this return it will not fail in constexpr (but the matching result will not be correct)
			//  return inner_result
			// I tried to add all constructors to R but without any success
			auto tmp_current = current;
			tmp_current = inner_result.get_end_position();
			#ifdef CTRE_MSVC_GREEDY_WORKAROUND
			evaluate_recursive(result, i+1, begin, tmp_current, end, f, inner_result.unmatch(), stack);
			if (result) {
				return;
			}
			#else
			if (auto rec_result = evaluate_recursive(i+1, begin, tmp_current, end, f, inner_result.unmatch(), stack)) {
				return rec_result;
			}
			#endif
		}
	}
	#ifdef CTRE_MSVC_GREEDY_WORKAROUND
	result = evaluate(begin, current, end, consumed_something(f), captures, ctll::list<Tail...>());
	#else
	return evaluate(begin, current, end, consumed_something(f), captures, ctll::list<Tail...>());
	#endif
}	



// (greedy) repeat 
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] const flags & f, R captures, [[maybe_unused]] ctll::list<repeat<A,B,Content...>, Tail...> stack) {

	if constexpr ((B != 0) && (A > B)) {
		return not_matched;
	}

#ifndef CTRE_DISABLE_GREEDY_OPT
	if constexpr (!collides(calculate_first(Content{}...), calculate_first(Tail{}...))) {
		return evaluate(begin, current, end, f, captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>());
	}
#endif
	
	// A..B
	size_t i{0};
	while (less_than<A>(i)) {
		auto inner_result = evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
		if (!inner_result) return not_matched;
		
		captures = inner_result.unmatch();
		current = inner_result.get_end_position();
		
		++i;
	}
	
#ifdef CTRE_MSVC_GREEDY_WORKAROUND
	R result;
	evaluate_recursive(result, i, begin, current, end, f, captures, stack);
	return result;
#else
	return evaluate_recursive(i, begin, current, end, f, captures, stack);
#endif

}

// capture (numeric ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// capture end mark (numeric and string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

// capture (string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// backreference support (match agains content of iterators)
template <typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> match_against_range(Iterator current, const EndIterator end, Iterator range_current, const Iterator range_end, flags) noexcept {
	while (end != current && range_end != range_current) {
		if (*current == *range_current) {
			current++;
			range_current++;
		} else {
			return {current, false};
		}
	}
	return {current, range_current == range_end};
}

// backreference with name
template <typename R, typename Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<back_reference_with_name<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto result = match_against_range(current, end, ref.begin(), ref.end(), f); result.match) {
			return evaluate(begin, result.position, end, consumed_something(f, ref.begin() != ref.end()), captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

// backreference
template <typename R, size_t Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<back_reference<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto result = match_against_range(current, end, ref.begin(), ref.end(), f); result.match) {
			return evaluate(begin, result.position, end, consumed_something(f, ref.begin() != ref.end()), captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

// end of lookahead
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R captures, ctll::list<end_lookahead_mark>) noexcept {
	// TODO check interaction with non-empty flag
	return captures.matched();
}

// lookahead positive
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<lookahead_positive<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, f, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		captures = lookahead_result.unmatch();
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// lookahead negative
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, const flags & f, R captures, ctll::list<lookahead_negative<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, f, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		return not_matched;
	} else {
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	}
}

template <typename T>
constexpr bool is_string(T) noexcept {
	return false;
}
template <auto... String>
constexpr bool is_string(string<String...>)noexcept {
	return true;
}

template <typename T>
constexpr bool is_string_like(T) noexcept {
	return false;
}
template <auto... String>
constexpr bool is_string_like(string<String...>) noexcept {
	return true;
}
template <typename CharacterLike, typename = std::enable_if_t<MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<std::string_view::iterator>())>>>
constexpr bool is_string_like(CharacterLike) noexcept {
	return true;
}

template <typename... Content>
constexpr auto extract_leading_string(ctll::list<Content...>) noexcept -> ctll::list<Content...> {
	return {};
}
template <typename... Content>
constexpr auto extract_leading_string(sequence<Content...>) noexcept -> sequence<Content...> {
	return {};
}

//concatenation
template <auto C, auto... String, typename... Content>
constexpr auto extract_leading_string(ctll::list<string<String...>, character<C>, Content...>) noexcept {
	return extract_leading_string(ctll::list<string<String..., C>, Content...>());
}

template <auto... StringA, auto... StringB, typename... Content>
constexpr auto extract_leading_string(ctll::list<string<StringA...>, string<StringB...>, Content...>) noexcept {
	return extract_leading_string(ctll::list<string<StringA..., StringB...>, Content...>());
}
//move things up out of sequences
template <typename... Content, typename... Tail>
constexpr auto extract_leading_string(ctll::list<sequence<Content...>, Tail...>) noexcept {
	return extract_leading_string(ctll::list<Content..., Tail...>());
}

template <typename T, typename... Content, typename... Tail>
constexpr auto extract_leading_string(ctll::list<T, sequence<Content...>, Tail...>) noexcept {
	return extract_leading_string(ctll::list<T, Content..., Tail...>());
}

template <typename... Content>
constexpr auto make_into_sequence(ctll::list<Content...>) noexcept -> sequence<Content...> {
	return{};
}
template <typename... Content>
constexpr auto make_into_sequence(sequence<Content...>) noexcept -> sequence<Content...> {
	return{};
}

//boyer moore utils
template<typename Ty>
constexpr bool is_prefix(Ty* word, size_t wordlen, ptrdiff_t pos) {
	ptrdiff_t suffixlen = wordlen - pos;
	for (int i = 0; i < suffixlen; i++) {
		if (word[i] != word[pos + i]) {
			return false;
		}
	}
	return true;
}

template<typename Ty>
constexpr size_t suffix_length(Ty* word, size_t wordlen, ptrdiff_t pos) {
	size_t i = 0;
	// increment suffix length i to the first mismatch or beginning of the word
	for (; (word[pos - i] == word[wordlen - 1 - i]) && (i < pos); i++);
	return i;
}
//MSVC workaround, array operator[] blows up in face if constexpr, use pointers instead
template<typename Ty, auto... String>
constexpr auto make_delta_2(string<String...>) {
	std::array<Ty, sizeof...(String)> chars{ String... };
	std::array<ptrdiff_t, sizeof...(String)> table;
	constexpr size_t patlen = sizeof...(String);
	size_t p = 0;
	size_t last_prefix_index = patlen - 1;

	for (p = patlen - 1; p < patlen; p--) {
		if (is_prefix(chars.data(), patlen, p + 1)) {
			last_prefix_index = p + 1;
		}
		table.data()[p] = last_prefix_index + (patlen - 1 - p);
	}

	for (p = 0; p < patlen - 1; p++) {
		size_t slen = suffix_length(chars.data(), patlen, p);
		if (chars.data()[p - slen] != chars.data()[patlen - 1 - slen]) {
			table.data()[patlen - 1 - slen] = patlen - 1 - p + slen;
		}
	}

	return table;
}

template <typename Iterator> struct string_search_result {
	Iterator position;
	Iterator end_position;
	bool match;
};

template <typename Iterator, typename EndIterator, auto... String>
constexpr CTRE_FORCE_INLINE string_search_result<Iterator> search_for_string(Iterator current, const EndIterator end, string<String...>) noexcept {
#if __cpp_char8_t >= 201811
	if constexpr (sizeof...(String) > 2 && !std::is_same_v<Iterator, utf8_iterator> && is_random_accessible(typename std::iterator_traits<Iterator>::iterator_category{})) {
#else
	if constexpr (sizeof...(String) > 2 && is_random_accessible(typename std::iterator_traits<Iterator>::iterator_category{})) {
#endif
		constexpr std::array<typename ::std::iterator_traits<Iterator>::value_type, sizeof...(String)> chars{ String... };
		constexpr std::array<ptrdiff_t, sizeof...(String)> delta_2 = make_delta_2<typename ::std::iterator_traits<Iterator>::value_type>(string<String...>());

		size_t str_size = std::distance(current, end);
		if (str_size < sizeof...(String)) { //quick exit no way to match
			return { current + str_size, current + str_size, false };
		}

		size_t i = sizeof...(String) - 1; //index over to the starting location
		for (; i < str_size;) {
			size_t j = sizeof...(String) - 1;
			size_t m = i + 1;
			for (; *(current + i) == *(chars.data() + j); --i, --j) { //match string in reverse
				if (j == 0) {
					return { current + i, current + m, true };
				}
			}
			size_t shift = enumeration<String...>::match_char(*(current + i)) ? static_cast<size_t>(*(delta_2.data() + j)) : sizeof...(String);
			i += shift;
		}

		return { current + str_size, current + str_size, false };
	} else if constexpr (sizeof...(String)) {
		//fallback to plain string matching
		constexpr std::array<typename ::std::iterator_traits<Iterator>::value_type, sizeof...(String)> chars{ String... };
		constexpr typename ::std::iterator_traits<Iterator>::value_type first_char = chars.data()[0];
		while (current != end) {
			while (current != end && *current != first_char) {
				current++;
			}
			auto result = evaluate_match_string<String...>(current, end, std::make_index_sequence<sizeof...(String)>());
			if (result.match) {
				return { current, result.position, result.match };
			} else {
				++current;
			}
		}
		return { current, current, false };
	} else {
		return { current, current, true };
	}
}

}

#endif
