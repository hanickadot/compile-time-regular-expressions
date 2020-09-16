#ifndef CTLL__TYPE_STACK__HPP
#define CTLL__TYPE_STACK__HPP

#ifndef CTLL__UTILITIES__HPP
#include "utilities.hpp"
#endif

namespace ctll {

template <typename... Ts> struct list { };
	
struct _nothing { };

using empty_list = list<>;

// calculate size of list content
template <typename... Ts> constexpr auto size(list<Ts...>) noexcept { return sizeof...(Ts); }

	
// check if the list is empty
template <typename... Ts> constexpr bool empty(list<Ts...>) noexcept { return false; }
constexpr bool empty(empty_list) { return true; }


// concat two lists together left to right
template <typename... As, typename... Bs> constexpr auto concat(list<As...>, list<Bs...>) noexcept -> list<As..., Bs...> { return {}; }


// push something to the front of a list
template <typename T, typename... As> constexpr auto push_front(T, list<As...>) noexcept -> list<T, As...> { return {}; }


// pop element from the front of a list
template <typename T, typename... As> constexpr auto pop_front(list<T, As...>) noexcept -> list<As...> { return {}; }
constexpr auto pop_front(empty_list) -> empty_list;

// pop element from the front of a list and return new typelist too
template <typename Front, typename List> struct list_pop_pair {
	Front front{};
	List list{};
	constexpr list_pop_pair() = default;
};

template <typename Head, typename... As, typename T = _nothing> constexpr auto pop_and_get_front(list<Head, As...>, T = T()) noexcept -> list_pop_pair<Head, list<As...>> { return {}; }
template <typename T = _nothing> constexpr auto pop_and_get_front(empty_list, T = T()) noexcept -> list_pop_pair<T, empty_list> { return {}; }


// return front of the list
template <typename Head, typename... As, typename T = _nothing> constexpr auto front(list<Head, As...>, T = T()) noexcept -> Head { return {}; }
template <typename T = _nothing> constexpr auto front(empty_list, T = T()) noexcept -> T { return {}; }


}

#endif
