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
	
	template <typename Subject = empty_subject> static constexpr auto decide() noexcept {
		// this will stop compiler from emmiting all visited symbols and make the function (true) constexpr!
		using output = decltype(decide<0>(grammar.start_stack, Subject()));
		return output();
	}
	template <size_t pos> static constexpr auto get_current_term() noexcept {
		if constexpr (pos < input.size()) {
			return term<input[pos]>{};
		} else {
			return epsilon{};
		}
	}
	template <size_t pos> static constexpr auto get_previous_term() noexcept {
		if constexpr (pos == 0) {
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
			auto new_subject = select_action(top_symbol, get_previous_term<pos>(), subject);
			return decide<pos>(pop_front(stack), new_subject);
		} else {
			// we need to look at the input
			auto current_term = get_current_term<pos>();
			// rule(...) functions doesn't have a body (because I want them to be pretty)
			auto rule = decltype(grammar.rule(top_symbol,current_term))();
			if constexpr (std::is_same_v<ctll::accept, decltype(rule)>) {
				return parse_result<true, decltype(subject)>();
			} else if constexpr (std::is_same_v<ctll::reject, decltype(rule)>) {
				return parse_result<false, decltype(subject)>();
			} else if constexpr (std::is_same_v<ctll::pop_input, decltype(rule)>) {
				return decide<pos+1>(pop_front(stack), subject);
			} else if constexpr (ctll::is_quick(current_term, rule)) {
				return decide<pos+1>(pop_front_and_push_front_quick(rule, stack), subject);
			} else {
				// special cases for epsilon and list<...> are defined in grammars.hpp
				return decide<pos>(pop_front_and_push_front(rule, stack), subject);
			}
		}
	}
};

} // end of ctll namespace


#endif

