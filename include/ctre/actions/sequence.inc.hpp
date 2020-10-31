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

static constexpr size_t combine_max_repeat_length(size_t A, size_t B) {
	if (A && B) return A+B;
	else return 0;
}

// concat repeat sequences
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<repeat<MinA, MaxA, Content...>, repeat<MinB, MaxB, Content...>, Ts...>, Parameters> subject) {
	constexpr size_t A = MinA + MinB;
	constexpr size_t B = combine_max_repeat_length(MaxA, MaxB);
	
	return pcre_context{ctll::push_front(repeat<A, B, Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// concat lazy repeat seqeunces
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<lazy_repeat<MinA, MaxA, Content...>, lazy_repeat<MinB, MaxB, Content...>, Ts...>, Parameters> subject) {
	constexpr size_t A = MinA + MinB;
	constexpr size_t B = combine_max_repeat_length(MaxA, MaxB);
	
	return pcre_context{ctll::push_front(lazy_repeat<A, B, Content...>(), ctll::list<Ts...>()), subject.parameters};
}


#endif
