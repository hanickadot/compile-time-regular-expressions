#ifndef CTRE__TRANSLATE_XPRESSIVE__HPP
#define CTRE__TRANSLATE_XPRESSIVE__HPP

#if __has_include(<boost/xpressive/xpressive.hpp>)
#define CTRE_HAS_XPRESSIVE_AVAILABLE 1
#include <boost/xpressive/xpressive.hpp>

namespace ctre {

namespace xpressive {

// trasnforming the pattern into xpressive static regex

template <typename Iterator> static auto empty_object = boost::xpressive::basic_regex<Iterator>();

template <typename Iterator> constexpr inline auto & xpressive_translate(ctll::list<>) noexcept {
	return empty_object<Iterator>;
}

// list
//template <typename Iterator, typename... Content, typename... Rest> 
//constexpr inline auto xpressive_translate(ctll::list<ctll::list<Content...>, Rest...>) noexcept {
//	return boost::xpressive::basic_regex<Iterator>((xpressive_translate<Iterator>(ctll::list<Rest>()) >> ...));
//}

// sequence
template <typename Iterator, typename... Content, typename... Rest> 
constexpr inline auto xpressive_translate(ctll::list<ctre::sequence<Content...>, Rest...>) noexcept {
	return boost::xpressive::basic_regex<Iterator>((xpressive_translate<Iterator>(ctll::list<Rest>()) >> ...));
}

// character
template <typename Iterator, auto N, typename... Rest> static auto character_object = boost::xpressive::basic_regex<Iterator>(boost::xpressive::as_xpr(N) >> xpressive_translate<Iterator>(ctll::list<Rest...>()));

template <typename Iterator, auto N, typename... Rest> 
constexpr inline auto & xpressive_translate(ctll::list<ctre::character<N>, Rest...>) noexcept {
	return character_object<Iterator, N, Rest...>;
}

// any
template <typename Iterator, typename... Rest> 
constexpr inline auto xpressive_translate(ctll::list<ctre::any, Rest...>) noexcept {
	return boost::xpressive::basic_regex<Iterator>(boost::xpressive::_ >> xpressive_translate<Iterator>(ctll::list<Rest...>()));
}

// string
template <typename Iterator, auto... Str, typename... Rest> 
constexpr inline auto xpressive_translate(ctll::list<ctre::string<Str...>, Rest...>) noexcept {
	return (boost::xpressive::basic_regex<Iterator>(boost::xpressive::as_xpr(Str))  >> ...) >> xpressive_translate<Iterator>(ctll::list<Rest...>());
}



template <typename Iterator, typename Pattern> static auto whole_pattern = xpressive_translate<Iterator>(ctll::list<Pattern>()); 


} // xpressive namespace 

template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto xpressive_match_re(const Iterator begin, const EndIterator end, Pattern) noexcept {
	
	if constexpr (std::is_same_v<EndIterator, zero_terminated_string_end_iterator>) {
		return boost::xpressive::regex_match(begin, xpressive::whole_pattern<Iterator, Pattern>);
	} else {
		return boost::xpressive::regex_match(begin, end, xpressive::whole_pattern<Iterator, Pattern>);
	}
	return false;
}

template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto xpressive_search_re(const Iterator begin, const EndIterator end, Pattern) noexcept {
	//identify<decltype(xpressive_pattern<Iterator, Pattern>)> i;
	if constexpr (std::is_same_v<EndIterator, zero_terminated_string_end_iterator>) {
		return boost::xpressive::regex_search(begin, xpressive::whole_pattern<Iterator, Pattern>);
	} else {
		return boost::xpressive::regex_search(begin, end, xpressive::whole_pattern<Iterator, Pattern>);
	}
	return false;
}
	
}

#endif // boost::xpressive include test
#endif
