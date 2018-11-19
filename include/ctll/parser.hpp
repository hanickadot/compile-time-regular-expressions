#ifndef CTLL__PARSER__HPP
#define CTLL__PARSER__HPP

#include "fixed_string.hpp"
#include "list.hpp"
#include "grammars.hpp"
#include "actions.hpp"

namespace ctll {


enum class decision {
	reject,
	accept,
	undecided
};

template <typename T> void id(T);

struct placeholder { };

template <bool Correct, typename Subject> struct result_of_parsing {
	constexpr operator bool() const noexcept {
		return Correct;
	}
	using output_type = Subject;
};

#if !__cpp_nontype_template_parameter_class
template <typename Grammar, const auto & input, typename ActionSelector = empty_actions, bool IgnoreUnknownActions = false> struct parser {
#else
template <typename Grammar, ctll::basic_fixed_string input, typename ActionSelector = empty_actions, bool IgnoreUnknownActions = false> struct parser { // in c++20
#endif
	using Actions = ctll::conditional<IgnoreUnknownActions, ignore_unknown<ActionSelector>, identity<ActionSelector>>;
	using grammar = augment_grammar<Grammar>;
	
	
	template <size_t Pos> static constexpr auto get_current_term() noexcept {
		if constexpr (Pos < input.size()) {
			return term<input[Pos]>{};
		} else {
			// return epsilon if we are past the input
			return epsilon{};
		}
	}
	template <size_t Pos> static constexpr auto get_previous_term() noexcept {
		if constexpr (Pos == 0) {
			// there is no previous character on input if we are on start
			return epsilon{};
		} else if constexpr ((Pos-1) < input.size()) {
			return term<input[Pos-1]>{};
		} else {
			return epsilon{};
		}
	}
	// if rule is accept => return true and subject
	template <size_t Pos, typename Terminal, typename Stack, typename Subject> 
	static constexpr auto move(ctll::accept, Terminal, Stack, Subject) noexcept {
		return result_of_parsing<true, Subject>{};
	}
	// if rule is reject => return false and subject
	template <size_t Pos, typename Terminal, typename Stack, typename Subject>
	static constexpr auto move(ctll::reject, Terminal, Stack, Subject) noexcept {
		return result_of_parsing<false, Subject>{};
	}
	// if rule is pop_input => move to next character
	template <size_t Pos, typename Terminal, typename Stack, typename Subject>
	static constexpr auto move(ctll::pop_input, Terminal, Stack stack, Subject subject) noexcept {
		return decide<Pos+1>(stack, subject);
	}
	// if rule is string => push it to the front of stack
	template <size_t Pos, typename... Content, typename Terminal, typename Stack, typename Subject>
	static constexpr auto move(ctll::list<Content...> string, Terminal, Stack stack, Subject subject) noexcept {
		return decide<Pos>(push_front(string, stack), subject);
	}
	// if rule is epsilon (empty string) => continue
	template <size_t Pos, typename Terminal, typename Stack, typename Subject>
	static constexpr auto move(epsilon, Terminal, Stack stack, Subject subject) noexcept {
		return decide<Pos>(stack, subject);
	}
	// if rule is string with current character at the beginning (term<V>) => move to next character 
	// and push string without the character (quick LL(1))
	template <size_t Pos, auto V, typename... Content, typename Stack, typename Subject>
	static constexpr auto move(ctll::list<term<V>, Content...>, term<V>, Stack stack, Subject subject) noexcept {
		return decide<Pos+1>(push_front(list<Content...>(), stack), subject);
		//return seed<Pos+1, decltype(push_front(list<Content...>(), stack)), Subject, decision::undecided>();
	}
	// if rule is string with any character at the beginning (compatible with current term<T>) => move to next character 
	// and push string without the character (quick LL(1))
	template <size_t Pos, auto V, typename... Content, auto T, typename Stack, typename Subject>
	static constexpr auto move(ctll::list<anything, Content...>, term<T>, Stack stack, Subject subject) noexcept {
		return decide<Pos+1>(push_front(list<Content...>(), stack), subject);
		
		//return seed<Pos+1, decltype(push_front(list<Content...>(), stack)), Subject, decision::undecided>();
	}
	// decide if we need to take action or move
	template <size_t Pos, typename Stack, typename Subject> static constexpr auto decide(Stack previous_stack, Subject previous_subject) noexcept {
		// each call means we pop something from stack
		auto top_symbol = decltype(ctll::front(previous_stack, empty_stack_symbol()))();
		// gcc pedantic warning
		[[maybe_unused]] auto stack = decltype(ctll::pop_front(previous_stack))();
		
		// in case top_symbol is action type (apply it on previous subject and get new one)
		if constexpr (std::is_base_of_v<ctll::action, decltype(top_symbol)>) {
			auto subject = Actions::apply(top_symbol, get_previous_term<Pos>(), previous_subject);
			
			// in case that semantic action is error => reject input
			if constexpr (std::is_same_v<ctll::reject, decltype(subject)>) {
				return result_of_parsing<false, Subject>();
			} else {
				return decide<Pos>(stack, subject);
			}
		} else {
			// all other cases are ordinary for LL(1) parser
			auto current_term = get_current_term<Pos>();
			auto rule = decltype(grammar::rule(top_symbol,current_term))();
			return move<Pos>(rule, current_term, stack, previous_subject);
		}
	}
	
	template <size_t> using index_placeholder = placeholder;
	
#if false
	// trampolines with folded expression
	template <typename Subject, size_t... Pos> static constexpr auto trampoline_decide(Subject, std::index_sequence<Pos...>) noexcept {
		// parse everything for first char and than for next and next ...
		// Pos+1 is needed as we want to finish calculation with epsilons on stack
		auto v = (seed<0, grammar::start_stack, Subject, decision::undecided>::parse() + ... + index_placeholder<Pos+1>());
		//id(v);
		return v;
	}
	
	template <typename Subject = empty_subject> static constexpr auto trampoline_decide(Subject subject = {}) noexcept {
		// there will be no recursion, just sequence long as the input
		return trampoline_decide(subject, std::make_index_sequence<input.size()>());
	}
#endif
	
	template <typename Subject = empty_subject> using output = decltype(decide<0, typename grammar::start_stack, Subject>({}, {}));
	static inline constexpr bool correct = decide<0, typename grammar::start_stack, empty_subject>({}, {});
	template <typename Subject = empty_subject> static inline constexpr bool correct_with = decide<0, typename grammar::start_stack, Subject>({}, {});
};

} // end of ctll namespace


#endif

