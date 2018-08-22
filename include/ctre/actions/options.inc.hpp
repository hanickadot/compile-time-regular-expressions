#ifndef CTRE__ACTIONS__OPTIONS__HPP
#define CTRE__ACTIONS__OPTIONS__HPP

template <auto V, typename... Ts> constexpr auto operator()(pcre::push_empty, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(sequence<>(), stack);
}

// make_alternate (A|B)
template <auto V, typename A, typename B, typename... Ts> constexpr auto operator()(pcre::make_alternate, ctll::term<V>, ctll::list<B, A, Ts...> stack) const {
	return ctll::push_front(select<A,B>(), ctll::list<Ts...>());
}
// make_alternate (As..)|B => (As..|B)
template <auto V, typename A, typename... Bs, typename... Ts> constexpr auto operator()(pcre::make_alternate, ctll::term<V>, ctll::list<ctre::select<Bs...>, A, Ts...> stack) const {
	return ctll::push_front(select<A,Bs...>(), ctll::list<Ts...>());
}

#endif
