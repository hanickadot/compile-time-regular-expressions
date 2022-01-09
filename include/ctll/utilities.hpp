#ifndef CTLL__UTILITIES__HPP
#define CTLL__UTILITIES__HPP

#include <type_traits>

#if defined __cpp_nontype_template_parameter_class
    #define CTLL_CNTTP_COMPILER_CHECK 1
#elif defined __cpp_nontype_template_args
    #if __cpp_nontype_template_args >= 201911L
        #define CTLL_CNTTP_COMPILER_CHECK 1
    #endif
#endif

#ifndef CTLL_CNTTP_COMPILER_CHECK
    #define CTLL_CNTTP_COMPILER_CHECK 0
#endif

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
