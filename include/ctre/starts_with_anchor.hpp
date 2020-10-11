#ifndef CTRE__STARTS_WITH_ANCHOR__HPP
#define CTRE__STARTS_WITH_ANCHOR__HPP

namespace ctre {

template <typename... Content> 
constexpr bool starts_with_anchor(ctll::list<Content...>) noexcept {
	return false;
}

template <typename... Content> 
constexpr bool starts_with_anchor(ctll::list<assert_begin, Content...>) noexcept {
	// yes! start anchor is here
	return true;
}

template <typename... Options, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<select<Options...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return (starts_with_anchor(ctll::list<Options, Content...>{}) && ... && true);
}

template <typename... Optional, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<optional<Optional...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Optional..., Content...>{}) && starts_with_anchor(ctll::list<Content...>{});
}

template <typename... Optional, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<lazy_optional<Optional...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Optional..., Content...>{}) && starts_with_anchor(ctll::list<Content...>{});
}

template <typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<sequence<Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Seq..., Content...>{});
}

template <size_t A, size_t B, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<repeat<A, B, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Seq..., Content...>{});
}

template <size_t A, size_t B, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<lazy_repeat<A, B, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Seq..., Content...>{});
}

template <size_t A, size_t B, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<possessive_repeat<A, B, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Seq..., Content...>{});
}

template <size_t Index, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<capture<Index, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Seq..., Content...>{});
}

template <size_t Index, typename... Seq, typename... Content> 
constexpr bool starts_with_anchor(ctll::list<capture_with_name<Index, Seq...>, Content...>) noexcept {
	// check if all options starts with anchor or if they are empty, there is an anchor behind them
	return starts_with_anchor(ctll::list<Seq..., Content...>{});
}

}

#endif
