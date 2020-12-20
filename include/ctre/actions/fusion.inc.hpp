#ifndef CTRE__ACTIONS__FUSION__HPP
#define CTRE__ACTIONS__FUSION__HPP

static constexpr size_t combine_max_repeat_length(size_t A, size_t B) {
	if (A && B) return A+B;
	else return 0;
}

template <size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content> static constexpr auto combine_repeat(repeat<MinA, MaxA, Content...>, repeat<MinB, MaxB, Content...>) {
	return repeat<MinA + MinB, combine_max_repeat_length(MaxA, MaxB), Content...>();
}

template <size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content> static constexpr auto combine_repeat(lazy_repeat<MinA, MaxA, Content...>, lazy_repeat<MinB, MaxB, Content...>) {
	return lazy_repeat<MinA + MinB, combine_max_repeat_length(MaxA, MaxB), Content...>();
}

template <size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content> static constexpr auto combine_repeat(possessive_repeat<MinA, MaxA, Content...>, possessive_repeat<MinB, MaxB, Content...>) {
	[[maybe_unused]] constexpr bool first_is_unbounded = (MaxA == 0);
	[[maybe_unused]] constexpr bool second_is_nonempty = (MinB > 0);
	[[maybe_unused]] constexpr bool second_can_be_empty = (MinB == 0);

	if constexpr (first_is_unbounded && second_is_nonempty) {
		// will always reject, but I keep the content, so I have some amount of captures
		return sequence<reject, Content...>();
	} else if constexpr (first_is_unbounded) {
		return possessive_repeat<MinA, MaxA, Content...>();
	} else if constexpr (second_can_be_empty) {
		return possessive_repeat<MinA, combine_max_repeat_length(MaxA, MaxB), Content...>();
	} else {
		return possessive_repeat<MaxA + MinB, combine_max_repeat_length(MaxA, MaxB), Content...>();
	}
}

// concat repeat sequences
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<repeat<MinB, MaxB, Content...>, repeat<MinA, MaxA, Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(combine_repeat(repeat<MinA, MaxA, Content...>(), repeat<MinB, MaxB, Content...>()), ctll::list<Ts...>()), subject.parameters};
}

// concat lazy repeat sequences
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<lazy_repeat<MinB, MaxB, Content...>, lazy_repeat<MinA, MaxA, Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(combine_repeat(lazy_repeat<MinA, MaxA, Content...>(), lazy_repeat<MinB, MaxB, Content...>()), ctll::list<Ts...>()), subject.parameters};
}

// concat possessive repeat seqeunces
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<possessive_repeat<MinB, MaxB, Content...>, possessive_repeat<MinA, MaxA, Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(combine_repeat(possessive_repeat<MinA, MaxA, Content...>(), possessive_repeat<MinB, MaxB, Content...>()), ctll::list<Ts...>()), subject.parameters};
}

// concat repeat sequences into sequence
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... As, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<repeat<MinB, MaxB, Content...>, As...>,repeat<MinA, MaxA, Content...>,Ts...>, Parameters> subject) {
	using result = decltype(combine_repeat(repeat<MinB, MaxB, Content...>(), repeat<MinA, MaxA, Content...>()));
	
	return pcre_context{ctll::push_front(sequence<result,As...>(), ctll::list<Ts...>()), subject.parameters};
}

// concat lazy repeat sequences into sequence
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... As, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<lazy_repeat<MinB, MaxB, Content...>, As...>,lazy_repeat<MinA, MaxA, Content...>,Ts...>, Parameters> subject) {
	using result = decltype(combine_repeat(lazy_repeat<MinB, MaxB, Content...>(), lazy_repeat<MinA, MaxA, Content...>()));
	
	return pcre_context{ctll::push_front(sequence<result,As...>(), ctll::list<Ts...>()), subject.parameters};
}

// concat possessive repeat sequences into sequence
template <auto V, size_t MinA, size_t MaxA, size_t MinB, size_t MaxB, typename... Content, typename... As, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<possessive_repeat<MinB, MaxB, Content...>, As...>,possessive_repeat<MinA, MaxA, Content...>,Ts...>, Parameters> subject) {
	using result = decltype(combine_repeat(possessive_repeat<MinB, MaxB, Content...>(), possessive_repeat<MinA, MaxA, Content...>()));
	
	return pcre_context{ctll::push_front(sequence<result,As...>(), ctll::list<Ts...>()), subject.parameters};
}

#endif
