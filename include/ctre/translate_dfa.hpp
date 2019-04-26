#ifndef CTRE__TRANSLATE_DFA__HPP
#define CTRE__TRANSLATE_DFA__HPP

#include "../ctfa.hpp"
#include <type_traits>

namespace ctre {
	
struct unsupported_pattern_tag { };

static constexpr inline auto unsupported_pattern = unsupported_pattern_tag{};

// trasnforming the pattern into NFA

template <const auto & Fa = ctfa::block::empty, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<>) noexcept {
	return Fa;
}

// list
template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctll::list<Content...>, Rest...>) noexcept {
	constexpr auto & inner = translate_nfa(ctll::list<Content...>());
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// sequence
template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::sequence<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::concat<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// capture without capturing (TODO do the capturing)

template <const auto & Fa = ctfa::block::empty, size_t Id, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::capture<Id, Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::concat<translate_nfa(ctll::list<Content...>())>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;

	return translate_nfa<output>(ctll::list<Rest...>());
}

// capture with name
template <const auto & Fa = ctfa::block::empty, size_t Id, typename Name, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::capture_with_name<Id, Name, Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::concat<translate_nfa(ctll::list<Content...>())>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;

	return translate_nfa<output>(ctll::list<Rest...>());
}

// optional
template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::optional<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::optional<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// lazy_optional
template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::lazy_optional<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::optional<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}
	

// greedy repeat
template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::plus<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::plus<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::star<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::star<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// repeat<A,B>
template <size_t Num, typename... Content> struct head_repeat { };
template <size_t Num, typename... Content> struct tail_repeat { };

template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::head_repeat<0, Content...>, Rest...>) noexcept {
	return translate_nfa<Fa>(ctll::list<Rest...>());
}

template <const auto & Fa = ctfa::block::empty, size_t A, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::head_repeat<A, Content...>, Rest...>) noexcept {
	return translate_nfa<Fa>(ctll::list<Content..., ctre::head_repeat<A-1, Content...>, Rest...>());
}

template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::tail_repeat<0, Content...>, Rest...>) noexcept {
	return translate_nfa<Fa>(ctll::list<Rest...>());
}

template <const auto & Fa = ctfa::block::empty, size_t B, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::tail_repeat<B, Content...>, Rest...>) noexcept {
	return translate_nfa<Fa>(ctll::list<optional<Content...>, ctre::tail_repeat<B-1, Content...>, Rest...>());
}

template <const auto & Fa = ctfa::block::empty, size_t A, size_t B, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::repeat<A,B, Content...>, Rest...>) noexcept {
	if constexpr (A <= B || B == 0) {
		if constexpr (B != 0) {
			return translate_nfa<Fa>(ctll::list<head_repeat<A,Content...>, tail_repeat<B-A,Content...>, Rest...>());
		} else {
			return translate_nfa<Fa>(ctll::list<head_repeat<A,Content...>, star<Content...>, Rest...>());
		}
	} else {
		return ctfa::block::reject_all;
	}
}

// lazy repeat
template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::lazy_plus<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::plus<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::lazy_star<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::star<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// lazy_repeat<A,B>
template <const auto & Fa = ctfa::block::empty, size_t A, size_t B, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::lazy_repeat<A,B, Content...>, Rest...>) noexcept {
	return translate_nfa<Fa>(ctll::list<ctre::repeat<A,B,Content...>, Rest...>());
}

// possesive repeat
template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::possessive_plus<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::plus<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

template <const auto & Fa = ctfa::block::empty, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::possessive_star<Content...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::star<translate_nfa(ctll::list<Content>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// possessive_repeat<A,B>
template <const auto & Fa = ctfa::block::empty, size_t A, size_t B, typename... Content, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::possessive_repeat<A,B, Content...>, Rest...>) noexcept {
	return translate_nfa<Fa>(ctll::list<ctre::repeat<A,B,Content...>, Rest...>());
}

// select
template <const auto & Fa = ctfa::block::empty, typename... Options, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::select<Options...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::alternative<translate_nfa(ctll::list<Options>())...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// string
template <const auto & Fa = ctfa::block::empty, auto... Str, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::string<Str...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::block::string<char32_t(Str)...>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// one character
template <const auto & Fa = ctfa::block::empty, auto N, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::character<N>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::block::unit<char32_t(N)>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// anything
template <const auto & Fa = ctfa::block::empty, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::any, Rest...>) noexcept {
	constexpr auto & output = ctfa::remove_unneeded<Fa, ctfa::block::anything>;
	
	return translate_nfa<output>(ctll::list<Rest...>());
}

// character sets

struct nfa_set_builder {
	
	//template <typename Head> static constexpr inline auto & item(Head) noexcept {
	//	constexpr auto & current = item(Head())
	//	constexpr auto & output = ctfa::join_character_set<Fa, 
	//}

	template <auto A, auto B> static constexpr inline auto & item(ctre::char_range<A,B>) {
		return ctfa::block::range<char32_t(A), char32_t(B)>;
	}

	template <auto A> static constexpr inline auto & item(ctre::character<A>) {
		return ctfa::block::unit<char32_t(A)>;
	}

	static constexpr inline auto & item(ctre::any) {
		return ctfa::block::anything;
	}

	template <typename... Items> static constexpr inline auto & build(Items...) noexcept {
		return ctfa::join_character_set<item(Items())...>;
	}

	template <typename... Definition> static constexpr inline auto & item(ctre::set<Definition...>) {
		return build(Definition()...);
	}
	
	template <typename... Items> static constexpr inline auto & item(ctre::negative_set<Items...>) {
		return ctfa::join_negate_character_set<item(Items())...>;
	}

};



template <const auto & Fa = ctfa::block::empty, typename... Definition, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::set<Definition...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::determinize<nfa_set_builder::build(Definition()...)>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;

	return translate_nfa<output>(ctll::list<Rest...>());
}

template <const auto & Fa = ctfa::block::empty, typename... Definition, typename... Rest> 
constexpr inline auto & translate_nfa(ctll::list<ctre::negative_set<Definition...>, Rest...>) noexcept {
	constexpr auto & inner = ctfa::determinize<nfa_set_builder::build(ctre::negative_set<Definition...>())>;
	constexpr auto & output = ctfa::remove_unneeded<Fa, inner>;

	return translate_nfa<output>(ctll::list<Rest...>());
}

//template <const auto & Fa = ctfa::block::empty> 
//constexpr inline auto & translate_nfa(...) noexcept {
//	return unsupported_pattern;
//}



// wrapper for DFA

template <typename Pattern> 
constexpr inline auto & translate_dfa(Pattern) noexcept {
	const auto & result = translate_nfa<ctfa::block::empty>(ctll::list<Pattern>());
	using return_type = decltype(result);
	constexpr bool supported_pattern = !std::is_same_v<return_type, unsupported_pattern_tag>;
	static_assert(supported_pattern);
	if constexpr (supported_pattern) return ctfa::minimize<ctfa::determinize<result>>;
	else return ctfa::block::empty;
}

template <typename Pattern> 
constexpr inline auto & search_translate_dfa(Pattern) noexcept {
	const auto & result = translate_nfa<ctfa::block::empty>(ctll::list<Pattern>());
	using return_type = decltype(result);
	constexpr bool supported_pattern = !std::is_same_v<return_type, unsupported_pattern_tag>;
	static_assert(supported_pattern);
	if constexpr (supported_pattern) return ctfa::minimize<ctfa::determinize<ctfa::any_star, ctfa::minimize<ctfa::determinize<result>>, ctfa::any_star>>;
	else return ctfa::block::empty;
}


template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto fast_match_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	constexpr auto & dfa = translate_dfa(pattern);
	return ctfa::dispatcher<dfa>::run(begin, end);
}

template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto fast_search_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	constexpr auto & dfa = search_translate_dfa(pattern);
	return ctfa::dispatcher<dfa>::run(begin, end);
}
	
	
template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto fast_table_match_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	constexpr auto & dfa = translate_dfa(pattern);
	return ctfa::table_dispatcher<dfa, std::remove_const_t<std::remove_reference_t<std::remove_const_t<decltype(*begin)>>>>::run(begin, end);
}

template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto fast_table_search_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	constexpr auto & dfa = search_translate_dfa(pattern);
	return ctfa::table_dispatcher<dfa, std::remove_const_t<std::remove_reference_t<std::remove_const_t<decltype(*begin)>>>>::run(begin, end);
}
		

}

#endif
