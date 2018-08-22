#ifndef CTRE__ACTIONS__ASSERTS__HPP
#define CTRE__ACTIONS__ASSERTS__HPP

// push_assert_begin
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_assert_begin, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(assert_begin(), stack);
}

// push_assert_end
template <auto V, typename... Ts> constexpr auto operator()(pcre::push_assert_end, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(assert_end(), stack);
}

#endif
	