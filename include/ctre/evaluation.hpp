#ifndef CTRE__EVALUATION__HPP
#define CTRE__EVALUATION__HPP

#include "atoms.hpp"

namespace ctre {

template <typename Iterator, typename R = void> struct eval_result {
	bool _matched;
	Iterator _position;
	
	R _result{};
	constexpr eval_result(bool m, Iterator p) noexcept: _matched{m}, _position{p} { }
	constexpr eval_result(bool m, Iterator p, R r) noexcept: _matched{m}, _position{p}, _result{r} { }
	constexpr operator bool() const noexcept {
		return _matched;
	}
};

template <typename Iterator> struct eval_result<Iterator, void> {
	bool _matched;
	Iterator _position;
	
	constexpr eval_result(bool m, Iterator p) noexcept: _matched{m}, _position{p} { }
	constexpr operator bool() const noexcept {
		return _matched;
	}
};

// calling with pattern prepare stack and triplet of iterators
template <typename Iterator, typename Pattern> 
constexpr auto evaluate(const Iterator begin, const Iterator end, Pattern pattern) {
	using return_type = eval_result<Iterator, void>;
	return evaluate<return_type>(begin, begin, end, ctll::list<Pattern, accept>());
}

// if we found "accept" object on stack => ACCEPT
template <typename R, typename Iterator> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<accept>) {
	return R{true, current};
}

// matching character like parts of patterns

template <typename T> class MatchesCharacter {
	template <typename Y, typename CharT> static auto test(Y*, CharT c) -> decltype(Y::match_char(c), std::true_type());
	template <typename> static auto test(...) -> std::false_type;
public:
	template <typename CharT> static inline constexpr bool value = decltype(test<T>(nullptr, std::declval<CharT>()))();
};

template <typename R, typename Iterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<CharacterLike, Tail...>) {
	if (current == end) return R{false, current};
	if (!CharacterLike::match_char(*current)) return R{false, current};
	return evaluate<R>(begin, current+1, end, ctll::list<Tail...>());
}

// matching strings in patterns

template <typename R, typename Iterator, auto... String, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<string<String...>, Tail...>) {
	auto match = [&current, end](auto c) constexpr -> bool {
		return (current != end) && (c == *current++);
	};
	if (!(match(String) && ... && true)) return R{false, current};
	return evaluate<R>(begin, current, end, ctll::list<Tail...>());
}

// matching select in patterns
template <typename R, typename Iterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) {
	if (auto r = evaluate<R>(begin, current, end, ctll::list<HeadOptions, Tail...>())) {
		return r;
	} else {
		return evaluate<R>(begin, current, end, ctll::list<select<TailOptions...>, Tail...>());
	}
}

template <typename R, typename Iterator, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<select<>, Tail...>) {
	// no previous option was matched => REJECT
	return R{false, current};
}

// matching optional in patterns
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<optional<Content...>, Tail...>) {
	if (auto r1 = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate<R>(begin, current, end, ctll::list<Tail...>())) {
		return r2;
	} else {
		return R{false, current};
	}
}

// lazy optional
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<lazy_optional<Content...>, Tail...>) {
	if (auto r1 = evaluate<R>(begin, current, end, ctll::list<Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, Tail...>())) {
		return r2;
	} else {
		return R{false, current};
	}
}

// TODO possessive optional

// matching sequence in patterns
template <typename R, typename Iterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
}

// empty sequence should be skipped
template <typename R, typename Iterator, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<sequence<>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<Tail...>());
}

// matching empty in patterns
template <typename R, typename Iterator, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<empty, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<Tail...>());
}

// matching asserts
template <typename R, typename Iterator, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<assert_begin, Tail...>) {
	if (current == begin) {
		return evaluate<R>(begin, current, end, ctll::list<Tail...>());
	}
	return R{false, current};
}

template <typename R, typename Iterator, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<assert_end, Tail...>) {
	if (current == end) {
		return evaluate<R>(begin, current, end, ctll::list<Tail...>());
	}
	return R{false, current};
}

// lazy repeat
template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto inner_result = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, accept>())) {
			current = inner_result._position;
		} else {
			return R{false, current};
		}
	}
	
	if (auto outer_result = evaluate<R>(begin, current, end, ctll::list<Tail...>())) {
		return outer_result;
	} else {
		for (; (i < B) || (B == 0); ++i) {
			if (auto inner_result = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, accept>())) {
				if (auto outer_result = evaluate<R>(begin, inner_result._position, end, ctll::list<Tail...>())) {
					return outer_result;
				} else {
					current = inner_result._position;
					continue;
				}
			} else {
				return R{false, current};
			}
		}
		return evaluate<R>(begin, current, end, ctll::list<Tail...>());
	}
}

// possessive repeat
template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto inner_result = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, accept>())) {
			current = inner_result._position;
		} else {
			return R{false, current};
		}
	}
	
	for (; (i < B) || (B == 0); ++i) {
		// try as many of inner as possible and then try outer once
		if (auto inner_result = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, accept>())) {
			current = inner_result._position;
		} else {
			return evaluate<R>(begin, current, end, ctll::list<Tail...>());
		}
	}
	
	return evaluate<R>(begin, current, end, ctll::list<Tail...>());
}

// (gready) repeat
template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr R evaluate_recursive(size_t i, const Iterator begin, Iterator current, const Iterator end, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	if ((i < B) || (B == 0)) {
		 
		// a*ab
		// aab
		
		if (auto inner_result = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, accept>())) {
			if (auto rec_result = evaluate_recursive<R>(i+1, begin, inner_result._position, end, stack)) {
				return rec_result;
			}
		}
	} 
	return evaluate<R>(begin, current, end, ctll::list<Tail...>());
}	

template <typename R, typename Iterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto inner_result = evaluate<R>(begin, current, end, ctll::list<sequence<Content...>, accept>())) {
			current = inner_result._position;
		} else {
			return R{false, current};
		}
	}
	
	return evaluate_recursive<R>(i, begin, current, end, stack);
}

// repeat lazy_star
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<lazy_star<Content...>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<lazy_repeat<0,0,Content...>, Tail...>());
}

// repeat (lazy_plus)
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<lazy_plus<Content...>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<lazy_repeat<1,0,Content...>, Tail...>());
}

// repeat (possessive_star)
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<possessive_star<Content...>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<possessive_repeat<0,0,Content...>, Tail...>());
}


// repeat (possessive_plus)
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<possessive_plus<Content...>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<possessive_repeat<1,0,Content...>, Tail...>());
}

// repeat (greedy) star
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<star<Content...>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<repeat<0,0,Content...>, Tail...>());
}


// repeat (greedy) plus
template <typename R, typename Iterator, typename... Content, typename... Tail> 
constexpr R evaluate(const Iterator begin, Iterator current, const Iterator end, ctll::list<plus<Content...>, Tail...>) {
	return evaluate<R>(begin, current, end, ctll::list<repeat<1,0,Content...>, Tail...>());
}


}

#endif
