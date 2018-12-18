#ifndef CTRE__ACTIONS__BACKREFERENCE__HPP
#define CTRE__ACTIONS__BACKREFERENCE__HPP

// backreference with name
template <auto... Str, auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_back_reference, ctll::term<V>, pcre_context<ctll::list<id<Str...>, Ts...>, pcre_parameters<Counter>>) {	
	return pcre_context{ctll::push_front(back_reference_with_name<id<Str...>>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}

// with just a number
template <auto V, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_back_reference, ctll::term<V>, pcre_context<ctll::list<number<Id>, Ts...>, pcre_parameters<Counter>>) {
	// if we are looking outside of existing list of Ids ... reject input during parsing
	if constexpr (Counter < Id) {
		return ctll::reject{};
	} else {
		return pcre_context{ctll::push_front(back_reference<Id>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
	}
}

// relative backreference
template <auto V, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_relative_back_reference, ctll::term<V>, [[maybe_unused]] pcre_context<ctll::list<number<Id>, Ts...>, pcre_parameters<Counter>>) {	
	// if we are looking outside of existing list of Ids ... reject input during parsing
	if constexpr (Counter < Id) {
		return ctll::reject{};
	} else {
		constexpr size_t absolute_id = (Counter + 1) - Id;
		return pcre_context{ctll::push_front(back_reference<absolute_id>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
	}
}

#endif
