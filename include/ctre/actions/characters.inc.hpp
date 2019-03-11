#ifndef CTRE__ACTIONS__CHARACTERS__HPP
#define CTRE__ACTIONS__CHARACTERS__HPP

// push character
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<V>(), subject.stack), subject.parameters};
}
// push_any_character
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_anything, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(any(), subject.stack), subject.parameters};
}
// character_alarm
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_alarm, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x07'>(), subject.stack), subject.parameters};
}
// character_escape
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_escape, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x14'>(), subject.stack), subject.parameters};
}
// character_formfeed
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_formfeed, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x0C'>(), subject.stack), subject.parameters};
}
// push_character_newline
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_newline, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x0A'>(), subject.stack), subject.parameters};
}
// push_character_null
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_null, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\0'>(), subject.stack), subject.parameters};
}
// push_character_return_carriage
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_return_carriage, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x0D'>(), subject.stack), subject.parameters};
}
// push_character_tab
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_tab, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x09'>(), subject.stack), subject.parameters};
}

#endif
