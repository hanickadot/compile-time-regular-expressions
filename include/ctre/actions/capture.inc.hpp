#ifndef CTRE__ACTIONS__CAPTURE__HPP
#define CTRE__ACTIONS__CAPTURE__HPP

// capture
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::make_capture, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(capture<0, A>(), ctll::list<Ts...>());
}
// capture (sequence)
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::make_capture, ctll::term<V>, ctll::list<sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(capture<0, Content...>(), ctll::list<Ts...>());
}
// push_name
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::push_name, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(id<V>(), stack);
}
// push_name (concat)
template <auto... Str, auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::push_name, ctll::term<V>, ctll::list<id<Str...>, Ts...> stack) const {
	return ctll::push_front(id<Str..., V>(), ctll::list<Ts...>());
}
// capture with name
template <auto... Str, auto V, typename A, typename... Ts> constexpr auto operator()(pcre::make_capture_with_name, ctll::term<V>, ctll::list<A, id<Str...>, Ts...> stack) const {
	return ctll::push_front(capture_with_name<id<Str...>, A>(), ctll::list<Ts...>());
}
// capture with name (sequence)
template <auto... Str, auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::make_capture_with_name, ctll::term<V>, ctll::list<sequence<Content...>, id<Str...>, Ts...> stack) const {
	return ctll::push_front(capture_with_name<id<Str...>, Content...>(), ctll::list<Ts...>());
}


#endif
