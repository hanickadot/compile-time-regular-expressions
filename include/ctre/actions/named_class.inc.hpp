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

/*
struct class_named_ascii: ctll::action {};
struct class_named_blank: ctll::action {};
struct class_named_cntrl: ctll::action {};
struct class_named_graph: ctll::action {};
struct class_named_lower: ctll::action {};
struct class_named_print: ctll::action {};
struct class_named_punct: ctll::action {};
struct class_named_space: ctll::action {};
struct class_named_upper: ctll::action {};
struct class_named_word: ctll::action {};
struct class_named_xdigit: ctll::action {};
*/

#endif
