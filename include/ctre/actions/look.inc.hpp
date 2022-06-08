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

// LOOKBEHIND

// lookbehind positive start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_lookbehind_positive, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<look_start<lookbehind_positive<>>, Ts...>(), pcre_parameters<Counter>()};
}

// lookbehind positive end
template <auto V, typename Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<Look, look_start<lookbehind_positive<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookbehind_positive<decltype(ctre::rotate_for_lookbehind::rotate(Look{}))>, Ts...>(), pcre_parameters<Counter>()};
}

// lookbehind positive end (sequence)
template <auto V, typename... Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<ctre::sequence<Look...>, look_start<lookbehind_positive<>>, Ts...>, pcre_parameters<Counter>>) {
	using my_lookbehind = decltype(ctre::convert_to_basic_list<lookbehind_positive>(ctll::rotate(ctll::list<decltype(ctre::rotate_for_lookbehind::rotate(Look{}))...>{})));
	return pcre_context{ctll::list<my_lookbehind, Ts...>(), pcre_parameters<Counter>()};
}

// lookbehind negative start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_lookbehind_negative, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<look_start<lookbehind_negative<>>, Ts...>(), pcre_parameters<Counter>()};
}

// lookbehind negative end
template <auto V, typename Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<Look, look_start<lookbehind_negative<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookbehind_negative<decltype(ctre::rotate_for_lookbehind::rotate(Look{}))>, Ts...>(), pcre_parameters<Counter>()};
}

// lookbehind negative end (sequence)
template <auto V, typename... Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<ctre::sequence<Look...>, look_start<lookbehind_negative<>>, Ts...>, pcre_parameters<Counter>>) {
	using my_lookbehind = decltype(ctre::convert_to_basic_list<lookbehind_negative>(ctll::rotate(ctll::list<decltype(ctre::rotate_for_lookbehind::rotate(Look{}))...>{})));
	return pcre_context{ctll::list<my_lookbehind, Ts...>(), pcre_parameters<Counter>()};
}

#endif
