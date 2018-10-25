#ifndef CTRE__ACTIONS__CAPTURE__HPP
#define CTRE__ACTIONS__CAPTURE__HPP

// capture
template <auto V, typename A, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture<Counter+1, A>(), ctll::list<Ts...>()), pcre_parameters<Counter+1>()};
}
// capture (sequence)
template <auto V, typename... Content, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture<Counter+1, Content...>(), ctll::list<Ts...>()), pcre_parameters<Counter+1>()};
}
// push_name
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_name, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(id<V>(), subject.stack), subject.parameters};
}
// push_name (concat)
template <auto... Str, auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_name, ctll::term<V>, pcre_context<ctll::list<id<Str...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(id<Str..., V>(), ctll::list<Ts...>()), subject.parameters};
}
// capture with name
template <auto... Str, auto V, typename A, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture_with_name, ctll::term<V>, pcre_context<ctll::list<A, id<Str...>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_with_name<Counter+1, id<Str...>, A>(), ctll::list<Ts...>()), pcre_parameters<Counter+1>()};
}
// capture with name (sequence)
template <auto... Str, auto V, typename... Content, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture_with_name, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, id<Str...>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_with_name<Counter+1, id<Str...>, Content...>(), ctll::list<Ts...>()), pcre_parameters<Counter+1>()};
}


#endif
