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

template <size_t Limit> constexpr CTRE_FORCE_INLINE bool less_than_or_infinite(size_t i) noexcept {
	if constexpr (Limit == 0) {
		// infinite
		return true;
	} else {
		return i < Limit;
	}
}

template <size_t Limit> constexpr CTRE_FORCE_INLINE bool less_than(size_t i) noexcept {
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R, ...) noexcept {
	return not_matched;
}

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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<start_mark, Tail...>) noexcept {
	return evaluate(begin, current, last, f, captures.set_start_mark(current), ctll::list<Tail...>());
}

// mark end of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<end_mark, Tail...>) noexcept {
	return evaluate(begin, current, last, f, captures.set_end_mark(current), ctll::list<Tail...>());
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (current == last) return not_matched;
	if (!CharacterLike::match_char(*current)) return not_matched;
	
	return evaluate(begin, ++current, last, consumed_something(f), captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<any, Tail...>) noexcept {
	if (current == last) return not_matched;
	
	if (multiline_mode(f)) {
		// TODO add support for different line ending and unicode (in a future unicode mode)
		if (*current == '\n') return not_matched;
	}
	return evaluate(begin, ++current, last, consumed_something(f), captures, ctll::list<Tail...>());
}

// matching strings in patterns

template <typename Iterator> struct string_match_result {
	Iterator position;
	bool match;
};

template <typename CharT, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE bool compare_character(CharT c, Iterator & it, const EndIterator & last) noexcept {
	if (it != last) {
		using char_type = decltype(*it);
		return *it++ == static_cast<char_type>(c);
	}
	return false;
}

template <auto... String, size_t... Idx, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> evaluate_match_string(Iterator current, [[maybe_unused]] const EndIterator last, std::index_sequence<Idx...>) noexcept {

	bool same = (compare_character(String, current, last) && ... && true);

	return {current, same};
}

template <typename R, typename Iterator, typename EndIterator, auto... String, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, [[maybe_unused]] const flags & f, R captures, ctll::list<string<String...>, Tail...>) noexcept {
	auto result = evaluate_match_string<String...>(current, last, std::make_index_sequence<sizeof...(String)>());
	
	if (!result.match) {
		return not_matched;
	}
	
	return evaluate(begin, result.position, last, consumed_something(f, sizeof...(String) > 0), captures, ctll::list<Tail...>());
}

template <typename T, typename... Ts> constexpr auto collides_with(T, ctll::list<Ts...>) noexcept {
	return decltype((ctll::list<>{} + ... + ctll::item_matcher<Ts>::pick(
		typename std::conditional<collides(calculate_first(sequence<T>{}), calculate_first(sequence<Ts>{})), std::true_type, std::false_type>::type{}
	))){};
}

template <typename T, typename... Ts> constexpr auto collides_with_negated(T, ctll::list<Ts...>) noexcept {
	return decltype((ctll::list<>{} + ... + ctll::item_matcher<Ts>::pick(
		typename std::conditional<collides(ctre::negative_set<decltype(calculate_first(sequence<T>{}))>{}, calculate_first(sequence<Ts>{})), std::true_type, std::false_type>::type{}
	))){};
}

template <typename T, typename... Ts> constexpr auto mutually_exclusive_with(T, ctll::list<Ts...>) noexcept {
	return decltype((ctll::list<>{} + ... + ctll::item_matcher<Ts>::pick(
		typename std::conditional<!collides(calculate_first(sequence<T>{}), calculate_first(sequence<Ts>{})), std::true_type, std::false_type>::type{}
	))){};
}

namespace detail {
	template <typename... Content>
	constexpr auto transform_into_set(ctll::list<Content...>) {
		return ctre::set<decltype(Content{})...>{};
		//return ctre::set<decltype(transform_into_set(Content{}))... > {};
	}

	template <typename T>
	constexpr auto transform_into_set(T) {
		return T{};
	}

	template<>
	constexpr auto transform_into_set(can_be_anything) {
		return ctre::char_range<std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()>{};
	}

	template <typename... Content>
	constexpr auto transform_into_select(ctll::list<Content...>) {
		return ctre::select<decltype(Content{})...>{};
	}
}

// matching select in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailOptions) > 1) {
		constexpr auto collision_list = collides_with(sequence<HeadOptions, Tail...>{}, ctll::list<
			decltype(sequence<TailOptions, Tail...>{})...
		>{});

		if constexpr (ctll::empty(collision_list)) {
			using set_type = decltype(detail::transform_into_set(calculate_first(sequence<HeadOptions, Tail...>{})));
			if constexpr (::std::is_same_v<set<>, set_type>) {
				//fail handle as normal
				if (auto r = evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>())) {
					return r;
				} else {
					return evaluate(begin, current, last, f, captures, ctll::list<select<TailOptions...>, Tail...>());
				}
			} else {
				//ok optimize into exclusive select
				if (auto r = evaluate(begin, current, last, f, captures, ctll::list<set_type, end_cycle_mark>{})) {
					return evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>{});
				} else {
					return evaluate(begin, current, last, f, captures, ctll::list<select<TailOptions...>, Tail...>());
				}
			}

			return evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>());
		} else if constexpr (ctll::size(collision_list) == sizeof...(TailOptions)) {
			//continue as normal...we collided with everything
			if (auto r = evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>())) {
				return r;
			} else {
				return evaluate(begin, current, last, f, captures, ctll::list<select<TailOptions...>, Tail...>());
			}
		} else {
			//we collided with some things, but not others, bifricate on the first character we see
			//may be less computationally expensive to do a set subtraction from the complete list with the collision list, because we're evaluating collisions again
			constexpr auto negated_collision_list = collides_with_negated(sequence<HeadOptions, Tail...>{}, ctll::list<
				decltype(sequence<TailOptions, Tail...>{})...
			>{});

			using set_type = decltype(detail::transform_into_set(calculate_first(sequence<HeadOptions, Tail...>{})));
			if constexpr (::std::is_same_v<set<>, set_type>) {
				//fail
				if (auto r = evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>())) {
					return r;
				} else {
					return evaluate(begin, current, last, f, captures, ctll::list<non_exclusive_select<TailOptions...>, Tail...>());
				}
			} else {
				//ok optimize into a split
				if (auto r = evaluate(begin, current, last, f, captures, ctll::list<set_type, end_cycle_mark>{})) {
					if (auto r2 = evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>{})) {
						return r2;
					} else {
						return evaluate(begin, current, last, f, captures, detail::transform_into_select(collision_list));
					}
				} else {
					return evaluate(begin, current, last, f, captures, detail::transform_into_select(negated_collision_list));
				}
			}
		}
	} else {
		//simple case, too few branches to pick handle as normal
		if (auto r = evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>())) {
			return r;
		} else {
			return evaluate(begin, current, last, f, captures, ctll::list<select<TailOptions...>, Tail...>());
		}
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R, ctll::list<select<>, Tail...>) noexcept {
	// no previous option was matched => REJECT
	return not_matched;
}

// non exclusive select (assume collisions)
template <typename R, typename Iterator, typename EndIterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<non_exclusive_select<HeadOptions, TailOptions...>, Tail...>) noexcept {
	if (auto r = evaluate(begin, current, last, f, captures, ctll::list<HeadOptions, Tail...>())) {
		return r;
	} else {
		return evaluate(begin, current, last, f, captures, ctll::list<non_exclusive_select<TailOptions...>, Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail>
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, flags, R, ctll::list<non_exclusive_select<>, Tail...>) noexcept {
	// no previous option was matched => REJECT
	return not_matched;
}

// matching sequence in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailContent) > 0) {
		return evaluate(begin, current, last, f, captures, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
	} else {
		return evaluate(begin, current, last, f, captures, ctll::list<HeadContent, Tail...>());
	}
}

// matching empty in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<empty, Tail...>) noexcept {
	return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
}

// matching asserts
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<assert_subject_begin, Tail...>) noexcept {
	if (begin != current) {
		return not_matched;
	}
	return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<assert_subject_end, Tail...>) noexcept {
	if (last != current) {
		return not_matched;
	}
	return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<assert_subject_end_line, Tail...>) noexcept {
	if (multiline_mode(f)) {
		if (last == current) {
			return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
		} else if (*current == '\n' && std::next(current) == last) {
			return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
		} else {
			return not_matched;
		}
	} else {
		if (last != current) {
			return not_matched;
		}
		return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<assert_line_begin, Tail...>) noexcept {
	if (multiline_mode(f)) {
		if (begin == current) {
			return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
		} else if (*std::prev(current) == '\n') {
			return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
		} else {
			return not_matched;
		}
	} else {
		if (begin != current) {
			return not_matched;
		}
		return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<assert_line_end, Tail...>) noexcept {
	if (multiline_mode(f)) {
		if (last == current) {
			return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
		} else if (*current == '\n') {
			return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
		} else {
			return not_matched;
		}
	} else {
		if (last != current) {
			return not_matched;
		}
		return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
	}
	
	// TODO properly match line end
	if (last != current) {
		return not_matched;
	}
	return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
}

// matching boundary
template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<boundary<CharacterLike>, Tail...>) noexcept {
	
	// reason why I need bidirectional iterators or some clever hack
	bool before = false;
	bool after = false;
	
	static_assert(is_bidirectional(typename std::iterator_traits<Iterator>::iterator_category{}), "To use boundary in regex you need to provide bidirectional iterator or range.");
	
	if (last != current) {
		after = CharacterLike::match_char(*current);
	}
	if (begin != current) {
		before = CharacterLike::match_char(*std::prev(current));
	}
	
	if (before == after) return not_matched;
	
	return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
}

// matching not_boundary
template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<not_boundary<CharacterLike>, Tail...>) noexcept {
	
	// reason why I need bidirectional iterators or some clever hack
	bool before = false;
	bool after = false;
	
	static_assert(is_bidirectional(typename std::iterator_traits<Iterator>::iterator_category{}), "To use boundary in regex you need to provide bidirectional iterator or range.");
	
	if (last != current) {
		after = CharacterLike::match_char(*current);
	}
	if (begin != current) {
		before = CharacterLike::match_char(*std::prev(current));
	}
	
	if (before != after) return not_matched;
	
	return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
}

// lazy repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, [[maybe_unused]] const flags & f, R captures, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) noexcept {

	if constexpr (B != 0 && A > B) {
		return not_matched;
	} else {
		const Iterator backup_current = current;
	
		size_t i{0};
	
		while (less_than<A>(i)) {
			auto outer_result = evaluate(begin, current, last, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
			if (!outer_result) return not_matched;
		
			captures = outer_result.unmatch();
			current = outer_result.get_end_position();
		
			++i;
		}
	
		if (auto outer_result = evaluate(begin, current, last, consumed_something(f, backup_current != current), captures, ctll::list<Tail...>())) {
			return outer_result;
		}
	
		while (less_than_or_infinite<B>(i)) {
			auto inner_result = evaluate(begin, current, last, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
			if (!inner_result) return not_matched;
		
			auto outer_result = evaluate(begin, inner_result.get_end_position(), last, consumed_something(f), inner_result.unmatch(), ctll::list<Tail...>());
		
			if (outer_result) {
				return outer_result;
			}
		
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		
			++i;
		}
	
		// rest of regex
		return evaluate(begin, current, last, consumed_something(f), captures, ctll::list<Tail...>());
	}
}

// possessive repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, [[maybe_unused]] const flags & f, R captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {

	if constexpr ((B != 0) && (A > B)) {
		return not_matched;
	} else {
		const auto backup_current = current;

		for (size_t i{0}; less_than_or_infinite<B>(i); ++i) {
			// try as many of inner as possible and then try outer once
			auto inner_result = evaluate(begin, current, last, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
			if (!inner_result) {
				if (!less_than<A>(i)) break;
				return not_matched;
			}
		
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		}
	
		return evaluate(begin, current, last, consumed_something(f, backup_current != current), captures, ctll::list<Tail...>());
	}
}

// (gready) repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
#ifdef CTRE_MSVC_GREEDY_WORKAROUND
constexpr inline void evaluate_recursive(R & result, size_t i, const Iterator begin, Iterator current, const EndIterator last, [[maybe_unused]] const flags & f, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) noexcept {
#else
constexpr inline R evaluate_recursive(size_t i, const Iterator begin, Iterator current, const EndIterator last, [[maybe_unused]] const flags & f, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) noexcept {
#endif
	if (less_than_or_infinite<B>(i)) {
		 
		// a*ab
		// aab
		
		if (auto inner_result = evaluate(begin, current, last, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>())) {
			// TODO MSVC issue:
			// if I uncomment this return it will not fail in constexpr (but the matching result will not be correct)
			//  return inner_result
			// I tried to add all constructors to R but without any success
			auto tmp_current = current;
			tmp_current = inner_result.get_end_position();
			#ifdef CTRE_MSVC_GREEDY_WORKAROUND
			evaluate_recursive(result, i+1, begin, tmp_current, last, f, inner_result.unmatch(), stack);
			if (result) {
				return;
			}
			#else
			if (auto rec_result = evaluate_recursive(i+1, begin, tmp_current, last, f, inner_result.unmatch(), stack)) {
				return rec_result;
			}
			#endif
		}
	}
	#ifdef CTRE_MSVC_GREEDY_WORKAROUND
	result = evaluate(begin, current, last, consumed_something(f), captures, ctll::list<Tail...>());
	#else
	return evaluate(begin, current, last, consumed_something(f), captures, ctll::list<Tail...>());
	#endif
}	



// (greedy) repeat 
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, [[maybe_unused]] const flags & f, R captures, [[maybe_unused]] ctll::list<repeat<A,B,Content...>, Tail...> stack) {

	if constexpr ((B != 0) && (A > B)) {
		return not_matched;
	}

#ifndef CTRE_DISABLE_GREEDY_OPT
	else if constexpr (!collides(calculate_first(Content{}...), calculate_first(Tail{}...))) {
		return evaluate(begin, current, last, f, captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>());
	}
#endif
	else {
		// A..B
		size_t i{0};
		while (less_than<A>(i)) {
			auto inner_result = evaluate(begin, current, last, not_empty_match(f), captures, ctll::list<Content..., end_cycle_mark>());
		
			if (!inner_result) return not_matched;
		
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		
			++i;
		}
	
#ifdef CTRE_MSVC_GREEDY_WORKAROUND
		R result;
		evaluate_recursive(result, i, begin, current, last, f, captures, stack);
		return result;
#else
		return evaluate_recursive(i, begin, current, last, f, captures, stack);
#endif
	}

}

// capture (numeric ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, last, f, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// capture end mark (numeric and string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return evaluate(begin, current, last, f, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

// capture (string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, last, f, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// backreference support (match agains content of iterators)
template <typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> match_against_range(Iterator current, const EndIterator last, Iterator range_current, const Iterator range_end, flags) noexcept {
	while (last != current && range_end != range_current) {
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<back_reference_with_name<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto result = match_against_range(current, last, ref.begin(), ref.end(), f); result.match) {
			return evaluate(begin, result.position, last, consumed_something(f, ref.begin() != ref.end()), captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

// backreference
template <typename R, size_t Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<back_reference<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto result = match_against_range(current, last, ref.begin(), ref.end(), f); result.match) {
			return evaluate(begin, result.position, last, consumed_something(f, ref.begin() != ref.end()), captures, ctll::list<Tail...>());
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
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<lookahead_positive<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, last, f, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		captures = lookahead_result.unmatch();
		return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// lookahead negative
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator last, const flags & f, R captures, ctll::list<lookahead_negative<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, last, f, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		return not_matched;
	} else {
		return evaluate(begin, current, last, f, captures, ctll::list<Tail...>());
	}
}


}

#endif
