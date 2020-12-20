#ifndef CTRE__ACTIONS__OPTIONS__HPP
#define CTRE__ACTIONS__OPTIONS__HPP

// empty option for alternate
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_empty, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(empty(), subject.stack), subject.parameters};
}

// empty option for empty regex
template <typename Parameters> static constexpr auto apply(pcre::push_empty, ctll::epsilon, pcre_context<ctll::list<>, Parameters> subject) {
	return pcre_context{ctll::push_front(empty(), subject.stack), subject.parameters};
}

// make_alternate (A|B)
template <auto V, typename A, typename B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_alternate, ctll::term<V>, pcre_context<ctll::list<B, A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(select<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_alternate (As..)|B => (As..|B)
template <auto V, typename A, typename... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_alternate, ctll::term<V>, pcre_context<ctll::list<ctre::select<Bs...>, A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(select<A,Bs...>(), ctll::list<Ts...>()), subject.parameters};
}


// make_optional
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(optional<A>(), ctll::list<Ts...>()), subject.parameters};
}

template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(optional<Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// prevent from creating wrapped optionals
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<optional<A>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(optional<A>(), ctll::list<Ts...>()), subject.parameters};
}

// in case inner optional is lazy, result should be lazy too
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<lazy_optional<A>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(lazy_optional<A>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (optional)
template <auto V, typename... Subject, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<optional<Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(lazy_optional<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// if you already got a lazy optional, make_lazy is no-op
template <auto V, typename... Subject, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<lazy_optional<Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(lazy_optional<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}



#endif
