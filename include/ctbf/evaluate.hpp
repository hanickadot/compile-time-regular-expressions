#ifndef CTBF__EVALUATION__HPP
#define CTBF__EVALUATION__HPP

#include "actions.hpp"
#include "utility.hpp"

namespace ctbf {

// evaluate empty list (end of computation)
template <typename Memory, typename Input, typename Output, typename... Content> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory &, Input &&, Output &&, ctll::list<>) noexcept {
	return pos;
}

// evaluate sequence
template <typename Memory, typename Input, typename Output, typename... Content> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<sequence<Content...>>) noexcept {
	return evaluate(pos, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Content...>());
}

// evaluate ptr_inc
template <typename Memory, typename Input, typename Output, typename... Ts> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<ptr_inc, Ts...>) noexcept {
	return evaluate(pos+1, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Ts...>());
}

// evaluate ptr_dec
template <typename Memory, typename Input, typename Output, typename... Ts> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<ptr_dec, Ts...>) noexcept {
	return evaluate(pos-1, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Ts...>());
}

// evaluate value_inc
template <typename Memory, typename Input, typename Output, typename... Ts> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<value_inc, Ts...>) noexcept {
	memory[pos] ++;
	return evaluate(pos, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Ts...>());
}

// evaluate value_dec
template <typename Memory, typename Input, typename Output, typename... Ts> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<value_dec, Ts...>) noexcept {
	memory[pos] --;
	return evaluate(pos, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Ts...>());
}

// char_put
template <typename Memory, typename Input, typename Output, typename... Ts> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<char_put, Ts...>) noexcept {
	output(memory[pos]);
	return evaluate(pos, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Ts...>());
}

// char_get
template <typename Memory, typename Input, typename Output, typename... Ts> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<char_get, Ts...>) noexcept {
	memory[pos] = input();
	return evaluate(pos, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Ts...>());
}

// cycle
template <typename Memory, typename Input, typename Output, typename... Content, typename... Ts> 
constexpr CTBF_FORCE_INLINE size_t evaluate(size_t pos, Memory & memory, Input && input, Output && output, ctll::list<cycle<Content...>, Ts...>) noexcept {
	while (memory[pos]) {
		pos = evaluate(pos, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Content...>());
	}
	return evaluate(pos, memory, std::forward<Input>(input), std::forward<Output>(output), ctll::list<Ts...>());
}



}

#endif
