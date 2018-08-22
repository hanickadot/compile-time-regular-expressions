#ifndef CTLL__PARSER__HPP
#define CTLL__PARSER__HPP

#include "fixed_string.hpp"
#include "list.hpp"
#include "grammars.hpp"
#include "traits.hpp"

namespace ctll {
	
struct empty_subject { };

// get current terminal or epsilon for position in our input
#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <const auto & input, size_t pos> constexpr auto get_current_term() noexcept {
#else
template <basic_fixed_string input, size_t pos> constexpr auto get_current_term() noexcept {
#endif
	if constexpr (pos < input.size()) {
		return term<input[pos]>{};
	} else {
		return epsilon{};
	}
};

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename Grammar, const auto & input> struct parser {
#else
template <typename Grammar, basic_fixed_string input> struct parser { // in c++20
#endif
	static inline constexpr auto grammar = augment_grammar<Grammar>();
	template <typename Subject = empty_subject> static constexpr auto decide() noexcept {
		return decide<0>(grammar.start_stack, Subject());
	}
	template <size_t pos, typename Stack, typename Subject> static constexpr auto decide(Stack stack, Subject subject) noexcept {
		// look on top of the stack
		auto top_symbol = front(stack, epsilon());
		if constexpr (is_action(top_symbol)) {
			// skip for now
			return decide<pos>(pop_front(stack), subject);
		} else {
			// we need to look at the input
			auto current_term = get_current_term<input, pos>();
			// rule(...) functions doesn't have a body (because I want them to be pretty)
			auto rule = decltype(grammar.rule(top_symbol,current_term))();
			if constexpr (is_decision(rule)) {
				return bool(rule);
			} else if constexpr (is_pop_input(rule)) {
				return decide<pos+1>(pop_front(stack), subject);
			} else {
				// special cases for epsilon and list<...> are defined in grammars.hpp
				return decide<pos>(push_and_pop_front(rule, stack), subject);
				return decide<pos>(push_front(rule, pop_front(stack)), subject);
			}
		}
	}
};

} // end of ctll namespace


#endif

