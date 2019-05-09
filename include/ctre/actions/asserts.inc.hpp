#ifndef CTRE__ACTIONS__ASSERTS__HPP
#define CTRE__ACTIONS__ASSERTS__HPP

// push_assert_begin
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_assert_begin, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(assert_begin(), subject.stack), subject.parameters};
}

// push_assert_end
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_assert_end, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(assert_end(), subject.stack), subject.parameters};
}

#endif
