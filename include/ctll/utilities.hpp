#ifndef CTLL__UTILITIES__HPP
#define CTLL__UTILITIES__HPP

#include <type_traits>

#ifdef _MSC_VER
#define CTLL_FORCE_INLINE __forceinline
#else
#define CTLL_FORCE_INLINE __attribute__((always_inline))
#endif

namespace ctll {
	
template <bool> struct conditional_helper;
	
template <> struct conditional_helper<true> {
	template <typename A, typename> using type = A;
};

template <> struct conditional_helper<false> {
	template <typename, typename B> using type = B;
};

template <bool V, typename A, typename B> using conditional = typename conditional_helper<V>::template type<A,B>;
	
}

#endif
