#ifndef CTRE_V2__CTRE__TYPE_STACK__HPP
#define CTRE_V2__CTRE__TYPE_STACK__HPP

namespace ctre {

struct epsilon { };

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


template <typename... Ts> constexpr size_t size(list<Ts...>) {
	return sizeof...(Ts);
}

}

#endif
