#ifndef CTRE__ACTIONS__BOUNDARIES__HPP
#define CTRE__ACTIONS__BOUNDARIES__HPP

// push_word_boundary
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_word_boundary, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(boundary<word_chars>(), subject.stack), subject.parameters};
}

// push_not_word_boundary
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_not_word_boundary, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(boundary<negative_set<word_chars>>(), subject.stack), subject.parameters};
}

#endif
