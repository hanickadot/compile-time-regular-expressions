#ifndef CTRE__ACTIONS__PROPERTIES__HPP
#define CTRE__ACTIONS__PROPERTIES__HPP

// push_property_name
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_property_name, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(property_name<V>(), subject.stack), subject.parameters};
}
// push_property_name (concat)
template <auto... Str, auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_property_name, ctll::term<V>, pcre_context<ctll::list<property_name<Str...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(property_name<Str..., V>(), ctll::list<Ts...>()), subject.parameters};
}

// push_property_value
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_property_value, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(property_value<V>(), subject.stack), subject.parameters};
}
// push_property_value (concat)
template <auto... Str, auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_property_value, ctll::term<V>, pcre_context<ctll::list<property_value<Str...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(property_value<Str..., V>(), ctll::list<Ts...>()), subject.parameters};
}

// make_property
template <auto V, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property, ctll::term<V>, pcre_context<ctll::list<property_name<Name...>, Ts...>, Parameters> subject) {
	constexpr ctll::fixed_string name{Name...};
	
	constexpr unicode::property p = unicode::property_from_string(std::string_view(name));
	
	if constexpr (unicode::is_defined(p)) {
		return pcre_context{ctll::push_front(property<property_name<Name...>>(), subject.stack), subject.parameters};
	} else {
		return ctll::reject{};
	}
}

// make_property
template <auto V, auto... Value, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property, ctll::term<V>, pcre_context<ctll::list<property_value<Value...>, property_name<Name...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(property<property_name<Name...>, property_value<Value...>>(), subject.stack), subject.parameters};
}

// make_property_negative
template <auto V, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property_negative, ctll::term<V>, pcre_context<ctll::list<property_name<Name...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(negative_property<property_name<Name...>>(), subject.stack), subject.parameters};
}

// make_property_negative
template <auto V, auto... Value, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property_negative, ctll::term<V>, pcre_context<ctll::list<property_value<Value...>, property_name<Name...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(negative_property<property_name<Name...>, property_value<Value...>>(), subject.stack), subject.parameters};
}

#endif
