#ifndef CTLL__TYPE_STACK__HPP
#define CTLL__TYPE_STACK__HPP

#include "utilities.hpp"

namespace ctll {

template <typename... Ts> struct list { };
	
struct _nothing { };

using empty_list = list<>;

// calculate size of list content
template <typename... Ts> constexpr auto size(list<Ts...>) -> std::integral_constant<size_t, sizeof...(Ts)> { return {}; };

	
// check if the list is empty
template <typename... Ts> constexpr auto empty(list<Ts...>) -> std::false_type { return {}; };
constexpr auto empty(empty_list) -> std::true_type { return {}; };


// concat two lists together left to right
template <typename... As, typename... Bs> constexpr auto concat(list<As...>, list<Bs...>) -> list<As..., Bs...> { return {}; };


// push something to the front of a list
template <typename T, typename... As> constexpr auto push_front(T, list<As...>) -> list<T, As...> { return {}; };


// pop element from the front of a list
template <typename T, typename... As> constexpr auto pop_front(list<T, As...>) -> list<As...> { return {}; };
constexpr auto pop_front(empty_list) -> empty_list;


// return front of the list
template <typename Head, typename... As, typename T = _nothing> constexpr auto front(list<Head, As...>, T = T()) -> Head { return {}; };
template <typename T> constexpr auto front(empty_list, T) -> T { return {}; };


// find in list
template <typename T, typename... As> constexpr auto find(T, list<As...>) -> std::integral_constant<bool, (std::is_same_v<T, As> || ... || false)> { return {}; };
template <typename T> constexpr auto find(T, empty_list) -> std::false_type { return {}; };


// add to list if not exists
template <typename T, typename... As, bool Exists = (std::is_same_v<T, As> || ... || false)> constexpr auto insert_into_set(T, list<As...>) -> conditional<Exists, list<As...>, list<T, As...>> { return {}; };

}

#endif
