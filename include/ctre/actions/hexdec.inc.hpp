#ifndef CTRE__ACTIONS__HEXDEC__HPP
#define CTRE__ACTIONS__HEXDEC__HPP

// hexdec character support (seed)
template <auto V, typename... Ts> constexpr auto operator()(pcre::create_hexdec, ctll::term<V>, ctll::list<Ts...> stack) const {
	return ctll::push_front(hexdec_number<0ull>(), stack);
}
// hexdec character support (push value)
template <auto V, uint64_t N, typename... Ts> constexpr auto operator()(pcre::push_hexdec, ctll::term<V>, ctll::list<hexdec_number<N>, Ts...> stack) const {
	constexpr auto previous = N << 4ull;	
	if constexpr (V >= 'a' && V <= 'f') {
		return ctll::push_front(hexdec_number<(previous + (V - 'a' + 10))>(), ctll::list<Ts...>());
	} else if constexpr (V >= 'A' && V <= 'F') {
		return ctll::push_front(hexdec_number<(previous + (V - 'A' + 10))>(), ctll::list<Ts...>());
	} else {
		return ctll::push_front(hexdec_number<(previous + (V - '0'))>(), ctll::list<Ts...>());
	}
}
// hexdec character support (convert to character)
template <auto V, uint64_t N, typename... Ts> constexpr auto operator()(pcre::finish_hexdec, ctll::term<V>, ctll::list<hexdec_number<N>, Ts...> stack) const {
	if constexpr (N <= std::numeric_limits<unsigned char>::max()) {
		return ctll::push_front(character<(char)N>(), ctll::list<Ts...>());
	} else {
		return ctll::push_front(character<(wchar_t)N>(), ctll::list<Ts...>());
	} 
}	

#endif
