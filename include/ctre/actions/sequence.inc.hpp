#ifndef CTRE__ACTIONS__SEQUENCE__HPP
#define CTRE__ACTIONS__SEQUENCE__HPP

// make_sequence
template <auto V, typename A, typename B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<B,A,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (concat)
template <auto V, typename... As, typename B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<As...>,B,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<B,As...>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (make string)
template <auto V, auto A, auto B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<character<B>,character<A>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(string<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (concat string)
template <auto V, auto... As, auto B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<string<As...>,character<B>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(string<B,As...>(), ctll::list<Ts...>()), subject.parameters};
}

#endif
