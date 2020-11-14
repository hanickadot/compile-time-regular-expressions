#ifndef CTRE__EVALUATION__HPP
#define CTRE__EVALUATION__HPP

#include "flags_and_modes.hpp"
#include "atoms.hpp"
#include "atoms_unicode.hpp"
#include "starts_with_anchor.hpp"
#include "utility.hpp"
#include "return_type.hpp"
#include "find_captures.hpp"
#include "optimizer/first.hpp"
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

template <typename> static constexpr bool always_false = false; 

// helper to make Captures reference
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate_split(const Iterator begin, Iterator current, const EndIterator end, flags f, R captures, Tail...) noexcept {
	return evaluate(begin, current, end, f, captures, Tail{}...);
}

// sink for making the errors shorter
template <typename R, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R &, ...) noexcept {
	static_assert(always_false<R>, "unknown atom to evaluate!");
	return not_matched;
}

// if we found "accept" object on stack => ACCEPT
template <typename R, typename Iterator, typename EndIterator, typename... Rest> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R & captures, ctll::list<accept, Rest...>) noexcept {
	return captures.matched();
}

// if we found "reject" object on stack => REJECT
template <typename R, typename... Rest, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R &, ctll::list<reject, Rest...>) noexcept {
	return not_matched;
}

// mark start of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<start_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.set_start_mark(current), ctll::list<Tail...>());
}

// mark end of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<end_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.set_end_mark(current), ctll::list<Tail...>());
}

// mark end of cycle
template <typename R, typename Iterator, typename EndIterator, typename... Rest> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator current, const EndIterator, [[maybe_unused]] flags f, R & captures, ctll::list<end_cycle_mark, Rest...>) noexcept {
	if (cannot_be_empty_match(f)) {
		return not_matched;
	}
	
	return captures.set_end_mark(current).matched();
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] flags f, R & captures, ctll::list<fail_if_empty, Tail...>) noexcept {
	if (cannot_be_empty_match(f)) {
		return not_matched;
	}
	
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

// matching everything which behave as a one character matcher

template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (current == end) return not_matched;
	if (!CharacterLike::match_char(*current)) return not_matched;
	
	return evaluate(begin, ++current, end, consumed_something(f), captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<any, Tail...>) noexcept {
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
		return *it++ == c;
	}
	return false;
}

template <auto... String, size_t... Idx, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> evaluate_match_string(Iterator current, [[maybe_unused]] const EndIterator end, std::index_sequence<Idx...>) noexcept {

	bool same = (compare_character(String, current, end) && ... && true);

	return {current, same};
}

template <typename R, typename Iterator, typename EndIterator, auto... String, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] flags f, R & captures, ctll::list<string<String...>, Tail...>) noexcept {
	auto result = evaluate_match_string<String...>(current, end, std::make_index_sequence<sizeof...(String)>());
	
	if (!result.match) {
		return not_matched;
	}
	
	return evaluate(begin, result.position, end, consumed_something(f, sizeof...(String) > 0), captures, ctll::list<Tail...>());
}

// matching select in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) noexcept {
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
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<sequence<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures, ctll::list<Content..., Tail...>());
}

// matching empty in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<empty, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

// matching asserts
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<assert_subject_begin, Tail...>) noexcept {
	if (begin != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<assert_subject_end, Tail...>) noexcept {
	if (end != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<assert_subject_end_line, Tail...>) noexcept {
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<assert_line_begin, Tail...>) noexcept {
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<assert_line_end, Tail...>) noexcept {
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<boundary<CharacterLike>, Tail...>) noexcept {
	
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<not_boundary<CharacterLike>, Tail...>) noexcept {
	
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
constexpr inline R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] flags f, R & captures, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) noexcept {
	if constexpr ((B != 0) && (A > B)) {
		return not_matched;
	}
	
	[[maybe_unused]] constexpr size_t next_max_level = B ? B - 1 : 0;
	[[maybe_unused]] constexpr bool max_limited = B > 0;
	
	[[maybe_unused]] constexpr size_t next_min_level = A ? A - 1 : 0;
	[[maybe_unused]] constexpr bool min_unlimited = A == 0;
	
	// first try tail...
	if constexpr (min_unlimited) {
		auto result = evaluate_split(begin, current, end, f, captures, ctll::list<Tail...>());
		if (result) return result;
	}
	
	// and then try internal content...
	if constexpr (max_limited && B == 1) {
		// maximum limit reached
		return evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., fail_if_empty, Tail...>());
	} else {
		return evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., fail_if_empty, lazy_repeat<next_min_level, next_max_level, Content...>, Tail...>());
	}
}

// possessive repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] flags f, R & captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {

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

template <typename T> struct identify;

// greedy repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr inline R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] flags f, R & captures, ctll::list<repeat<A,B,Content...>, Tail...>) noexcept {
	if constexpr ((B != 0) && (A > B)) {
		return not_matched;
	}
	
	[[maybe_unused]] constexpr size_t next_max_level = B ? B - 1 : 0;
	[[maybe_unused]] constexpr bool max_limited = B > 0;
	
	[[maybe_unused]] constexpr size_t next_min_level = A ? A - 1 : 0;
	[[maybe_unused]] constexpr bool min_limited = A > 0;
	
	auto try_content = [&](auto && captures){
		if constexpr (max_limited && B == 1) {
			// maximum limit reached
			return evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., fail_if_empty, Tail...>());
		} else {
			return evaluate(begin, current, end, not_empty_match(f), captures, ctll::list<Content..., fail_if_empty, repeat<next_min_level, next_max_level, Content...>, Tail...>());
		}
	};
	
	constexpr auto fcontent = calculate_first(Content{}...);
	constexpr auto ftail = calculate_first(Tail{}...);
	
	// first I try internal content... and if that fail then tail
	if constexpr (collides(fcontent, ftail)) {
		//auto result = try_content(R{captures});
		//if (result) return result;
		auto can_be_content = lookahead_first(begin, current, end, f, fcontent);
		auto can_be_tail = lookahead_first(begin, current, end, f, ftail);
		
		if (can_be_content && !can_be_tail) {
			return try_content(captures);
		} else if (!can_be_content && can_be_tail) {
			if constexpr (!min_limited) {
				return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
			} else {
				return not_matched;
			}
		} else if (!can_be_content && !can_be_tail) {
			return not_matched;
		} else {
			auto result = try_content(R{captures});
			if (result) return result;
			// otherwise it will jump to Tail...
		}
	} else if (lookahead_first(begin, current, end, f, fcontent)) {
		// look ahead success, no need to try tail
		return try_content(captures);
	}
	
	
	if constexpr (!min_limited) {
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// atomic group
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, [[maybe_unused]] flags f, R & captures, ctll::list<atomic_group<Content...>, Tail...>) noexcept {

	auto inner_result = evaluate(begin, current, end, f, captures, ctll::list<Content..., end_mark, accept>());
	
	if (!inner_result) return not_matched;
	
	captures = inner_result.unmatch();
	current = inner_result.get_end_position();
	
	return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
}

// capture (numeric ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// capture end mark (numeric and string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return evaluate(begin, current, end, f, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

// capture (string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<back_reference_with_name<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto result = match_against_range(current, end, ref.begin(), ref.end(), f); result.match) {
			return evaluate(begin, result.position, end, consumed_something(f, ref.begin() != ref.end()), captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

// backreference
template <typename R, size_t Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<back_reference<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto result = match_against_range(current, end, ref.begin(), ref.end(), f); result.match) {
			return evaluate(begin, result.position, end, consumed_something(f, ref.begin() != ref.end()), captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

// end of lookahead
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R & captures, ctll::list<end_lookahead_mark>) noexcept {
	// TODO check interaction with non-empty flag
	return captures.matched();
}

// lookahead positive
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<lookahead_positive<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, f, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		captures = lookahead_result.unmatch();
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// lookahead negative
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, flags f, R & captures, ctll::list<lookahead_negative<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, f, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		return not_matched;
	} else {
		return evaluate(begin, current, end, f, captures, ctll::list<Tail...>());
	}
}


}

#endif
