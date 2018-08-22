#ifndef CTRE__ACTIONS__SEQUENCE__HPP
#define CTRE__ACTIONS__SEQUENCE__HPP

// make_sequence
template <auto V, typename A, typename B, typename... Ts> constexpr auto operator()(pcre::make_sequence, ctll::term<V>, ctll::list<B,A,Ts...> stack) const {
	return ctll::push_front(sequence<A,B>(), ctll::list<Ts...>());
}
// make_sequence (concat)
template <auto V, typename... As, typename B, typename... Ts> constexpr auto operator()(pcre::make_sequence, ctll::term<V>, ctll::list<sequence<As...>,B,Ts...> stack) const {
	return ctll::push_front(sequence<B,As...>(), ctll::list<Ts...>());
}
// make_sequence (make string)
template <auto V, auto A, auto B, typename... Ts> constexpr auto operator()(pcre::make_sequence, ctll::term<V>, ctll::list<character<B>,character<A>,Ts...> stack) const {
	return ctll::push_front(string<A,B>(), ctll::list<Ts...>());
}
// make_sequence (concat string)
template <auto V, auto... As, auto B, typename... Ts> constexpr auto operator()(pcre::make_sequence, ctll::term<V>, ctll::list<string<As...>,character<B>,Ts...> stack) const {
	return ctll::push_front(string<B,As...>(), ctll::list<Ts...>());
}

#endif
