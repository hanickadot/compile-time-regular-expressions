#ifndef CTRE__ACTIONS__CLASS__HPP
#define CTRE__ACTIONS__CLASS__HPP

// class_digit
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_digit, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::set<ctre::range<'0','9'>>(), stack);
}
// class_non_digit
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_nondigit, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::negative_set<ctre::range<'0','9'>>(), stack);
}
// class_space
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_space, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::set<ctre::space_chars>(), stack);
}
// class_nonspace
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_nonspace, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::negative_set<ctre::space_chars>(), stack);
}
// class_word
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_word, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::set<ctre::word_chars>(), stack);
}
// class_nonword
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_nonword, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::negative_set<ctre::word_chars>(), stack);
}
// class_nonnewline
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_nonnewline, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::negative_set<character<'\n'>>(), stack);
}

#endif
