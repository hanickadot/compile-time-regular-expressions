#ifndef CTRE__ACTIONS__REPEAT__HPP
#define CTRE__ACTIONS__REPEAT__HPP

// repeat 1..N
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::repeat_plus, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(plus<A>(), ctll::list<Ts...>());
}
// repeat 1..N (sequence)
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::repeat_plus, ctll::term<V>, ctll::list<sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(plus<Content...>(), ctll::list<Ts...>());
}

// repeat 0..N
template <auto V, typename A, typename... Ts> constexpr auto operator()(pcre::repeat_star, ctll::term<V>, ctll::list<A, Ts...> stack) const {
	return ctll::push_front(star<A>(), ctll::list<Ts...>());
}
// repeat 0..N (sequence)
template <auto V, typename... Content, typename... Ts> constexpr auto operator()(pcre::repeat_star, ctll::term<V>, ctll::list<sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(star<Content...>(), ctll::list<Ts...>());
}

// create_number (seed)
template <auto V, typename... Ts> constexpr auto operator()(pcre::create_number, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(number<static_cast<uint64_t>(V - '0')>(), stack);
}
// push_number
template <auto V, uint64_t N, typename... Ts> constexpr auto operator()(pcre::push_number, ctll::term<V>, ctll::list<number<N>, Ts...> stack) const {
	constexpr uint64_t previous = N * 10ull;	
	return ctll::push_front(number<(previous + (V - '0'))>(), ctll::list<Ts...>());
}

// repeat A..B
template <auto V, typename Subject, uint64_t A, uint64_t B, typename... Ts> constexpr auto operator()(pcre::repeat_ab, ctll::term<V>, ctll::list<number<B>, number<A>, Subject, Ts...> stack) const {
	return ctll::push_front(repeat<A,B,Subject>(), ctll::list<Ts...>());
}
// repeat A..B (sequence)
template <auto V, typename... Content, uint64_t A, uint64_t B, typename... Ts> constexpr auto operator()(pcre::repeat_ab, ctll::term<V>, ctll::list<number<B>, number<A>, sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(repeat<A,B,Content...>(), ctll::list<Ts...>());
}

// repeat_exactly 
template <auto V, typename Subject, uint64_t A, typename... Ts> constexpr auto operator()(pcre::repeat_exactly, ctll::term<V>, ctll::list<number<A>, Subject, Ts...> stack) const {
	return ctll::push_front(repeat<A,A,Subject>(), ctll::list<Ts...>());
}
// repeat_exactly A..B (sequence)
template <auto V, typename... Content, uint64_t A, typename... Ts> constexpr auto operator()(pcre::repeat_exactly, ctll::term<V>, ctll::list<number<A>, sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(repeat<A,A,Content...>(), ctll::list<Ts...>());
}

// repeat_at_least (A+) 
template <auto V, typename Subject, uint64_t A, typename... Ts> constexpr auto operator()(pcre::repeat_at_least, ctll::term<V>, ctll::list<number<A>, Subject, Ts...> stack) const {
	return ctll::push_front(repeat<A,0,Subject>(), ctll::list<Ts...>());
}
// repeat_at_least (A+) (sequence)
template <auto V, typename... Content, uint64_t A, typename... Ts> constexpr auto operator()(pcre::repeat_at_least, ctll::term<V>, ctll::list<number<A>, sequence<Content...>, Ts...> stack) const {
	return ctll::push_front(repeat<A,0,Content...>(), ctll::list<Ts...>());
}


#endif
