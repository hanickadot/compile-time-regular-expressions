#ifndef CTRE__EVALUATION__HPP
#define CTRE__EVALUATION__HPP

#include "atoms.hpp"
#include "utility.hpp"
#include "return_type.hpp"
#include "find_captures.hpp"

namespace ctre {

// calling with pattern prepare stack and triplet of iterators
template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto match_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	return evaluate(begin, begin, end, return_type{}, ctll::list<start_mark, Pattern, assert_end, end_mark, accept>());
}

template <typename Iterator, typename EndIterator, typename Pattern>
constexpr inline auto ordered_match_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	return ordered_evaluate(begin, begin, end, return_type{}, ctll::list<start_mark, Pattern, assert_end, end_mark, accept>());
}

template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto search_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	
	auto it = begin;
	for (; end != it; ++it) {
		if (auto out = evaluate(begin, it, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>())) {
			return out;
		}
	}
	
	// in case the RE is empty
	return evaluate(begin, it, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>());
}




// if we found "accept" object on stack => ACCEPT
template <typename R, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R captures, ctll::list<accept>) noexcept {
	return captures.matched();
}

template <typename R, typename Iterator, typename EndIterator>
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator, Iterator, const EndIterator, R captures, ctll::list<accept>) noexcept {
	return captures.matched();
}

// mark start of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<start_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_start_mark(current), ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail>
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<start_mark, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures.set_start_mark(current), ctll::list<Tail...>());
}

// mark end of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<end_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_end_mark(current), ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail>
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<end_mark, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures.set_end_mark(current), ctll::list<Tail...>());
}

// mark end of cycle
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator current, const EndIterator, R captures, ctll::list<end_cycle_mark>) noexcept {
	return captures.set_end_mark(current).matched();
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator, Iterator current, const EndIterator, R captures, ctll::list<end_cycle_mark>) noexcept {
	return captures.set_end_mark(current);
}


// matching everything which behave as a one character matcher

template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (end == current) return not_matched;
	if (!CharacterLike::match_char(*current)) return not_matched;
	return evaluate(begin, current+1, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>>
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (end == current) { // target is shorter than pattern
		captures.mask_elg({0,1,0});
		return captures;
	}
	captures.mask_elg(CharacterLike::compare_char(*current));
	if (captures) {
		if(!captures.is_less() && !captures.is_greater())
			return evaluate(begin, current+1, end, captures.unmatch(), ctll::list<Tail...>());
		return ordered_evaluate(begin, current+1, end, captures, ctll::list<Tail...>());
	}
	return captures;
}

// matching strings in patterns

template <typename Iterator> struct string_match_result {
	Iterator current;
	bool match;
};

template <typename Iterator> struct string_compare_result {
	Iterator current;
	equal_less_greater elg;
};

template <auto Head, auto... String, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> evaluate_match_string(Iterator current, const EndIterator end) noexcept {
	if ((end != current) && (Head == *current)) {
		if constexpr (sizeof...(String) > 0) {
			return evaluate_match_string<String...>(++current, end);
		} else {
			return {++current, true};
		}
	} else {
		return {++current, false}; // not needed but will optimize
	}
}

template <auto Head, auto... String, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_compare_result<Iterator> evaluate_compare_string(Iterator current, const EndIterator end) noexcept {
	if (end == current) { // target is shorter than pattern (string)
		return {current, {0, 1, 0}};
	}
	if (Head < *current) return {++current, {0, 0, 1}};
	if (Head > *current) return {++current, {0, 1, 0}};
	if constexpr (sizeof...(String) > 0) {
		return evaluate_compare_string<String...>(++current, end);
	} else {
		return {++current, {1, 1, 1}};
	}
}

template <typename R, typename Iterator, typename EndIterator, auto... String, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<string<String...>, Tail...>) noexcept {
	if constexpr (sizeof...(String) == 0) {
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	} else if (auto tmp = evaluate_match_string<String...>(current, end); tmp.match) {
		return evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

template <typename R, typename Iterator, typename EndIterator, auto... String, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<string<String...>, Tail...>) noexcept {
	if constexpr (sizeof...(String) == 0) {
		return ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
	} else if (auto tmp = evaluate_compare_string<String...>(current, end); tmp.elg.equal) {
		return ordered_evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
	} else {
		captures.mask_elg(tmp.elg);
		return captures;
	}
}

// matching select in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) noexcept {
	if (auto r = evaluate(begin, current, end, captures, ctll::list<HeadOptions, Tail...>())) {
		return r;
	} else {
		return evaluate(begin, current, end, captures, ctll::list<select<TailOptions...>, Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R, ctll::list<select<>, Tail...>) noexcept {
	// no previous option was matched => REJECT
	return not_matched;
}

template <typename R, typename Iterator, typename EndIterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) noexcept {
	auto r = ordered_evaluate(begin, current, end, captures, ctll::list<HeadOptions, Tail...>());
	if (r) { return r; }
	captures.mask_lg(r);
	return ordered_evaluate(begin, current, end, captures, ctll::list<select<TailOptions...>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator, Iterator, const EndIterator, R captures, ctll::list<select<>, Tail...>) noexcept {
	// no previous option was matched => REJECT
	captures.mask_elg({0,1,1});
	return captures;
}

// matching optional in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<optional<Content...>, Tail...>) noexcept {
	if (auto r1 = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return r2;
	} else {
		return not_matched;
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<optional<Content...>, Tail...>) noexcept {
	auto r1 = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>());
	if (r1) { return r1; }
	auto r2 = ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
	if (r2) { return r2; }
	captures.mask_elg(r1);
	captures.mask_elg(r2);
	return captures;
}

// lazy optional
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_optional<Content...>, Tail...>) noexcept {
	if (auto r1 = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>())) {
		return r2;
	} else {
		return not_matched;
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_optional<Content...>, Tail...>) noexcept {
	auto r1 = ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
	if (r1) { return r1; }
	auto r2 = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>());
	if (r2) { return r2; }
	captures.mask_elg(r1);
	captures.mask_elg(r2);
	return captures;
}

// matching sequence in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailContent) > 0) {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
	} else {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailContent) > 0) {
		return ordered_evaluate(begin, current, end, captures, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
	} else {
		return ordered_evaluate(begin, current, end, captures, ctll::list<HeadContent, Tail...>());
	}
	
}

// matching empty in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<empty, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<empty, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// matching asserts
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<assert_begin, Tail...>) noexcept {
	if (begin != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<assert_begin, Tail...>) noexcept {
	if (begin != current) {
		captures.mask_elg({0,1,1});
		return captures;
	}
	return ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<assert_end, Tail...>) noexcept {
	if (end != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<assert_end, Tail...>) noexcept {
	if (end != current) { // target is longer than pattern
		captures.mask_elg({0,0,1});
		return captures;
	}
	return ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// lazy repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) noexcept {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto outer_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			captures = outer_result.unmatch();
			current = outer_result.get_end_position();
		} else {
			return not_matched;
		}
	}
	
	if (auto outer_result = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return outer_result;
	} else {
		for (; (i < B) || (B == 0); ++i) {
			if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
				if (auto outer_result = evaluate(begin, inner_result.get_end_position(), end, inner_result.unmatch(), ctll::list<Tail...>())) {
					return outer_result;
				} else {
					captures = inner_result.unmatch();
					current = inner_result.get_end_position();
					continue;
				}
			} else {
				return not_matched;
			}
		}
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) noexcept {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (captures = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			current = captures.get_end_position();
		} else {
			return captures;
		}
	}
	
	if (auto outer_result = ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return outer_result;
	} else {
		captures.mask_lg(outer_result);
		for (; (i < B) || (B == 0); ++i) {
			if (auto inner_result = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
				if (captures = ordered_evaluate(begin, inner_result.get_end_position(), end, inner_result.unmatch(), ctll::list<Tail...>())) {
					return captures;
				} else {
					captures = inner_result;
					current = inner_result.get_end_position();
					continue;
				}
			} else {
				return inner_result;
			}
		}
		return ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
	}
}

// possessive repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		} else {
			return not_matched;
		}
	}
	
	for (; (i < B) || (B == 0); ++i) {
		// try as many of inner as possible and then try outer once
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			current = inner_result.get_end_position();
		} else {
			return evaluate(begin, current, end, captures, ctll::list<Tail...>());
		}
	}
	
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (captures = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			current = captures.get_end_position();
		} else {
			return captures;
		}
	}
	
	for (; (i < B) || (B == 0); ++i) {
		// try as many of inner as possible and then try outer once

		// I have to mask this ordering beforehand, as we can't backtrack later.
		// This basically throws away the usual runtime benefits of a possessive repeat.
		auto outer_result = ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
		captures.mask_lg(outer_result);

		auto inner_result = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>());
		captures.mask_lg(inner_result);
		if (inner_result) {
			current = inner_result.get_end_position();
		} else {
			return outer_result;
		}
	}
	
	return ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// (gready) repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr inline R evaluate_recursive(size_t i, const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	if ((i < B) || (B == 0)) {
		 
		// a*ab
		// aab
		
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			// TODO MSVC issue:
			// if I uncomment this return it will not fail in constexpr (but the matching result will not be correct)
			//  return inner_result
			// I tried to add all constructors to R but without any success 
			if (auto rec_result = evaluate_recursive(i+1, begin, inner_result.get_end_position(), end, inner_result.unmatch(), stack)) {
				return rec_result;
			}
		}
	} 
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}	

template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		} else {
			return not_matched;
		}
	}
	
	return evaluate_recursive(i, begin, current, end, captures, stack);
}

template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr inline R ordered_evaluate_recursive(size_t i, const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	if ((i < B) || (B == 0)) {
		auto inner_result = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>());
		if (inner_result) {
			// TODO MSVC issue:
			// if I uncomment this return it will not fail in constexpr (but the matching result will not be correct)
			//  return inner_result
			// I tried to add all constructors to R but without any success 
			auto rec_result = ordered_evaluate_recursive(i+1, begin, inner_result.get_end_position(), end, inner_result, stack);
			if (rec_result) { return rec_result; }
			captures.mask_lg(rec_result);
		}
		captures.mask_lg(inner_result);
	}
	return ordered_evaluate(begin, current, end, captures, ctll::list<Tail...>());
}	

template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		captures = ordered_evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>());
		if (captures) {
			current = captures.get_end_position();
		} else {
			return captures;
		}
	}
	
	return ordered_evaluate_recursive(i, begin, current, end, captures, stack);
}

// repeat lazy_star
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<0,0,Content...>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_star<Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures, ctll::list<lazy_repeat<0,0,Content...>, Tail...>());
}

// repeat (lazy_plus)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<1,0,Content...>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_plus<Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures, ctll::list<lazy_repeat<1,0,Content...>, Tail...>());
}

// repeat (possessive_star)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<0,0,Content...>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_star<Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures, ctll::list<possessive_repeat<0,0,Content...>, Tail...>());
}


// repeat (possessive_plus)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<1,0,Content...>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_plus<Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures, ctll::list<possessive_repeat<1,0,Content...>, Tail...>());
}

// repeat (greedy) star
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<0,0,Content...>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<star<Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures, ctll::list<repeat<0,0,Content...>, Tail...>());
}


// repeat (greedy) plus
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<1,0,Content...>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<plus<Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures, ctll::list<repeat<1,0,Content...>, Tail...>());
}

// capture (numeric ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// capture end mark (numeric and string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

// capture (string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
	return ordered_evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// backreference support (match agains content of iterators)
template <typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> match_against_range(Iterator current, const EndIterator end, Iterator range_current, const Iterator range_end) noexcept {
	while (current != end && range_current != range_end) {
		if (*current == *range_current) {
			current++;
			range_current++;
		} else {
			return {current, false};
		}
	}
	return {current, range_current == range_end};
}

template <typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_compare_result<Iterator> compare_against_range(Iterator current, const EndIterator end, Iterator range_current, const Iterator range_end) noexcept {
	while (current != end && range_current != range_end) {
		if (*current == *range_current) {
			current++;
			range_current++;
		} else {
			return {current, {0, *current < *range_current, *current > *range_current}};
		}
	}
	return {current, {range_current == range_end, 1, range_current == range_end}};
}

// backreference with name
template <typename R, typename Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<back_reference_with_name<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto tmp = match_against_range(current, end, ref.begin(), ref.end()); tmp.match) {
			return evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

template <typename R, typename Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<back_reference_with_name<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		auto tmp = compare_against_range(current, end, ref.begin(), ref.end());
		captures.mask_elg(tmp.elg);
		if (tmp.elg.equal) {
			return ordered_evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
		}
	}
	captures.mask_elg({0,1,1});
	return captures;
}

// backreference
template <typename R, size_t Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<back_reference<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto tmp = match_against_range(current, end, ref.begin(), ref.end()); tmp.match) {
			return evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

template <typename R, size_t Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R ordered_evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<back_reference<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		auto tmp = compare_against_range(current, end, ref.begin(), ref.end());
		captures.mask_elg(tmp.elg);
		if (tmp.elg.equal) {
			return ordered_evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
		}
	}
	captures.mask_elg({0,1,1});
	return captures;
}


}

#endif
