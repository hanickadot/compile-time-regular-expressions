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
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<repeat<MinB, MaxB, Content...>, repeat<MinA, MaxA, Content...>, Ts...>, Parameters> subject) {
	constexpr size_t A = MinA + MinB;
	constexpr size_t B = combine_max_repeat_length(MaxA, MaxB);
	
	return pcre_context{ctll::push_front(repeat<A, B, Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// concat lazy repeat seqeunces
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<lazy_repeat<MinB, MaxB, Content...>, lazy_repeat<MinA, MaxA, Content...>, Ts...>, Parameters> subject) {
	constexpr size_t A = MinA + MinB;
	constexpr size_t B = combine_max_repeat_length(MaxA, MaxB);
	
	return pcre_context{ctll::push_front(lazy_repeat<A, B, Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// concat possessive repeat seqeunces
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<possessive_repeat<MinB, MaxB, Content...>, possessive_repeat<MinA, MaxA, Content...>, Ts...>, Parameters> subject) {

	constexpr bool first_is_unbounded = (MaxA == 0);
	constexpr bool second_is_nonempty = (MinB > 0);
	constexpr bool second_can_be_empty = (MinB == 0);
	
	if constexpr (first_is_unbounded && second_is_nonempty) {
		// will always reject, but I keep the content, so I have some amount of captures
		return pcre_context{ctll::push_front(sequence<reject, Content...>(), ctll::list<Ts...>()), subject.parameters};
	} else if constexpr (first_is_unbounded) {
		return pcre_context{ctll::push_front(possessive_repeat<MinA, MaxA, Content...>(), ctll::list<Ts...>()), subject.parameters};
	} else if constexpr (second_can_be_empty) {
		return pcre_context{ctll::push_front(possessive_repeat<MinA, combine_max_repeat_length(MaxA, MaxB), Content...>(), ctll::list<Ts...>()), subject.parameters};
	} else {
		return pcre_context{ctll::push_front(possessive_repeat<MaxA + MinB, combine_max_repeat_length(MaxA, MaxB), Content...>(), ctll::list<Ts...>()), subject.parameters};
	}
}

#endif
