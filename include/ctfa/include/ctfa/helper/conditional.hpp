#ifndef CTFA__HELPER__CONDITIONAL__HPP
#define CTFA__HELPER__CONDITIONAL__HPP

namespace ctfa {

template <bool> struct conditional_helper;

template <> struct conditional_helper<true> {
	template <typename T, typename> using type = T;
};

template <> struct conditional_helper<false> {
	template <typename, typename T> using type = T;
};

}

#endif
