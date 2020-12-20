#ifndef CTRE__ACTIONS__ATOMIC_GROUP__HPP
#define CTRE__ACTIONS__ATOMIC_GROUP__HPP

// atomic start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_atomic, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<atomic_start, Ts...>(), pcre_parameters<Counter>()};
}

// atomic
template <auto V, typename Atomic, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_atomic, ctll::term<V>, pcre_context<ctll::list<Atomic, atomic_start, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<atomic_group<Atomic>, Ts...>(), pcre_parameters<Counter>()};
}

// atomic sequence
template <auto V, typename... Atomic, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_atomic, ctll::term<V>, pcre_context<ctll::list<sequence<Atomic...>, atomic_start, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<atomic_group<Atomic...>, Ts...>(), pcre_parameters<Counter>()};
}

#endif
