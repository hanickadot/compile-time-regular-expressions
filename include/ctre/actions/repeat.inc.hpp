#ifndef CTRE__ACTIONS__REPEAT__HPP
#define CTRE__ACTIONS__REPEAT__HPP

// repeat 1..N
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::repeat_plus, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(plus<A>(), ctll::list<Ts...>());
}
// repeat 1..N (sequence)
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::repeat_plus, ctll::term<V>, ctll::list<sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(plus<Content...>(), ctll::list<Ts...>());
}

// repeat 0..N
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::repeat_star, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(star<A>(), ctll::list<Ts...>());
}
// repeat 0..N (sequence)
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::repeat_star, ctll::term<V>, ctll::list<sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(star<Content...>(), ctll::list<Ts...>());
}


#endif
