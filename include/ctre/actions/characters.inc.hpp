#ifndef CTRE__ACTIONS__CHARACTERS__HPP
#define CTRE__ACTIONS__CHARACTERS__HPP

// push character
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<V>(), stack);
}
// push_any_character
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_anything, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(any(), stack);
}
// character_alarm
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_alarm, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<'\x07'>(), stack);
}
// character_escape
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_escape, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<'\x14'>(), stack);
}
// character_formfeed
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_formfeed, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<'\x0C'>(), stack);
}
// push_character_newline
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_newline, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<'\x0A'>(), stack);
}
// push_character_null
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_null, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<'\0'>(), stack);
}
// push_character_return_carriage
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_return_carriage, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<'\x0D'>(), stack);
}
// push_character_tab
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character_tab, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(character<'\x09'>(), stack);
}

#endif
