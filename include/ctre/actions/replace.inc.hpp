#ifndef CTRE__ACTIONS__REPLACE__HPP
#define CTRE__ACTIONS__REPLACE__HPP

// push_assert_begin
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(replace_gram::replacement_char_push, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<V>(), subject.stack), subject.parameters};
}

template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(replace_gram::replacement_capture, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<V>(), subject.stack), subject.parameters};
}

template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(replace_gram::replacement_capture_push, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<V>(), subject.stack), subject.parameters};
}

#endif
