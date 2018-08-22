#ifndef CTRE__ACTIONS__NAMED_CLASS__HPP
#define CTRE__ACTIONS__NAMED_CLASS__HPP

// class_named_alnum
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_alnum, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::alphanum_chars(), stack);
}
// class_named_alpha
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_alpha, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::alpha_chars(), stack);
}
// class_named_digit
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_digit, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::digit_chars(), stack);
}
// class_named_ascii
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_ascii, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::ascii_chars(), stack);
}
// class_named_blank
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_blank, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::enumeration<' ','\t'>(), stack);
}
// class_named_cntrl
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_cntrl, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::set<ctre::range<'\x00','\x1F'>, ctre::character<'\x7F'>>(), stack);
}
// class_named_graph
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_graph, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::range<'\x21','\x7E'>(), stack);
}
// class_named_lower
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_lower, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::range<'a','z'>(), stack);
}
// class_named_upper
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_upper, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::range<'A','Z'>(), stack);
}
// class_named_print
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_print, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(ctre::range<'\x20','\x7E'>(), stack);
}
// class_named_space
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_space, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(space_chars(), stack);
}
// class_named_word
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_word, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(word_chars(), stack);
}
// class_named_punct
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_punct, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(punct_chars(), stack);
}
// class_named_xdigit
template <auto V, typename... Ts> constexpr auto operator()(pcre::class_named_xdigit, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(xdigit_chars(), stack);
}

#endif
