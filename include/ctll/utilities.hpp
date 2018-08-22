#ifndef CTLL__UTILITIES__HPP
#define CTLL__UTILITIES__HPP

#include <type_traits>

namespace ctll {
	
template <bool> struct conditional_helper;
	
template <> struct conditional_helper<true> {
	template <typename A, typename> using types = A;
};

template <> struct conditional_helper<false> {
	template <typename, typename B> using types = B;
};

template <bool V, typename A, typename B> using conditional = typename conditional_helper<V>::template types<A,B>;
	
}

#endif
