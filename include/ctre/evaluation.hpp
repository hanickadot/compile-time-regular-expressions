#ifndef CTRE__EVALUATION__HPP
#define CTRE__EVALUATION__HPP

#include "atoms.hpp"
#include "utility.hpp"
#include "return_type.hpp"
#include "find_captures.hpp"

namespace ctre {

// calling with pattern prepare stack and triplet of iterators
template <typename Iterator, typename Pattern> 
constexpr auto match_re(const Iterator begin, const Iterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	return evaluate(begin, begin, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>());
}

template <typename Iterator, typename Pattern> 
constexpr auto float_match_re(const Iterator begin, const Iterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	for (auto it{begin}; it != end; ++it) {
		if (auto out = evaluate(begin, it, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>())) {
			return out;
		}
	}
	return evaluate(begin, end, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>());
}




// if we found "accept" object on stack => ACCEPT
template <typename R, typename Iterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<accept>) noexcept {
	return captures.matched();
}

// mark start of outer capture
template <typename R, typename Iterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<start_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_start_mark(current), ctll::list<Tail...>());
}

// mark end of outer capture
template <typename R, typename Iterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<end_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_end_mark(current), ctll::list<Tail...>());
}

// mark end of cycle
template <typename R, typename Iterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<end_cycle_mark>) noexcept {
	return captures.set_end_mark(current).matched();
}


// matching everything which behave as a one character matcher

template <typename R, typename Iterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (current == end) return not_matched;
	if (!CharacterLike::match_char(*current)) return not_matched;
	return evaluate(begin, current+1, end, captures, ctll::list<Tail...>());
}

// matching strings in patterns

template <typename Iterator> struct string_match_result {
	Iterator current;
	bool match;
};

template <auto Head, auto... String, typename Iterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> evaluate_match_string(Iterator current, const Iterator end) noexcept {
	if ((current != end) && (Head == *current)) {
		if constexpr (sizeof...(String) > 0) {
			return evaluate_match_string<String...>(++current, end);
		} else {
			return {++current, true};
		}
	} else {
		return {++current, false}; // not needed but will optimize
	}
}

template <typename R, typename Iterator, auto... String, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<string<String...>, Tail...>) noexcept {
	if constexpr (sizeof...(String) == 0) {
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	} else if (auto [it, ok] = evaluate_match_string<String...>(current, end); ok) {
		return evaluate(begin, it, end, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// matching select in patterns
template <typename R, typename Iterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) noexcept {
	if (auto r = evaluate(begin, current, end, captures, ctll::list<HeadOptions, Tail...>())) {
		return r;
	} else {
		return evaluate(begin, current, end, captures, ctll::list<select<TailOptions...>, Tail...>());
	}
}

template <typename R, typename Iterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<select<>, Tail...>) noexcept {
	// no previous option was matched => REJECT
	return not_matched;
}

// matching optional in patterns
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<optional<Content...>, Tail...>) noexcept {
	if (auto r1 = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return r2;
	} else {
		return not_matched;
	}
}

// lazy optional
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<lazy_optional<Content...>, Tail...>) noexcept {
	if (auto r1 = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>())) {
		return r2;
	} else {
		return not_matched;
	}
}

// TODO possessive optional

// matching sequence in patterns
template <typename R, typename Iterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailContent) > 0) {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
	} else {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, Tail...>());
	}
	
}

// matching empty in patterns
template <typename R, typename Iterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<empty, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// matching asserts
template <typename R, typename Iterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<assert_begin, Tail...>) noexcept {
	if (current != begin) {
		return not_matched;
	}
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<assert_end, Tail...>) noexcept {
	if (current != end) {
		return not_matched;
	}
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// lazy repeat
template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) noexcept {
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

// possessive repeat
template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {
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

// (gready) repeat
template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr inline R evaluate_recursive(size_t i, const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	if ((i < B) || (B == 0)) {
		 
		// a*ab
		// aab
		
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			if (auto rec_result = evaluate_recursive(i+1, begin, inner_result.get_end_position(), end, inner_result.unmatch(), stack)) {
				return rec_result;
			}
		}
	} 
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}	

template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
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

// repeat lazy_star
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<lazy_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<0,0,Content...>, Tail...>());
}

// repeat (lazy_plus)
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<lazy_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<1,0,Content...>, Tail...>());
}

// repeat (possessive_star)
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<possessive_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<0,0,Content...>, Tail...>());
}


// repeat (possessive_plus)
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<possessive_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<1,0,Content...>, Tail...>());
}

// repeat (greedy) star
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<0,0,Content...>, Tail...>());
}


// repeat (greedy) plus
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<1,0,Content...>, Tail...>());
}

// capture (numeric ID)
template <typename R, typename Iterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// capture end mark (numeric and string ID)
template <typename R, typename Iterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

// capture (string ID)
template <typename R, typename Iterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const Iterator end, R captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}




}

#endif
