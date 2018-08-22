#ifndef CTRE__ACTIONS__SET__HPP
#define CTRE__ACTIONS__SET__HPP

// add to set if not exists
template <template <typename...> typename SetType, typename T, typename... As, bool Exists = (std::is_same_v<T, As> || ... || false)> static constexpr auto push_back_into_set(T, SetType<As...>) -> ctll::conditional<Exists, SetType<As...>, SetType<As...,T>> { return {}; };


// set_start
template <auto V, typename A,typename... Ts> constexpr auto operator()(pcre::set_start, ctll::term<V>, ctll::list<A,Ts...> stack) const {
	return ctll::push_front(set<A>(), ctll::list<Ts...>());
}
// set_make
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::set_make, ctll::term<V>, ctll::list<set<Content...>, Ts...> stack) const {
	return ctll::push_front(set<Content...>(), ctll::list<Ts...>());
}
// set_make_negative
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::set_make_negative, ctll::term<V>, ctll::list<set<Content...>, Ts...> stack) const {
	return ctll::push_front(negative_set<Content...>(), ctll::list<Ts...>());
}
// set{A...} + B = set{A,B}
template <auto V, typename A, typename... Content, typename... Ts> constexpr auto operator()(pcre::set_combine, ctll::term<V>, ctll::list<A,set<Content...>,Ts...> stack) const {
	auto new_set = push_back_into_set<set>(A(), set<Content...>());
	return ctll::push_front(new_set, ctll::list<Ts...>());
}
// negative_set{A...} + B = negative_set{A,B}
template <auto V, typename A, typename... Content, typename... Ts> constexpr auto operator()(pcre::set_combine, ctll::term<V>, ctll::list<A,negative_set<Content...>,Ts...> stack) const {
	auto new_set = push_back_into_set<set>(A(), set<Content...>());
	return ctll::push_front(new_set, ctll::list<Ts...>());
}
// negate_class_named: [[^:digit:]] = [^[:digit:]]
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::negate_class_named, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(negate<A>(), ctll::list<Ts...>());
}

#endif
	