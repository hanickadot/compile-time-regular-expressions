#ifndef CTRE__EVALUATION__HPP
#define CTRE__EVALUATION__HPP

#include "atoms.hpp"
#include "atoms_characters.hpp"
#include "atoms_unicode.hpp"
#include "utility.hpp"
#include "return_type.hpp"
#include "find_captures.hpp"
#include "first.hpp"

// remove me when MSVC fix the constexpr bug
#ifdef _MSC_VER
#ifndef CTRE_MSVC_GREEDY_WORKAROUND
#define CTRE_MSVC_GREEDY_WORKAROUND
#endif
#endif

namespace ctre {

// calling with pattern prepare stack and triplet of iterators
template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto match_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	return evaluate(begin, begin, end, return_type{}, ctll::list<start_mark, Pattern, assert_end, end_mark, accept>());
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


// sink for making the errors shorter
template <typename R, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R, ...) noexcept {
	return R{};
}

// if we found "accept" object on stack => ACCEPT
template <typename R, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R captures, ctll::list<accept>) noexcept {
	return captures.matched();
}

// if we found "reject" object on stack => REJECT
template <typename R, typename... Rest, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R, ctll::list<reject, Rest...>) noexcept {
	return R{}; // just return not matched return type
}

// mark start of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<start_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_start_mark(current), ctll::list<Tail...>());
}

// mark end of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<end_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_end_mark(current), ctll::list<Tail...>());
}

// mark end of cycle
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator current, const EndIterator, R captures, ctll::list<end_cycle_mark>) noexcept {
	return captures.set_end_mark(current).matched();
}


// matching everything which behave as a one character matcher

template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (end == current) return not_matched;
	if (!CharacterLike::match_char(*current)) return not_matched;
	return evaluate(begin, current+1, end, captures, ctll::list<Tail...>());
}

// matching strings in patterns

template <typename Iterator> struct string_match_result {
	Iterator current;
	bool match;
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

// matching sequence in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailContent) > 0) {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
	} else {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, Tail...>());
	}
}

// matching empty in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<empty, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<assert_end, Tail...>) noexcept {
	if (end != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
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

// possessive repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {

	for (size_t i{0}; (i < B) || (B == 0); ++i) {
		// try as many of inner as possible and then try outer once
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		} else {
			if (i < A && (A != 0)) return not_matched;
			else return evaluate(begin, current, end, captures, ctll::list<Tail...>());
		}
	}
	
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// (gready) repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
#ifdef CTRE_MSVC_GREEDY_WORKAROUND
constexpr inline void evaluate_recursive(R & result, size_t i, const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
#else
constexpr inline R evaluate_recursive(size_t i, const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
#endif
	if ((B == 0) || (i < B)) {
		 
		// a*ab
		// aab
		
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			// TODO MSVC issue:
			// if I uncomment this return it will not fail in constexpr (but the matching result will not be correct)
			//  return inner_result
			// I tried to add all constructors to R but without any success 
			#ifdef CTRE_MSVC_GREEDY_WORKAROUND
			evaluate_recursive(result, i+1, begin, inner_result.get_end_position(), end, inner_result.unmatch(), stack);
			if (result) {
				return;
			}
			#else
			if (auto rec_result = evaluate_recursive(i+1, begin, inner_result.get_end_position(), end, inner_result.unmatch(), stack)) {
				return rec_result;
			}
			#endif
		}
	} 
	#ifdef CTRE_MSVC_GREEDY_WORKAROUND
	result = evaluate(begin, current, end, captures, ctll::list<Tail...>());
	#else
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	#endif
}	


// (gready) repeat optimization
// basic one, if you are at the end of RE, just change it into possessive
// TODO do the same if there is no collision with rest of the RE
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>,assert_end, Tail...>) {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<A,B,Content...>, assert_end, Tail...>());
}

template <typename... T> struct identify_type;

// (greedy) repeat 
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, [[maybe_unused]] ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	// check if it can be optimized
#ifndef CTRE_DISABLE_GREEDY_OPT
	if constexpr (collides(calculate_first(Content{}...), calculate_first(Tail{}...))) {
#endif
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
	#ifdef CTRE_MSVC_GREEDY_WORKAROUND
		R result;
		evaluate_recursive(result, i, begin, current, end, captures, stack);
		return result;
	#else
		return evaluate_recursive(i, begin, current, end, captures, stack);
	#endif
#ifndef CTRE_DISABLE_GREEDY_OPT
	} else {
		// if there is no collision we can go possessive
		return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>());
	}
#endif

}

// repeat lazy_star
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<0,0,Content...>, Tail...>());
}

// repeat (lazy_plus)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<1,0,Content...>, Tail...>());
}

// repeat (possessive_star)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<0,0,Content...>, Tail...>());
}


// repeat (possessive_plus)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<1,0,Content...>, Tail...>());
}

// repeat (greedy) star
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<0,0,Content...>, Tail...>());
}


// repeat (greedy) plus
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<1,0,Content...>, Tail...>());
}

// capture (numeric ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// capture end mark (numeric and string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

// capture (string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// backreference support (match agains content of iterators)
template <typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> match_against_range(Iterator current, const EndIterator end, Iterator range_current, const Iterator range_end) noexcept {
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<back_reference_with_name<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto tmp = match_against_range(current, end, ref.begin(), ref.end()); tmp.match) {
			return evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
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

// end of lookahead
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R captures, ctll::list<end_lookahead_mark>) noexcept {
	return captures.matched();
}

// lookahead positive
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lookahead_positive<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		captures = lookahead_result.unmatch();
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// lookahead negative
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lookahead_negative<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		return not_matched;
	} else {
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	}
}

// property matching


// pattern analysis - returns the minimum and maximum # of characters in order for a regex to match a string
// a custom std::pair to overload some handy operations that we'll perform w/ a fold
struct analysis_results : std::pair<size_t, size_t> {
	// -1 is considered INF, -2 is finite (but perhaps too large to store), all other values are exact counts
	static constexpr CTRE_FORCE_INLINE size_t saturate_limit(const size_t& lhs, const size_t& rhs) {
		const constexpr size_t inf = size_t{ 0 } -1;
		const constexpr size_t lim = size_t{ 0 } -2;
		size_t ret = inf;
		if (lhs == inf || rhs == inf) {
			return ret;
		} else {
			ret = lhs + rhs;
			ret = ret < lhs ? lim : ret == inf ? lim : ret;
		}
		return ret;
	}

	static constexpr CTRE_FORCE_INLINE size_t mult_saturate_limit(const size_t& lhs, const size_t& rhs) {
		const constexpr size_t inf = size_t{ 0 } -1;
		const constexpr size_t lim = size_t{ 0 } -2;
		size_t ret = inf;
		if (lhs == inf || rhs == inf) {
			return ret;
		} else if (lhs == 0 || rhs == 0) {
			return ret = 0;
		} else {
			if (lhs > (SIZE_MAX / rhs))
				return ret = lim;
			ret = lhs * rhs;
			ret = ret == inf ? lim : ret;
			return ret;
		}
	}
	
	constexpr inline CTRE_FORCE_INLINE operator bool() const noexcept {
		return first;
	}
	constexpr auto CTRE_FORCE_INLINE operator+(analysis_results other) const noexcept {
		return analysis_results{std::make_pair(
			saturate_limit(first, other.first),
			saturate_limit(second, other.second)
		)};
	}
	constexpr auto CTRE_FORCE_INLINE operator||(analysis_results other) const noexcept {
		return analysis_results{std::make_pair(
			std::min(first, other.first),
			std::max(second, other.second)
		)};
	}
};

template <typename Pattern>
static constexpr auto trampoline_analysis(Pattern) noexcept;

template <typename... Patterns>
static constexpr auto trampoline_analysis(ctll::list<Patterns...>) noexcept;

template<typename T, typename R>
static constexpr auto trampoline_analysis(T, R captures) noexcept;

//processing for each type

//repeat
template<size_t A, size_t B, typename R, typename... Content>
static constexpr auto _analyze(repeat<A,B,Content...>, R captures) noexcept {
	analysis_results ret{ std::make_pair(0ULL, 0ULL) };
	if constexpr (sizeof...(Content)) {
		ret = trampoline_analysis(ctll::list<Content...>(), captures);
		ret.first = analysis_results::mult_saturate_limit(ret.first, A);
		ret.second = analysis_results::mult_saturate_limit(ret.second, B);
	}
	return ret;
}

//note: all * ? + operations are specialized variations of repeat {A,B}
//lazy_repeat
template<size_t A, size_t B, typename R, typename... Content>
static constexpr auto _analyze(lazy_repeat<A, B, Content...>, R captures) noexcept {
	return _analyze(repeat<A, B, Content...>(), captures);
}

//possessive_repeat
template<size_t A, size_t B, typename R, typename... Content>
static constexpr auto _analyze(possessive_repeat<A, B, Content...>, R captures) noexcept {
	return _analyze(repeat<A, B, Content...>(), captures);
}

//star
template<typename R, typename... Content>
static constexpr auto _analyze(star<Content...>, R captures) noexcept {
	return _analyze(repeat<0ULL, ~(0ULL), Content...>(), captures);
}

//lazy_star
template<typename R, typename... Content>
static constexpr auto _analyze(lazy_star<Content...>, R captures) noexcept {
	return _analyze(repeat<0ULL, ~(0ULL), Content...>(), captures);
}

//possessive_star
template<typename R, typename... Content>
static constexpr auto _analyze(possessive_star<Content...>, R captures) noexcept {
	return _analyze(repeat<0ULL, ~(0ULL), Content...>(), captures);
}

//plus
template<typename R, typename... Content>
static constexpr auto _analyze(plus<Content...>, R captures) noexcept {
	return _analyze(repeat<1ULL, ~(0ULL), Content...>(), captures);
}

//lazy_plus
template<typename R, typename... Content>
static constexpr auto _analyze(lazy_star<Content...>, R captures) noexcept {
	return _analyze(repeat<1ULL, ~(0ULL), Content...>(), captures);
}

//possessive_plus
template<typename R, typename... Content>
static constexpr auto _analyze(possessive_star<Content...>, R captures) noexcept {
	return _analyze(repeat<1ULL, ~(0ULL), Content...>(), captures);
}

//optional
template<typename R, typename... Content>
static constexpr auto _analyze(optional<Content...>, R captures) noexcept {
	return _analyze(repeat<0ULL, 1ULL, Content...>(), captures);
}

//lazy_optional
template<typename R, typename... Content>
static constexpr auto _analyze(lazy_optional<Content...>, R captures) noexcept {
	return _analyze(repeat<0ULL, 1ULL, Content...>(), captures);
}

//back_reference
template<size_t Id, typename R>
static constexpr auto _analyze(back_reference<Id>, R captures) noexcept {
	const auto ref = captures.template get<Id>();
	analysis_results ret{ std::make_pair(0ULL, 0ULL) };
	if constexpr (size(ref.get_expression())) {
		ret = trampoline_analysis(ref.get_expression(), captures);
	}
	return ret;
}

//back_reference_with_name
template<typename Name, typename R>
static constexpr auto _analyze(back_reference_with_name<Name>, R captures) noexcept {
	const auto ref = captures.template get<Name>();
	analysis_results ret{ std::make_pair(0ULL, 0ULL) };
	if constexpr (size(ref.get_expression())) {
		ret = trampoline_analysis(ref.get_expression(), captures);
	}
	return ret;
}

//select, this is specialized, we need to take the minimum of all minimums and maximum of all maximums
template<typename R, typename... Content>
static constexpr auto _analyze(select<Content...>, R captures) noexcept {
	analysis_results ret = trampoline_select_analysis(ctll::list<Content...>(), captures);
	return ret;
}

//character, any character contributes exactly one to both counts
template<auto C, typename R>
static constexpr auto _analyze(character<C>, R captures) noexcept {
	analysis_results ret{ std::make_pair(1ULL, 1ULL) };
	return ret;
}

//strings, any string contributes the # of characters it contains (if we have an empty string that'll be 0)
template<auto... Str, typename R>
static constexpr auto _analyze(string<Str...>, R captures) noexcept {
	analysis_results ret{ std::make_pair(sizeof...(Str), sizeof...(Str)) };
	return ret;
}

//we'll process anything that has contents as a regex
//ctll::list
template<typename R, typename... Content>
static constexpr auto _analyze(ctll::list<Content...>,R captures) noexcept {
	analysis_results ret = trampoline_analysis(ctll::list<Content...>(), captures);
	return ret;
}

//sequence
template<typename R, typename... Content>
static constexpr auto _analyze(sequence<Content...>, R captures) noexcept {
	analysis_results ret = trampoline_analysis(ctll::list<Content...>(), captures);
	return ret;
}

//capture
template<size_t Id, typename R, typename... Content>
static constexpr auto _analyze(capture<Id, Content...>, R captures) noexcept {
	analysis_results ret = trampoline_analysis(ctll::list<Content...>(), captures);
	return ret;
}

//capture_with_name
template<size_t Id, typename Name, typename R, typename... Content>
static constexpr auto _analyze(capture_with_name<Id, Name, Content...>, R captures) noexcept {
	analysis_results ret = trampoline_analysis(ctll::list<Content...>(), captures);
	return ret;
}

//everything else, anything we haven't matched already isn't supported and will contribute 0
template<typename T, typename R>
static constexpr auto _analyze(T, R captures) noexcept {
	analysis_results ret{ std::make_pair(0ULL, 0ULL) };
	return ret;
}
//note: ctll::list wraps patterns just like sequences, we'll treat anything that looks like a regex w/ ctll::list
template <typename... Patterns, typename R>
static constexpr auto trampoline_analysis(ctll::list<Patterns...>, R captures) noexcept {
	//fold, for every argument in a ctll::list, calculate its contribution to the limits
	auto r = ((_analyze(Patterns(), captures)) + ...);
	//note any reordering of parameters will result in the same limits
	return r;
}

template <typename... Patterns, typename R>
static constexpr auto trampoline_select_analysis(ctll::list<Patterns...>, R captures) noexcept {
	//fold, each argument in a selection of regexes we take the minimum and maximum of all values
	auto r = ((trampoline_analysis(Patterns(), captures)) || ...);
	//note again, order is unimportant
	return r;
}

template <typename... Patterns>
static constexpr auto pattern_analysis(ctll::list<Patterns...>) noexcept {
	using return_type = decltype(regex_results(std::declval<std::basic_string_view<char>::iterator>(), find_captures(pattern)));
	return trampoline_analysis(ctll::list<Patterns...>(), return_type{});
}

template <typename Pattern = empty>
static constexpr auto pattern_analysis(Pattern pattern = {}) noexcept {
	using return_type = decltype(regex_results(std::declval<std::basic_string_view<char>::iterator>(), find_captures(pattern)));
	return trampoline_analysis(ctll::list<Pattern>(), return_type{});
}


}

#endif
