#ifndef CTRE_V2__CTRE__TYPE_STACK__HPP
#define CTRE_V2__CTRE__TYPE_STACK__HPP

namespace ctre {

struct epsilon {
	static constexpr auto value = '-';
};

struct pop_input {
	static void move_forward();
};

template <typename... Ts> struct list { };
template <typename... Ts> using push = list<Ts...>;

constexpr auto head(list<>) -> epsilon { return {}; }
template <typename T> constexpr auto head(T) -> T { return {}; }

template <typename Head, typename... Ts> constexpr auto head(list<Head, Ts...>) -> Head { return {}; }
template <typename Head, typename... Ts> constexpr auto pop(list<Head, Ts...>) -> list<Ts...> { return {}; }
//template <typename T, typename... Ts> constexpr auto push(list<Ts...>, T) -> list<T, Ts...> { return {}; }
template <typename... Ts> constexpr auto is_empty(list<Ts...>) -> std::false_type;
constexpr auto is_empty(list<>) -> std::true_type;
template <typename... As, typename... Bs> constexpr auto concat(list<As...>, list<Bs...>) -> list<As..., Bs...> { return {}; }

template <typename T, typename Head, typename... Ts> constexpr auto pop_and_push(T, list<Head, Ts...>) -> list<T, Ts...> { return {}; }

template <typename... As, typename Head, typename... Ts> constexpr auto pop_and_push(list<As...>, list<Head, Ts...>) -> list<As..., Ts...> { return {}; }

template <typename Head, typename... Ts> constexpr auto pop_and_push(epsilon, list<Head, Ts...>) -> list<Ts...> { return {}; }

template <typename Head, typename... Ts> constexpr auto pop_and_push(pop_input, list<Head, Ts...>) -> list<Ts...> { return {}; }

// support for quick-LL1 grammar

template <typename T, typename... Content> constexpr auto is_quick(T, list<T, Content...>) -> bool { return true; }

template <typename T, typename Y> constexpr auto is_quick(T, Y) -> bool { return false; }

template <typename AHead, typename... As, typename BHead, typename... Bs> constexpr auto pop_and_push_quick(list<AHead, As...>, list<BHead, Bs...>) -> list<As..., Bs...> { return {}; }

// end of quick-LL1 grammar

template <typename... Ts> constexpr size_t size(list<Ts...>) {
	return sizeof...(Ts);
}

template <typename T, typename... Ts> static inline constexpr bool set_exists = (std::is_same_v<T, Ts> || ... || false);

template <typename T, typename... Ts> constexpr auto set_add(T a, list<Ts...> b) -> std::conditional_t<set_exists<T, Ts...>, list<Ts...>, list<Ts..., T>>;

}

#endif
