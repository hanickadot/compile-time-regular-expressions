#ifndef CTRE__ACTIONS__HEXDEC__HPP
#define CTRE__ACTIONS__HEXDEC__HPP

// hexdec character support (seed)
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::create_hexdec, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(number<0ull>(), subject.stack), subject.parameters};
}
// hexdec character support (push value)
template <auto V, size_t N, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_hexdec, ctll::term<V>, pcre_context<ctll::list<number<N>, Ts...>, Parameters> subject) {
	constexpr auto previous = N << 4ull;	
	if constexpr (V >= 'a' && V <= 'f') {
		return pcre_context{ctll::push_front(number<(previous + (V - 'a' + 10))>(), ctll::list<Ts...>()), subject.parameters};
	} else if constexpr (V >= 'A' && V <= 'F') {
		return pcre_context{ctll::push_front(number<(previous + (V - 'A' + 10))>(), ctll::list<Ts...>()), subject.parameters};
	} else {
		return pcre_context{ctll::push_front(number<(previous + (V - '0'))>(), ctll::list<Ts...>()), subject.parameters};
	}
}
// hexdec character support (convert to character)
template <auto V, size_t N, typename... Ts, typename Parameters> static constexpr auto apply(pcre::finish_hexdec, ctll::term<V>, pcre_context<ctll::list<number<N>, Ts...>, Parameters> subject) {
	constexpr size_t max_char = std::numeric_limits<char>::max();
	if constexpr (N <= max_char) {
		return pcre_context{ctll::push_front(character<(char)N>(), ctll::list<Ts...>()), subject.parameters};
	} else {
		return pcre_context{ctll::push_front(character<(char32_t)N>(), ctll::list<Ts...>()), subject.parameters};
	} 
}	

#endif
