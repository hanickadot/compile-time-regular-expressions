#ifndef CTLL__PARSER__HPP
#define CTLL__PARSER__HPP

#include "fixed_string.hpp"
#include "list.hpp"
#include "grammars.hpp"
#include "actions.hpp"

namespace ctll {
	
struct empty_subject { };

template <bool Result, typename Subject> struct parse_result {
	constexpr inline __attribute__((always_inline)) operator bool() const noexcept {
		return Result;
	}
	using output_type = Subject;
};

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename Grammar, const auto & input, typename ActionSelector = empty_actions, bool IngoreUnknownActions = true> struct parser {
#else
template <typename Grammar, basic_fixed_string input, typename ActionSelector = empty_actions, bool IngoreUnknownActions = true> struct parser { // in c++20
#endif
	static inline constexpr auto grammar = augment_grammar<Grammar>();
	static inline constexpr auto select_action = augment_actions<IngoreUnknownActions, ActionSelector>();
	
	template <size_t pos> static constexpr auto get_current_term() noexcept {
		if constexpr (pos < input.size()) {
			return term<input[pos]>{};
		} else {
			// return epsilon if we are past the input
			return epsilon{};
		}
	}
	template <size_t pos> static constexpr auto get_previous_term() noexcept {
		if constexpr (pos == 0) {
			// there is no previous character on input if we are on start
			return epsilon{};
		} else if constexpr ((pos-1) < input.size()) {
			return term<input[pos-1]>{};
		} else {
			return epsilon{};
		}
	}
	template <size_t pos, typename Stack, typename Subject> static constexpr auto decide(Stack stack, Subject subject) noexcept {
		// look on top of the stack
		auto top_symbol = front(stack, epsilon());
		if constexpr (std::is_base_of_v<ctll::action, decltype(top_symbol)>) {
			// skip for now
			auto new_subject = select_action.apply(top_symbol, get_previous_term<pos>(), subject);
			return decide<pos>(pop_front(stack), new_subject);
		} else {
			// we need to look at the input
			auto current_term = get_current_term<pos>();
			// rule(...) functions doesn't have a body (because I want them to be pretty)
			auto rule = decltype(grammar.rule(top_symbol,current_term))();
			if constexpr (std::is_same_v<ctll::accept, decltype(rule)>) {
				// we found accept command => we are done
				return parse_result<true, decltype(subject)>();
			} else if constexpr (std::is_same_v<ctll::reject, decltype(rule)>) {
				// we found reject command => we are done too
				return parse_result<false, decltype(subject)>();
			} else if constexpr (std::is_same_v<ctll::pop_input, decltype(rule)>) {
				// we found pop_front, move to next character and pop top of the stack
				return decide<pos+1>(pop_front(stack), subject);
			} else if constexpr (ctll::is_quick(current_term, rule)) {
				// current rule contains terminal as first symbol and same symbol is in input
				// => move forward in input and remove the symbol from stack
				return decide<pos+1>(pop_front_and_push_front_quick(rule, stack), subject);
			} else {
				// insert current rule into stack (can be list<...> or epsilon)
				// in case of epsilon we remove top symbol on stack and continue
				// in case of list<As...> we concat list with the stack to list<As...,Bs...>
				// this two `pop_front_and_push_front` functions are defined in grammars.hpp
				return decide<pos>(pop_front_and_push_front(rule, stack), subject);
			}
		}
	}
	
	template <typename Subject = empty_subject> using output = decltype(decide<0>(grammar.start_stack, Subject()));
	static inline constexpr bool correct = decide<0>(grammar.start_stack, empty_subject());
};

} // end of ctll namespace


#endif

