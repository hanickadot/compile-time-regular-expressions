#ifndef CTRE__ACTIONS__BACKREFERENCE__HPP
#define CTRE__ACTIONS__BACKREFERENCE__HPP

// backreference with name
template <auto... Str, auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_back_reference, ctll::term<V>, pcre_context<ctll::list<id<Str...>, Ts...>, pcre_parameters<Counter>> subject) {	
	return pcre_context{ctll::push_front(back_reference_with_name<id<Str...>>(), ctll::list<Ts...>()), subject};
}

// with just a number
template <auto V, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_back_reference, ctll::term<V>, pcre_context<ctll::list<number<Id>, Ts...>, pcre_parameters<Counter>> subject) {	
	return pcre_context{ctll::push_front(back_reference<Id>(), ctll::list<Ts...>()), subject};
}

// relative backreference
template <auto V, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_relative_back_reference, ctll::term<V>, pcre_context<ctll::list<number<Id>, Ts...>, pcre_parameters<Counter>> subject) {	
	static_assert(Counter >= Id, "Relative back-reference is too big number.");
	constexpr ssize_t absolute_id = Counter - Id;
	return pcre_context{ctll::push_front(back_reference<Id>(), ctll::list<Ts...>()), subject};
}

#endif
