#ifndef CTRE__ACTIONS__CLASS__HPP
#define CTRE__ACTIONS__CLASS__HPP

// class_digit
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_digit, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::digit_chars>(), subject.stack), subject.parameters};
}
// class_non_digit
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nondigit, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::digit_chars>(), subject.stack), subject.parameters};
}
// class_space
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_space, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::space_chars>(), subject.stack), subject.parameters};
}
// class_nonspace
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nonspace, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::space_chars>(), subject.stack), subject.parameters};
}

// class_horizontal_space
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_horizontal_space, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::horizontal_space_chars>(), subject.stack), subject.parameters};
}
// class_horizontal_nonspace
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_non_horizontal_space, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::horizontal_space_chars>(), subject.stack), subject.parameters};
}
// class_vertical_space
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_vertical_space, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::vertical_space_chars>(), subject.stack), subject.parameters};
}
// class_vertical_nonspace
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_non_vertical_space, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::vertical_space_chars>(), subject.stack), subject.parameters};
}

// class_word
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_word, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::word_chars>(), subject.stack), subject.parameters};
}
// class_nonword
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nonword, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::word_chars>(), subject.stack), subject.parameters};
}
// class_nonnewline
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nonnewline, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<character<'\n'>>(), subject.stack), subject.parameters};
}

#endif
