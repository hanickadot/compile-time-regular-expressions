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

template <size_t Sz> static constexpr std::string_view get_string_view(const std::array<char, Sz> & arr) noexcept {
	return std::string_view(arr.data(), arr.size());
}

// make_property
template <auto V, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property, ctll::term<V>, pcre_context<ctll::list<property_name<Name...>, Ts...>, Parameters> subject) {
	constexpr std::array<char, sizeof...(Name)> name{static_cast<char>(Name)...};
	
	constexpr auto p = uni::__category_from_string(get_string_view(name));
	
	if constexpr (p == uni::category::unassigned) {
		return ctll::reject{};
	} else {
		return pcre_context{ctll::push_front(binary_property<p>(), subject.stack), subject.parameters};
	}
}

// make_property
template <auto V, auto... Value, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property, ctll::term<V>, pcre_context<ctll::list<property_value<Value...>, property_name<Name...>, Ts...>, Parameters>) {
	return ctll::reject{};
	// TODO when nonbinary properties are supported
}

// make_property_negative
template <auto V, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property_negative, ctll::term<V>, pcre_context<ctll::list<property_name<Name...>, Ts...>, Parameters> subject) {
	constexpr std::array<char, sizeof...(Name)> name{static_cast<char>(Name)...};
	
	constexpr auto p = uni::__category_from_string(get_string_view(name));
	
	if constexpr (p == uni::category::unassigned) {
		return ctll::reject{};
	} else {
		return pcre_context{ctll::push_front(negate<binary_property<p>>(), subject.stack), subject.parameters};
	}
}

// make_property_negative
template <auto V, auto... Value, auto... Name, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_property_negative, ctll::term<V>, pcre_context<ctll::list<property_value<Value...>, property_name<Name...>, Ts...>, Parameters>) {
	return ctll::reject{};
	// TODO when nonbinary properties are supported
}

#endif
