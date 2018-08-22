#ifndef CTRE__ACTIONS__OPTIONS__HPP
#define CTRE__ACTIONS__OPTIONS__HPP

// empty option for alternate
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_empty, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(empty(), stack);
}

// make_alternate (A|B)
template <auto V, typename A, typename B, typename... Ts> constexpr auto operator()(pcre::make_alternate, ctll::term<V>, ctll::list<B, A, Ts...> stack) const {
	return ctll::push_front(select<A,B>(), ctll::list<Ts...>());
}
// make_alternate (As..)|B => (As..|B)
template <auto V, typename A, typename... Bs, typename... Ts> constexpr auto operator()(pcre::make_alternate, ctll::term<V>, ctll::list<ctre::select<Bs...>, A, Ts...> stack) const {
	return ctll::push_front(select<A,Bs...>(), ctll::list<Ts...>());
}


// make_optional
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::make_optional, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(optional<A>(), ctll::list<Ts...>());
}

template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::make_optional, ctll::term<V>, ctll::list<sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(optional<Content...>(), ctll::list<Ts...>());
}

#endif
