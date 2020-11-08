#ifndef CTRE__STARTS_WITH_ANCHOR__HPP
#define CTRE__STARTS_WITH_ANCHOR__HPP

#include "flags_and_modes.hpp"

namespace ctre {

template <typename... Content> 
constexpr bool starts_with_anchor(const flags &, ctll::list<Content...>) noexcept {
	return false;
}

template <typename... Content> 
constexpr bool starts_with_anchor(const flags &, ctll::list<assert_subject_begin, Content...>) noexcept {
	// yes! start subject anchor is here
	return true;
}

template <typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<assert_line_begin, Content...>) noexcept {
	// yes! start line anchor is here
	return !ctre::multiline_mode(f) || starts_with_anchor(f, ctll::list<Content...>{});
}

template <typename CharLike, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<boundary<CharLike>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Content...>{});
}

template <typename... Options, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<select<Options...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return (starts_with_anchor(f, ctll::list<Options, Content...>{}) && ... && true);
}

template <typename... Optional, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<optional<Optional...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Optional..., Content...>{}) && starts_with_anchor(f, ctll::list<Content...>{});
}

template <typename... Optional, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<lazy_optional<Optional...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Optional..., Content...>{}) && starts_with_anchor(f, ctll::list<Content...>{});
}

template <typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<sequence<Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Seq..., Content...>{});
}

template <size_t A, size_t B, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<repeat<A, B, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Seq..., Content...>{});
}

template <size_t A, size_t B, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<lazy_repeat<A, B, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Seq..., Content...>{});
}

template <size_t A, size_t B, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<possessive_repeat<A, B, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Seq..., Content...>{});
}

template <size_t Index, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<capture<Index, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Seq..., Content...>{});
}

template <size_t Index, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(const flags & f, ctll::list<capture_with_name<Index, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(f, ctll::list<Seq..., Content...>{});
}

}

#endif
