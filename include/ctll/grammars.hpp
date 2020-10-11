#ifndef CTLL__GRAMMARS__HPP
#define CTLL__GRAMMARS__HPP

namespace ctll {

// terminal type representing symbol / character of any type
template <auto v> struct term {
	static constexpr auto value = v;
};

// epsilon = nothing on input tape
// also used as an command for parsing means "do nothing"
struct epsilon {
	static constexpr auto value = '-';
};

// empty_stack_symbol = nothing on stack
struct empty_stack_symbol {};

// push<T...> is alias to list<T...>
template <typename... Ts> using push = list<Ts...>;

// accept/reject type for controlling output of LL1 machine
struct accept { constexpr explicit operator bool() noexcept { return true; } };
struct reject { constexpr explicit operator bool() noexcept { return false; } };

// action type, every action item in grammar must inherit from
struct action { 
	struct action_tag { };
};

// move one character forward and pop it from stack command
struct pop_input {
	struct pop_input_tag { };
};

// additional overloads for type list
template <typename... Ts> constexpr auto push_front(pop_input, list<Ts...>) -> list<Ts...> { return {}; }

template <typename... Ts> constexpr auto push_front(epsilon, list<Ts...>) -> list<Ts...> { return {}; }

template <typename... As, typename... Bs> constexpr auto push_front(list<As...>, list<Bs...>) -> list<As..., Bs...> { return {}; }

template <typename T, typename... As> constexpr auto pop_front_and_push_front(T item, list<As...> l) {
	return push_front(item, pop_front(l));
}

// SPECIAL matching types for nicer grammars

// match any term
struct anything {
	constexpr inline anything() noexcept { }
	template <auto V> constexpr anything(term<V>) noexcept;
};

// match range of term A-B
template <auto A, decltype(A) B> struct range {
	constexpr inline range() noexcept { }
	//template <auto V> constexpr range(term<V>) noexcept requires (A <= V) && (V <= B);
	template <auto V, typename = std::enable_if_t<(A <= V) && (V <= B)>> constexpr inline range(term<V>) noexcept;
};

#ifdef __EDG__
template <auto V, auto... Set> struct contains {
	static constexpr bool value = ((Set == V) || ... || false);
};
#endif

// match terms defined in set
template <auto... Def> struct set {
	constexpr inline set() noexcept { }
	#ifdef __EDG__
	template <auto V, typename = std::enable_if_t<contains<V, Def...>::value>> constexpr inline set(term<V>) noexcept;
	#else
	template <auto V, typename = std::enable_if_t<((Def == V) || ... || false)>> constexpr inline set(term<V>) noexcept;
	#endif
};

// match terms not defined in set
template <auto... Def> struct neg_set {
	constexpr inline neg_set() noexcept { }
	
	#ifdef __EDG__
	template <auto V, typename = std::enable_if_t<!contains<V, Def...>::value>> constexpr inline neg_set(term<V>) noexcept;
	#else
	template <auto V, typename = std::enable_if_t<!((Def == V) || ... || false)>> constexpr inline neg_set(term<V>) noexcept;
	#endif
};

// AUGMENTED grammar which completes user-defined grammar for all other cases
template <typename Grammar> struct augment_grammar: public Grammar {
	// start nonterminal is defined in parent type
	using typename Grammar::_start; 
	
	// grammar rules are inherited from Grammar parent type
	using Grammar::rule; 
	
	// term on stack and on input means pop_input;
	template <auto A> static constexpr auto rule(term<A>, term<A>) -> ctll::pop_input;
	
	// if the type on stack (range, set, neg_set, anything) is constructible from the terminal => pop_input
	template <typename Expected, auto V> static constexpr auto rule(Expected, term<V>) -> std::enable_if_t<std::is_constructible_v<Expected, term<V>>, ctll::pop_input>;
	
	// empty stack and empty input means we are accepting 
	static constexpr auto rule(empty_stack_symbol, epsilon) -> ctll::accept;
	
	// not matching anything else => reject
	static constexpr auto rule(...) -> ctll::reject;
	
	// start stack is just a list<Grammar::_start>;
	using start_stack = list<typename Grammar::_start>;
};



}





#endif

