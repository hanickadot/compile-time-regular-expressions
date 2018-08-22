#ifndef CTRE__ACTIONS__REPEAT__HPP
#define CTRE__ACTIONS__REPEAT__HPP

// repeat 1..N
template <auto V, typename A, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_plus, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(plus<A>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat 1..N (sequence)
template <auto V, typename... Content, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_plus, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(plus<Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat 0..N
template <auto V, typename A, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_star, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(star<A>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat 0..N (sequence)
template <auto V, typename... Content, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_star, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(star<Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// create_number (seed)
template <auto V, typename... Ts, typename Parameters> constexpr auto operator()(pcre::create_number, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(number<static_cast<uint64_t>(V - '0')>(), subject.stack), subject.parameters};
}
// push_number
template <auto V, uint64_t N, typename... Ts, typename Parameters> constexpr auto operator()(pcre::push_number, ctll::term<V>, pcre_context<ctll::list<number<N>, Ts...>, Parameters> subject) const {
	constexpr uint64_t previous = N * 10ull;	
	return pcre_context{ctll::push_front(number<(previous + (V - '0'))>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat A..B
template <auto V, typename Subject, uint64_t A, uint64_t B, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_ab, ctll::term<V>, pcre_context<ctll::list<number<B>, number<A>, Subject, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(repeat<A,B,Subject>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat A..B (sequence)
template <auto V, typename... Content, uint64_t A, uint64_t B, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_ab, ctll::term<V>, pcre_context<ctll::list<number<B>, number<A>, sequence<Content...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(repeat<A,B,Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat_exactly 
template <auto V, typename Subject, uint64_t A, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_exactly, ctll::term<V>, pcre_context<ctll::list<number<A>, Subject, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(repeat<A,A,Subject>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat_exactly A..B (sequence)
template <auto V, typename... Content, uint64_t A, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_exactly, ctll::term<V>, pcre_context<ctll::list<number<A>, sequence<Content...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(repeat<A,A,Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat_at_least (A+) 
template <auto V, typename Subject, uint64_t A, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_at_least, ctll::term<V>, pcre_context<ctll::list<number<A>, Subject, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(repeat<A,0,Subject>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat_at_least (A+) (sequence)
template <auto V, typename... Content, uint64_t A, typename... Ts, typename Parameters> constexpr auto operator()(pcre::repeat_at_least, ctll::term<V>, pcre_context<ctll::list<number<A>, sequence<Content...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(repeat<A,0,Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (plus)
template <auto V, typename... Subject, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<plus<Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(lazy_plus<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (star)
template <auto V, typename... Subject, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<star<Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(lazy_star<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (repeat<A,B>)
template <auto V, typename... Subject, uint64_t A, uint64_t B, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<repeat<A,B,Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(lazy_repeat<A,B,Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_possessive (plus)
template <auto V, typename... Subject, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_possessive, ctll::term<V>, pcre_context<ctll::list<plus<Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(possessive_plus<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_possessive (star)
template <auto V, typename... Subject, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_possessive, ctll::term<V>, pcre_context<ctll::list<star<Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(possessive_star<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_possessive (repeat<A,B>)
template <auto V, typename... Subject, uint64_t A, uint64_t B, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_possessive, ctll::term<V>, pcre_context<ctll::list<repeat<A,B,Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(possessive_repeat<A,B,Subject...>(), ctll::list<Ts...>()), subject.parameters};
}


#endif
