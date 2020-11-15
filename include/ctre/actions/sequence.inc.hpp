#ifndef CTRE__ACTIONS__SEQUENCE__HPP
#define CTRE__ACTIONS__SEQUENCE__HPP

// make_sequence
template <auto V, typename A, typename B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<B,A,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (concat)
template <auto V, typename A, typename... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<Bs...>,A,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<A,Bs...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_sequence (make string)
template <auto V, auto A, auto B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<character<B>,character<A>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(string<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (concat string)
template <auto V, auto A, auto... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<string<Bs...>,character<A>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(string<A,Bs...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_sequence (make string in front of different items)
template <auto V, auto A, auto B, typename... Sq, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<character<B>,Sq...>,character<A>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<string<A,B>,Sq...>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (concat string in front of different items)
template <auto V, auto A, auto... Bs, typename... Sq, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<string<Bs...>,Sq...>,character<A>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<string<A,Bs...>,Sq...>(), ctll::list<Ts...>()), subject.parameters};
}


#endif
