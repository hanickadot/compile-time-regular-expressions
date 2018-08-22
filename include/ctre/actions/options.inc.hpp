#ifndef CTRE__ACTIONS__OPTIONS__HPP
#define CTRE__ACTIONS__OPTIONS__HPP

// empty option for alternate
template <auto V, typename... Ts, typename Parameters> constexpr auto operator()(pcre::push_empty, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(empty(), subject.stack), subject.parameters};
}

// make_alternate (A|B)
template <auto V, typename A, typename B, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_alternate, ctll::term<V>, pcre_context<ctll::list<B, A, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(select<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_alternate (As..)|B => (As..|B)
template <auto V, typename A, typename... Bs, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_alternate, ctll::term<V>, pcre_context<ctll::list<ctre::select<Bs...>, A, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(select<A,Bs...>(), ctll::list<Ts...>()), subject.parameters};
}


// make_optional
template <auto V, typename A, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(optional<A>(), ctll::list<Ts...>()), subject.parameters};
}

template <auto V, typename... Content, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(optional<Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (optional)
template <auto V, typename... Subject, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<optional<Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(lazy_optional<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_possessive (optional)
template <auto V, typename... Subject, typename... Ts, typename Parameters> constexpr auto operator()(pcre::make_possessive, ctll::term<V>, pcre_context<ctll::list<optional<Subject...>, Ts...>, Parameters> subject) const {
	return pcre_context{ctll::push_front(possessive_optional<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}


#endif
