#ifndef CTRE__ACTIONS__MODE__HPP
#define CTRE__ACTIONS__MODE__HPP

// we need to reset counter and wrap Mode into mode_switch
template <typename Mode, typename... Ts, typename Parameters> static constexpr auto apply_mode(Mode, ctll::list<Ts...>, Parameters) {
	return pcre_context<ctll::list<mode_switch<Mode>, Ts...>, Parameters>{};
}

template <typename Mode, typename... Ts, size_t Id, size_t Counter> static constexpr auto apply_mode(Mode, ctll::list<capture_id<Id>, Ts...>, pcre_parameters<Counter>) {
	return pcre_context<ctll::list<mode_switch<Mode>, Ts...>, pcre_parameters<Counter-1>>{};
}

// catch a semantic action into mode
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::mode_case_insensitive mode, ctll::term<V>,pcre_context<ctll::list<Ts...>, Parameters>) {
	return apply_mode(mode, ctll::list<Ts...>{}, Parameters{});
}

template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::mode_case_sensitive mode, ctll::term<V>,pcre_context<ctll::list<Ts...>, Parameters>) {
	return apply_mode(mode, ctll::list<Ts...>{}, Parameters{});
}

template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::mode_singleline mode, ctll::term<V>,pcre_context<ctll::list<Ts...>, Parameters>) {
	return apply_mode(mode, ctll::list<Ts...>{}, Parameters{});
}

template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::mode_multiline mode, ctll::term<V>,pcre_context<ctll::list<Ts...>, Parameters>) {
	return apply_mode(mode, ctll::list<Ts...>{}, Parameters{});
}

// to properly reset capture

#endif
