#ifndef CTRE__ACTIONS__SET__HPP
#define CTRE__ACTIONS__SET__HPP

// UTILITY
// add into set if not exists
template <template <typename...> typename SetType, typename T, typename... As, bool Exists = (std::is_same_v<T, As> || ... || false)> static constexpr auto push_back_into_set(T, SetType<As...>) -> ctll::conditional<Exists, SetType<As...>, SetType<As...,T>> { return {}; }

//template <template <typename...> typename SetType, typename A, typename BHead, typename... Bs> struct set_merge_helper {
//	using step = decltype(push_back_into_set<SetType>(BHead(), A()));
//	using type = ctll::conditional<(sizeof...(Bs) > 0), set_merge_helper<SetType, step, Bs...>, step>;
//};
//
//// add set into set if not exists
//template <template <typename...> typename SetType, typename... As, typename... Bs> static constexpr auto push_back_into_set(SetType<As...>, SetType<Bs...>) -> typename set_merge_helper<SetType, SetType<As...>, Bs...>::type { return pcre_context{{};), subject.parameters}}
//
//template <template <typename...> typename SetType, typename... As> static constexpr auto push_back_into_set(SetType<As...>, SetType<>) -> SetType<As...> { return pcre_context{{};), subject.parameters}}


// END OF UTILITY

// set_start
template <auto V, typename A,typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_start, ctll::term<V>, pcre_context<ctll::list<A,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(set<A>(), ctll::list<Ts...>()), subject.parameters};
}
// set_make
template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_make, ctll::term<V>, pcre_context<ctll::list<set<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(set<Content...>(), ctll::list<Ts...>()), subject.parameters};
}
// set_make_negative
template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_make_negative, ctll::term<V>, pcre_context<ctll::list<set<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(negative_set<Content...>(), ctll::list<Ts...>()), subject.parameters};
}
// set{A...} + B = set{A,B}
template <auto V, typename A, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<A,set<Content...>,Ts...>, Parameters> subject) {
	auto new_set = push_back_into_set<set>(A(), set<Content...>());
	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
}
// TODO checkme
//// set{A...} + set{B...} = set{A...,B...}
//template <auto V, typename... As, typename... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<set<As...>,set<Bs...>,Ts...>, Parameters> subject) {
//	auto new_set = push_back_into_set<set>(set<As...>(), set<Bs...>());
//	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
//}

// negative_set{A...} + B = negative_set{A,B}
template <auto V, typename A, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<A,negative_set<Content...>,Ts...>, Parameters> subject) {
	auto new_set = push_back_into_set<set>(A(), set<Content...>());
	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
}
// TODO checkme
//// negative_set{A...} + negative_set{B...} = negative_set{A...,B...}
//template <auto V, typename... As, typename... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<negative_set<As...>,negative_set<Bs...>,Ts...>, Parameters> subject) {
//	auto new_set = push_back_into_set<negative_set>(negative_set<As...>(), negative_set<Bs...>());
//	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
//}
// negate_class_named: [[^:digit:]] = [^[:digit:]]
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::negate_class_named, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(negate<A>(), ctll::list<Ts...>()), subject.parameters};
}

// add range to set
template <auto V, auto B, auto A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_range, ctll::term<V>, pcre_context<ctll::list<character<B>,character<A>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(char_range<A,B>(), ctll::list<Ts...>()), subject.parameters};
}

#endif
