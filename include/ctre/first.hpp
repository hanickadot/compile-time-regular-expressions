#ifndef CTRE__FIRST__HPP
#define CTRE__FIRST__HPP

#include "atoms.hpp"
#include "atoms_characters.hpp"
#include "atoms_unicode.hpp"

namespace ctre {
	
struct can_be_anything {};
	

template <typename... Content> 
constexpr auto first(ctll::list<Content...> l, ctll::list<>) noexcept {
	return l;
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<accept, Tail...>) noexcept {
	return l;
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<end_mark, Tail...>) noexcept {
	return l;
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<end_cycle_mark, Tail...>) noexcept {
	return l;
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<end_lookahead_mark, Tail...>) noexcept {
	return l;
}

template <typename... Content, size_t Id, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return first(l, ctll::list<Tail...>{});
}

// empty
template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<empty, Tail...>) noexcept {
	return first(l, ctll::list<Tail...>{});
}

// asserts
template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<assert_begin, Tail...>) noexcept {
	return first(l, ctll::list<Tail...>{});
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<assert_end, Tail...>) noexcept {
	return l;
}

// sequence
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<sequence<Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

// plus
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<plus<Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

// lazy_plus
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<lazy_plus<Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

// possessive_plus
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<possessive_plus<Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

// star
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<star<Seq...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Tail...>{}), ctll::list<Seq..., Tail...>{});
}

// lazy_star
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<lazy_star<Seq...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Tail...>{}), ctll::list<Seq..., Tail...>{});
}

// possessive_star
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<possessive_star<Seq...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Tail...>{}), ctll::list<Seq..., Tail...>{});
}

// lazy_repeat
template <typename... Content, size_t A, size_t B, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<lazy_repeat<A, B, Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

template <typename... Content, size_t B, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<lazy_repeat<0, B, Seq...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Tail...>{}), ctll::list<Seq..., Tail...>{});
}

// possessive_repeat
template <typename... Content, size_t A, size_t B, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<possessive_repeat<A, B, Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

template <typename... Content, size_t B, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<possessive_repeat<0, B, Seq...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Tail...>{}), ctll::list<Seq..., Tail...>{});
}

// repeat
template <typename... Content, size_t A, size_t B, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<repeat<A, B, Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

template <typename... Content, size_t B, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<repeat<0, B, Seq...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Tail...>{}), ctll::list<Seq..., Tail...>{});
}

// lookahead_positive
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<lookahead_positive<Seq...>, Tail...>) noexcept {
	auto out = first(l, ctll::list<Seq..., Tail...>{});
	return first(out, ctll::list<Tail...>{});
}

// lookahead_negative TODO fixme
template <typename... Content, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<lookahead_negative<Seq...>, Tail...>) noexcept {
	return can_be_anything{};
}

// capture
template <typename... Content, size_t Id, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<capture<Id, Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

template <typename... Content, size_t Id, typename Name, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<capture_with_name<Id, Name, Seq...>, Tail...>) noexcept {
	return first(l, ctll::list<Seq..., Tail...>{});
}

// backreference
template <typename... Content, size_t Id, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<back_reference<Id, Seq...>, Tail...>) noexcept {
	return ctll::list<can_be_anything>{};
}

template <typename... Content, typename Name, typename... Seq, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<back_reference_with_name<Name, Seq...>, Tail...>) noexcept {
	return ctll::list<can_be_anything>{};
}


// string First extraction
template <typename... Content, auto First, auto... String, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<string<First, String...>, Tail...>) noexcept {
	return ctll::list<Content..., character<First>>{};
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<string<>, Tail...>) noexcept {
	return first(l, ctll::list<Tail...>{});
}

// optional
template <typename... Content, typename... Opt, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<optional<Opt...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Opt..., Tail...>{}), ctll::list<Tail...>{});
}

template <typename... Content, typename... Opt, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<lazy_optional<Opt...>, Tail...>) noexcept {
	return first(first(l, ctll::list<Opt..., Tail...>{}), ctll::list<Tail...>{});
}

// select (alternation)
template <typename... Content, typename SHead, typename... STail, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<select<SHead, STail...>, Tail...>) noexcept {
	return first(first(l, ctll::list<SHead, Tail...>{}), ctll::list<select<STail...>, Tail...>{});
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<select<>, Tail...>) noexcept {
	return l;
}


// characters / sets

template <typename... Content, auto V, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<character<V>, Tail...>) noexcept {
	return ctll::list<Content..., character<V>>{};
}

template <typename... Content, auto... Values, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<enumeration<Values...>, Tail...>) noexcept {
	return ctll::list<Content..., character<Values>...>{};
}

template <typename... Content, typename... SetContent, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<set<SetContent...>, Tail...>) noexcept {
	return ctll::list<Content..., SetContent...>{};
}

template <typename... Content, auto A, auto B, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<char_range<A,B>, Tail...>) noexcept {
	return ctll::list<Content..., char_range<A,B>>{};
}

template <typename... Content, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<any, Tail...>) noexcept {
	return ctll::list<can_be_anything>{};
}

// negative
template <typename... Content, typename... SetContent, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<negate<SetContent...>, Tail...>) noexcept {
	return ctll::list<Content..., negative_set<SetContent...>>{};
}

template <typename... Content, typename... SetContent, typename... Tail> 
constexpr auto first(ctll::list<Content...> l, ctll::list<negative_set<SetContent...>, Tail...>) noexcept {
	return ctll::list<Content..., negative_set<SetContent...>>{};
}



// calculate mutual exclusivity



}

#endif
