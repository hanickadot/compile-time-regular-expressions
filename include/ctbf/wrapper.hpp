#ifndef CTBF__PROGRAM__HPP
#define CTBF__PROGRAM__HPP

#include <cstdio>

#include "utility.hpp"
#include "evaluate.hpp"
#include <array>

namespace ctbf {

template <typename P> struct code {
	constexpr CTBF_FORCE_INLINE code(P) noexcept { };
	template <size_t sz> CTBF_FORCE_INLINE constexpr void eval() {
		int memory[sz] = {};
		eval(memory);
	}
	template <size_t sz> CTBF_FORCE_INLINE constexpr void eval(int (&memory)[sz]) {
		evaluate(0, memory, []{
			return getchar();
		}, [](int v){
			putchar(v);
		}, ctll::list<P>());
	}
};

template <typename P> code(P) -> code<P>;

	
}

#endif
