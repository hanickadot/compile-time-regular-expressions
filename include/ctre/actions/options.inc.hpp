#ifndef CTRE__ACTIONS__OPTIONS__HPP
#define CTRE__ACTIONS__OPTIONS__HPP

template <auto V, typename... Ts> constexpr auto operator()(pcre::push_empty, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(sequence<>(), stack);
}

#endif
