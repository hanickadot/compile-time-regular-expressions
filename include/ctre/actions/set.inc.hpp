#ifndef CTRE__ACTIONS__SET__HPP
#define CTRE__ACTIONS__SET__HPP

// UTILITY
// add into set if not exists
template <template <typename...> typename SetType, typename T, typename... As, bool Exists = (std::is_same_v<T, As> || ... || false)> static constexpr auto push_back_into_set(T, SetType<As...>) -> ctll::conditional<Exists, SetType<As...>, SetType<As...,T>> { return {}; };

//template <template <typename...> typename SetType, typename A, typename BHead, typename... Bs> struct set_merge_helper {
//	using step = decltype(push_back_into_set<SetType>(BHead(), A()));
//	using type = ctll::conditional<(sizeof...(Bs) > 0), set_merge_helper<SetType, step, Bs...>, step>;
//};
//
//// add set into set if not exists
//template <template <typename...> typename SetType, typename... As, typename... Bs> static constexpr auto push_back_into_set(SetType<As...>, SetType<Bs...>) -> typename set_merge_helper<SetType, SetType<As...>, Bs...>::type { return {}; }
//
//template <template <typename...> typename SetType, typename... As> static constexpr auto push_back_into_set(SetType<As...>, SetType<>) -> SetType<As...> { return {}; }


// END OF UTILITY

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
// TODO checkme
//// set{A...} + set{B...} = set{A...,B...}
//template <auto V, typename... As, typename... Bs, typename... Ts> constexpr auto operator()(pcre::set_combine, ctll::term<V>, ctll::list<set<As...>,set<Bs...>,Ts...> stack) const {
//	auto new_set = push_back_into_set<set>(set<As...>(), set<Bs...>());
//	return ctll::push_front(new_set, ctll::list<Ts...>());
//}

// negative_set{A...} + B = negative_set{A,B}
template <auto V, typename A, typename... Content, typename... Ts> constexpr auto operator()(pcre::set_combine, ctll::term<V>, ctll::list<A,negative_set<Content...>,Ts...> stack) const {
	auto new_set = push_back_into_set<set>(A(), set<Content...>());
	return ctll::push_front(new_set, ctll::list<Ts...>());
}
// TODO checkme
//// negative_set{A...} + negative_set{B...} = negative_set{A...,B...}
//template <auto V, typename... As, typename... Bs, typename... Ts> constexpr auto operator()(pcre::set_combine, ctll::term<V>, ctll::list<negative_set<As...>,negative_set<Bs...>,Ts...> stack) const {
//	auto new_set = push_back_into_set<negative_set>(negative_set<As...>(), negative_set<Bs...>());
//	return ctll::push_front(new_set, ctll::list<Ts...>());
//}
// negate_class_named: [[^:digit:]] = [^[:digit:]]
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::negate_class_named, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(negate<A>(), ctll::list<Ts...>());
}

// add range to set
template <auto B, auto A, typename... Ts> constexpr auto operator()(pcre::insert_range, ctll::term<B>, ctll::list<character<A>, Ts...> stack) const {
	return ctll::push_front(range<A,B>(), ctll::list<Ts...>());
}

#endif
	