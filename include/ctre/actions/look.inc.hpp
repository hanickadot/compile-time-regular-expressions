#ifndef CTRE__ACTIONS__LOOKAHEAD__HPP
#define CTRE__ACTIONS__LOOKAHEAD__HPP

// lookahead positive start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_lookahead_positive, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<look_start<lookahead_positive<>>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead positive end
template <auto V, typename Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<Look, look_start<lookahead_positive<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_positive<Look>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead positive end (sequence)
template <auto V, typename... Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<ctre::sequence<Look...>, look_start<lookahead_positive<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_positive<Look...>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead negative start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_lookahead_negative, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<look_start<lookahead_negative<>>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead negative end
template <auto V, typename Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<Look, look_start<lookahead_negative<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_negative<Look>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead negative end (sequence)
template <auto V, typename... Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<ctre::sequence<Look...>, look_start<lookahead_negative<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_negative<Look...>, Ts...>(), pcre_parameters<Counter>()};
}

#endif
