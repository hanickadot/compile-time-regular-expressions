/*

The MIT License (MIT)

Copyright (c) 2014-2019 Hana Dusíková (hanicka@hanicka.net)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef CTRE_V2__CTRE__HPP
#define CTRE_V2__CTRE__HPP

#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#ifndef CTRE_V2__CTLL__HPP
#define CTRE_V2__CTLL__HPP

#ifndef CTLL__PARSER__HPP
#define CTLL__PARSER__HPP

#ifndef CTLL__FIXED_STRING__GPP
#define CTLL__FIXED_STRING__GPP

#include <utility>
#include <cstddef>

namespace ctll {

template <typename CharT, size_t N> class basic_fixed_string {
	CharT content[N];
public: 
	using char_type = CharT;
	
	template <size_t... I> constexpr basic_fixed_string(const CharT (&input)[N], std::index_sequence<I...>) noexcept: content{input[I]...} { }
	
	constexpr basic_fixed_string(const CharT (&input)[N]) noexcept: basic_fixed_string(input, std::make_index_sequence<N>()) { }
	
	constexpr size_t size() const noexcept {
		// if it's zero terminated string (from const char * literal) then size N - 1
		if (content[N-1] == '\0') return N - 1;
		else return N;
	}
	constexpr CharT operator[](size_t i) const noexcept {
		return content[i];
	}
	constexpr const CharT * begin() const noexcept {
		return content;
	}
	constexpr const CharT * end() const noexcept {
		return content + size();
	}
#if __has_include(<compare>)
//	constexpr auto operator<=>(const basic_fixed_string &, const basic_fixed_string &) = default;
#endif
};

template <typename CharT> class basic_fixed_string<CharT, 0> {
public: 
	using char_type = CharT;
	constexpr basic_fixed_string(const CharT *) noexcept { }
	constexpr size_t size() const noexcept {
		return 0;
	}
	constexpr const CharT * begin() const noexcept {
		return nullptr;
	}
	constexpr const CharT * end() const noexcept {
		return nullptr;
	}
#if __has_include(<compare>)
//	constexpr auto operator<=>(const basic_fixed_string &, const basic_fixed_string &) = default;
#endif
};

template <typename CharT, size_t N> basic_fixed_string(const CharT (&)[N]) -> basic_fixed_string<CharT, N>;
template <typename CharT, size_t N> basic_fixed_string(basic_fixed_string<CharT, N>) -> basic_fixed_string<CharT, N>;

}

#endif

#ifndef CTLL__TYPE_STACK__HPP
#define CTLL__TYPE_STACK__HPP

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

namespace ctll {

template <typename... Ts> struct list { };
	
struct _nothing { };

using empty_list = list<>;

// calculate size of list content
template <typename... Ts> constexpr auto size(list<Ts...>) noexcept { return sizeof...(Ts); }

	
// check if the list is empty
template <typename... Ts> constexpr bool empty(list<Ts...>) noexcept { return false; }
constexpr bool empty(empty_list) { return true; }

// concat two lists together left to right
template <typename... As, typename... Bs> constexpr auto concat(list<As...>, list<Bs...>) noexcept -> list<As..., Bs...> { return {}; }

// push something to the front of a list
template <typename T, typename... As> constexpr auto push_front(T, list<As...>) noexcept -> list<T, As...> { return {}; }

// pop element from the front of a list
template <typename T, typename... As> constexpr auto pop_front(list<T, As...>) noexcept -> list<As...> { return {}; }
constexpr auto pop_front(empty_list) -> empty_list;

// pop element from the front of a list and return new typelist too
template <typename Front, typename List> struct list_pop_pair {
	Front front{};
	List list{};
	constexpr list_pop_pair() = default;
};

template <typename Head, typename... As, typename T = _nothing> constexpr auto pop_and_get_front(list<Head, As...>, T = T()) noexcept -> list_pop_pair<Head, list<As...>> { return {}; }
template <typename T = _nothing> constexpr auto pop_and_get_front(empty_list, T = T()) noexcept -> list_pop_pair<T, empty_list> { return {}; }

// return front of the list
template <typename Head, typename... As, typename T = _nothing> constexpr auto front(list<Head, As...>, T = T()) noexcept -> Head { return {}; }
template <typename T = _nothing> constexpr auto front(empty_list, T = T()) noexcept -> T { return {}; }

}

#endif

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
	constexpr inline anything() noexcept { };
	template <auto V> constexpr anything(term<V>) noexcept;
};

// match range of term A-B
template <auto A, decltype(A) B> struct range {
	constexpr inline range() noexcept { };
	//template <auto V> constexpr range(term<V>) noexcept requires (A <= V) && (V <= B);
	template <auto V, typename = std::enable_if_t<(A <= V) && (V <= B)>> constexpr inline range(term<V>) noexcept;
};

// match terms defined in set
template <auto... Def> struct set {
	constexpr inline set() noexcept { };
	//template <auto V> constexpr set(term<V>) noexcept requires ((Def == V) || ... || false);
	template <auto V, typename = std::enable_if_t<((Def == V) || ... || false)>> constexpr inline set(term<V>) noexcept;
};

// match terms not defined in set
template <auto... Def> struct neg_set {
	constexpr inline neg_set() noexcept { };
	//template <auto V> constexpr set(term<V>) noexcept requires ((Def == V) || ... || false);
	template <auto V, typename = std::enable_if_t<((Def != V) && ... && true)>> constexpr inline neg_set(term<V>) noexcept;
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

#ifndef CTLL__ACTIONS__HPP
#define CTLL__ACTIONS__HPP

namespace ctll {
	struct empty_subject { };
	
	struct empty_actions {
		// dummy operator so using Actions::operator() later will not give error
		template <typename Action, typename InputSymbol, typename Subject> static constexpr auto apply(Action, InputSymbol, Subject subject) {
			return subject;
		}
	};
	
	template <typename Actions> struct identity: public Actions {
		using Actions::apply;
		// allow empty_subject to exists
		template <typename Action, auto V> constexpr static auto apply(Action, term<V>, empty_subject) -> empty_subject { return {}; }
		template <typename Action> constexpr static auto apply(Action, epsilon, empty_subject) -> empty_subject { return {}; }	
	};
	
	template <typename Actions> struct ignore_unknown: public Actions {
		using Actions::apply;
		// allow flow thru unknown actions
		template <typename Action, auto V, typename Subject> constexpr static auto apply(Action, term<V>, Subject) -> Subject { return {}; }
		template <typename Action, typename Subject> constexpr static auto apply(Action, epsilon, Subject) -> Subject { return {}; }	
	};
}

#endif

#include <limits>

namespace ctll {

enum class decision {
	reject,
	accept,
	undecided
};

struct placeholder { };

template <size_t> using index_placeholder = placeholder;

#if !__cpp_nontype_template_parameter_class
template <typename Grammar, const auto & input, typename ActionSelector = empty_actions, bool IgnoreUnknownActions = false> struct parser {
#else
template <typename Grammar, ctll::basic_fixed_string input, typename ActionSelector = empty_actions, bool IgnoreUnknownActions = false> struct parser { // in c++20
#endif
	
	#if __cpp_nontype_template_parameter_class
	static constexpr auto _input = input; // workaround to GCC bug
	#else
	static constexpr auto & _input = input; // c++17 mode
	#endif

	using Actions = ctll::conditional<IgnoreUnknownActions, ignore_unknown<ActionSelector>, identity<ActionSelector>>;
	using grammar = augment_grammar<Grammar>;
	
	template <size_t Pos, typename Stack, typename Subject, decision Decision> struct results {
		constexpr inline CTLL_FORCE_INLINE operator bool() const noexcept {
			return Decision == decision::accept;
		}
		
		#if __cpp_nontype_template_parameter_class
		static constexpr auto _input = input; // workaround to GCC bug
		#else
		static constexpr auto & _input = input; // c++17 mode
		#endif
	
		using output_type = Subject;
    
		constexpr auto operator+(placeholder) const noexcept {
			if constexpr (Decision == decision::undecided) {
				// parse for current char (RPos) with previous stack and subject :)
				return parser<Grammar, _input, ActionSelector, IgnoreUnknownActions>::template decide<Pos, Stack, Subject>({}, {});
			} else {
				// if there is decision already => just push it to the end of fold expression
				return *this;
			}
		}
	};
	
	template <size_t Pos> static constexpr auto get_current_term() noexcept {
		if constexpr (Pos < input.size()) {
			constexpr auto value = input[Pos];
			if constexpr (value <= std::numeric_limits<char>::max()) {
				return term<static_cast<char>(value)>{};
			} else {
				return term<input[Pos]>{};
			}
			
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
			constexpr auto value = input[Pos-1];
			if constexpr (value <= std::numeric_limits<char>::max()) {
				return term<static_cast<char>(value)>{};
			} else {
				return term<input[Pos]>{};
			}
		} else {
			return epsilon{};
		}
	}
	// if rule is accept => return true and subject
	template <size_t Pos, typename Terminal, typename Stack, typename Subject> 
	static constexpr auto move(ctll::accept, Terminal, Stack, Subject) noexcept {
		return typename parser<Grammar, _input, ActionSelector, IgnoreUnknownActions>::template results<Pos, Stack, Subject, decision::accept>();
	}
	// if rule is reject => return false and subject
	template <size_t Pos, typename Terminal, typename Stack, typename Subject>
	static constexpr auto move(ctll::reject, Terminal, Stack, Subject) noexcept {
		return typename parser<Grammar, _input, ActionSelector, IgnoreUnknownActions>::template results<Pos, Stack, Subject, decision::reject>();
	}
	// if rule is pop_input => move to next character
	template <size_t Pos, typename Terminal, typename Stack, typename Subject>
	static constexpr auto move(ctll::pop_input, Terminal, Stack, Subject) noexcept {
		return typename parser<Grammar, _input, ActionSelector, IgnoreUnknownActions>::template results<Pos+1, Stack, Subject, decision::undecided>();
	}
	// if rule is string => push it to the front of stack
	template <size_t Pos, typename... Content, typename Terminal, typename Stack, typename Subject>
	static constexpr auto move(push<Content...> string, Terminal, Stack stack, Subject subject) noexcept {
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
	static constexpr auto move(push<term<V>, Content...>, term<V>, Stack stack, Subject) noexcept {
		constexpr auto _input = input;
		return typename parser<Grammar, _input, ActionSelector, IgnoreUnknownActions>::template results<Pos+1, decltype(push_front(list<Content...>(), stack)), Subject, decision::undecided>();
	}
	// if rule is string with any character at the beginning (compatible with current term<T>) => move to next character 
	// and push string without the character (quick LL(1))
	template <size_t Pos, auto V, typename... Content, auto T, typename Stack, typename Subject>
	static constexpr auto move(push<anything, Content...>, term<T>, Stack stack, Subject) noexcept {
		constexpr auto _input = input;
		return typename parser<Grammar, _input, ActionSelector, IgnoreUnknownActions>::template results<Pos+1, decltype(push_front(list<Content...>(), stack)), Subject, decision::undecided>();
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
				return typename parser<Grammar, _input, ActionSelector, IgnoreUnknownActions>::template results<Pos, Stack, Subject, decision::reject>();
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
	
	// trampolines with folded expression
	template <typename Subject, size_t... Pos> static constexpr auto trampoline_decide(Subject, std::index_sequence<Pos...>) noexcept {
		// parse everything for first char and than for next and next ...
		// Pos+1 is needed as we want to finish calculation with epsilons on stack
		auto v = (decide<0, typename grammar::start_stack, Subject>({}, {}) + ... + index_placeholder<Pos+1>());
		return v;
	}
	
	template <typename Subject = empty_subject> static constexpr auto trampoline_decide(Subject subject = {}) noexcept {
		// there will be no recursion, just sequence long as the input
		return trampoline_decide(subject, std::make_index_sequence<input.size()>());
	}
	
	template <typename Subject = empty_subject> using output = decltype(trampoline_decide<Subject>());
	template <typename Subject = empty_subject> static inline constexpr bool correct_with = trampoline_decide<Subject>();

};

} // end of ctll namespace

#endif

#endif

#ifndef CTRE__PCRE_ACTIONS__HPP
#define CTRE__PCRE_ACTIONS__HPP

#ifndef CTRE__PCRE__HPP
#define CTRE__PCRE__HPP

// THIS FILE WAS GENERATED BY DESATOMAT TOOL, DO NOT MODIFY THIS FILE

namespace ctre {

struct pcre {

// NONTERMINALS:
	struct a {};
	struct b {};
	struct backslash {};
	struct backslash_range {};
	struct block {};
	struct block_name2 {};
	struct block_name {};
	struct c {};
	struct class_named_name {};
	struct content2 {};
	struct content_in_capture {};
	struct d {};
	struct e {};
	struct f {};
	struct g {};
	struct h {};
	struct hexdec_repeat {};
	struct i {};
	struct j {};
	struct k {};
	struct l {};
	struct m {};
	struct mod {};
	struct mod_opt {};
	struct number2 {};
	struct number {};
	struct range {};
	struct repeat {};
	struct s {}; using _start = s;
	struct set2 {};
	struct set {};
	struct string2 {};

// 'action' types:
	struct class_digit: ctll::action {};
	struct class_named_alnum: ctll::action {};
	struct class_named_alpha: ctll::action {};
	struct class_named_ascii: ctll::action {};
	struct class_named_blank: ctll::action {};
	struct class_named_cntrl: ctll::action {};
	struct class_named_digit: ctll::action {};
	struct class_named_graph: ctll::action {};
	struct class_named_lower: ctll::action {};
	struct class_named_print: ctll::action {};
	struct class_named_punct: ctll::action {};
	struct class_named_space: ctll::action {};
	struct class_named_upper: ctll::action {};
	struct class_named_word: ctll::action {};
	struct class_named_xdigit: ctll::action {};
	struct class_nondigit: ctll::action {};
	struct class_nonnewline: ctll::action {};
	struct class_nonspace: ctll::action {};
	struct class_nonword: ctll::action {};
	struct class_space: ctll::action {};
	struct class_word: ctll::action {};
	struct create_hexdec: ctll::action {};
	struct create_number: ctll::action {};
	struct finish_hexdec: ctll::action {};
	struct look_finish: ctll::action {};
	struct make_alternate: ctll::action {};
	struct make_back_reference: ctll::action {};
	struct make_capture: ctll::action {};
	struct make_capture_with_name: ctll::action {};
	struct make_lazy: ctll::action {};
	struct make_optional: ctll::action {};
	struct make_possessive: ctll::action {};
	struct make_range: ctll::action {};
	struct make_relative_back_reference: ctll::action {};
	struct make_sequence: ctll::action {};
	struct negate_class_named: ctll::action {};
	struct prepare_capture: ctll::action {};
	struct push_assert_begin: ctll::action {};
	struct push_assert_end: ctll::action {};
	struct push_character: ctll::action {};
	struct push_character_alarm: ctll::action {};
	struct push_character_anything: ctll::action {};
	struct push_character_escape: ctll::action {};
	struct push_character_formfeed: ctll::action {};
	struct push_character_newline: ctll::action {};
	struct push_character_null: ctll::action {};
	struct push_character_return_carriage: ctll::action {};
	struct push_character_tab: ctll::action {};
	struct push_empty: ctll::action {};
	struct push_hexdec: ctll::action {};
	struct push_name: ctll::action {};
	struct push_number: ctll::action {};
	struct repeat_ab: ctll::action {};
	struct repeat_at_least: ctll::action {};
	struct repeat_exactly: ctll::action {};
	struct repeat_plus: ctll::action {};
	struct repeat_star: ctll::action {};
	struct reset_capture: ctll::action {};
	struct set_combine: ctll::action {};
	struct set_make: ctll::action {};
	struct set_make_negative: ctll::action {};
	struct set_start: ctll::action {};
	struct start_lookahead_negative: ctll::action {};
	struct start_lookahead_positive: ctll::action {};

// (q)LL1 function:
	using _others = ctll::neg_set<'!','$','\x28','\x29','*','+',',','-','.',':','<','=','>','?','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','0','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','\x7B','|','\x7D','1','2','3','4','5','6','7','8','9'>;
	static constexpr auto rule(s, ctll::term<'\\'>) -> ctll::push<ctll::anything, backslash, repeat, string2, content2>;
	static constexpr auto rule(s, ctll::term<'['>) -> ctll::push<ctll::anything, c, repeat, string2, content2>;
	static constexpr auto rule(s, ctll::term<'\x28'>) -> ctll::push<ctll::anything, prepare_capture, block, repeat, string2, content2>;
	static constexpr auto rule(s, ctll::term<'^'>) -> ctll::push<ctll::anything, push_assert_begin, repeat, string2, content2>;
	static constexpr auto rule(s, ctll::term<'$'>) -> ctll::push<ctll::anything, push_assert_end, repeat, string2, content2>;
	static constexpr auto rule(s, ctll::set<'!',',','-',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','Z',']','0','U','V','W','X','Y','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, repeat, string2, content2>;
	static constexpr auto rule(s, _others) -> ctll::push<ctll::anything, push_character, repeat, string2, content2>;
	static constexpr auto rule(s, ctll::term<'.'>) -> ctll::push<ctll::anything, push_character_anything, repeat, string2, content2>;
	static constexpr auto rule(s, ctll::epsilon) -> ctll::push<push_empty>;
	static constexpr auto rule(s, ctll::set<'\x29','*','+','?','_','\x7B','|','\x7D'>) -> ctll::reject;

	static constexpr auto rule(a, ctll::term<'\\'>) -> ctll::push<ctll::anything, backslash, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, ctll::term<'['>) -> ctll::push<ctll::anything, c, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, ctll::term<'\x28'>) -> ctll::push<ctll::anything, prepare_capture, block, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, ctll::term<'^'>) -> ctll::push<ctll::anything, push_assert_begin, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, ctll::term<'$'>) -> ctll::push<ctll::anything, push_assert_end, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, ctll::set<'!',',','-',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','Z',']','0','U','V','W','X','Y','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, _others) -> ctll::push<ctll::anything, push_character, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, ctll::term<'.'>) -> ctll::push<ctll::anything, push_character_anything, repeat, string2, content2, make_alternate>;
	static constexpr auto rule(a, ctll::term<'\x29'>) -> ctll::push<push_empty, make_alternate>;
	static constexpr auto rule(a, ctll::epsilon) -> ctll::push<push_empty, make_alternate>;
	static constexpr auto rule(a, ctll::set<'*','+','?','_','\x7B','|','\x7D'>) -> ctll::reject;

	static constexpr auto rule(b, ctll::term<','>) -> ctll::push<ctll::anything, l>;
	static constexpr auto rule(b, ctll::term<'\x7D'>) -> ctll::push<repeat_exactly, ctll::anything>;

	static constexpr auto rule(backslash, ctll::term<'d'>) -> ctll::push<ctll::anything, class_digit>;
	static constexpr auto rule(backslash, ctll::term<'D'>) -> ctll::push<ctll::anything, class_nondigit>;
	static constexpr auto rule(backslash, ctll::term<'N'>) -> ctll::push<ctll::anything, class_nonnewline>;
	static constexpr auto rule(backslash, ctll::term<'S'>) -> ctll::push<ctll::anything, class_nonspace>;
	static constexpr auto rule(backslash, ctll::term<'W'>) -> ctll::push<ctll::anything, class_nonword>;
	static constexpr auto rule(backslash, ctll::term<'s'>) -> ctll::push<ctll::anything, class_space>;
	static constexpr auto rule(backslash, ctll::term<'w'>) -> ctll::push<ctll::anything, class_word>;
	static constexpr auto rule(backslash, ctll::term<'u'>) -> ctll::push<ctll::anything, create_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, finish_hexdec>;
	static constexpr auto rule(backslash, ctll::term<'g'>) -> ctll::push<ctll::anything, ctll::term<'\x7B'>, k>;
	static constexpr auto rule(backslash, ctll::term<'x'>) -> ctll::push<ctll::anything, j>;
	static constexpr auto rule(backslash, ctll::set<'$','\x28','\x29','*','+','-','.','?','A','B','C','E','F','G','H','I','J','K','L','M','O','P','Q','U','V','X','Y','Z','[','\\',']','^','b','c','h','i','j','k','l','m','o','p','q','v','y','z','\x7B','|','\x7D','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character>;
	static constexpr auto rule(backslash, ctll::term<'a'>) -> ctll::push<ctll::anything, push_character_alarm>;
	static constexpr auto rule(backslash, ctll::term<'e'>) -> ctll::push<ctll::anything, push_character_escape>;
	static constexpr auto rule(backslash, ctll::term<'f'>) -> ctll::push<ctll::anything, push_character_formfeed>;
	static constexpr auto rule(backslash, ctll::term<'n'>) -> ctll::push<ctll::anything, push_character_newline>;
	static constexpr auto rule(backslash, ctll::term<'0'>) -> ctll::push<ctll::anything, push_character_null>;
	static constexpr auto rule(backslash, ctll::term<'r'>) -> ctll::push<ctll::anything, push_character_return_carriage>;
	static constexpr auto rule(backslash, ctll::term<'t'>) -> ctll::push<ctll::anything, push_character_tab>;

	static constexpr auto rule(backslash_range, ctll::term<'u'>) -> ctll::push<ctll::anything, create_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, finish_hexdec>;
	static constexpr auto rule(backslash_range, ctll::term<'x'>) -> ctll::push<ctll::anything, j>;
	static constexpr auto rule(backslash_range, ctll::term<'a'>) -> ctll::push<ctll::anything, push_character_alarm>;
	static constexpr auto rule(backslash_range, ctll::term<'e'>) -> ctll::push<ctll::anything, push_character_escape>;
	static constexpr auto rule(backslash_range, ctll::term<'f'>) -> ctll::push<ctll::anything, push_character_formfeed>;
	static constexpr auto rule(backslash_range, ctll::term<'n'>) -> ctll::push<ctll::anything, push_character_newline>;
	static constexpr auto rule(backslash_range, ctll::term<'0'>) -> ctll::push<ctll::anything, push_character_null>;
	static constexpr auto rule(backslash_range, ctll::term<'r'>) -> ctll::push<ctll::anything, push_character_return_carriage>;
	static constexpr auto rule(backslash_range, ctll::term<'t'>) -> ctll::push<ctll::anything, push_character_tab>;

	static constexpr auto rule(block, ctll::term<'\\'>) -> ctll::push<ctll::anything, backslash, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, ctll::term<'['>) -> ctll::push<ctll::anything, c, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, ctll::term<'?'>) -> ctll::push<ctll::anything, d>;
	static constexpr auto rule(block, ctll::term<'\x28'>) -> ctll::push<ctll::anything, prepare_capture, block, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, ctll::term<'^'>) -> ctll::push<ctll::anything, push_assert_begin, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, ctll::term<'$'>) -> ctll::push<ctll::anything, push_assert_end, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, ctll::set<'!',',','-',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','Z',']','0','U','V','W','X','Y','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, _others) -> ctll::push<ctll::anything, push_character, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, ctll::term<'.'>) -> ctll::push<ctll::anything, push_character_anything, repeat, string2, content2, make_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(block, ctll::term<'\x29'>) -> ctll::push<push_empty, make_capture, ctll::anything>;
	static constexpr auto rule(block, ctll::set<'*','+','_','\x7B','|','\x7D'>) -> ctll::reject;

	static constexpr auto rule(block_name2, ctll::set<'>','\x7D'>) -> ctll::epsilon;
	static constexpr auto rule(block_name2, ctll::set<'0','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_name, block_name2>;

	static constexpr auto rule(block_name, ctll::set<'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'>) -> ctll::push<ctll::anything, push_name, block_name2>;

	static constexpr auto rule(c, ctll::term<'['>) -> ctll::push<ctll::anything, ctll::term<':'>, h, range, set_start, set2, set_make, ctll::term<']'>>;
	static constexpr auto rule(c, ctll::term<'\\'>) -> ctll::push<ctll::anything, e, set_start, set2, set_make, ctll::term<']'>>;
	static constexpr auto rule(c, ctll::set<'!','$','\x28','\x29','*','+',',','.',':','=','?','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','0','r','s','t','u','v','w','x','y','z','\x7B','\x7D','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, range, set_start, set2, set_make, ctll::term<']'>>;
	static constexpr auto rule(c, _others) -> ctll::push<ctll::anything, push_character, range, set_start, set2, set_make, ctll::term<']'>>;
	static constexpr auto rule(c, ctll::term<'-'>) -> ctll::push<ctll::anything, push_character, set_start, set2, set_make, ctll::term<']'>>;
	static constexpr auto rule(c, ctll::term<'^'>) -> ctll::push<ctll::anything, set, set_make_negative, ctll::term<']'>>;
	static constexpr auto rule(c, ctll::set<'<','>',']','|'>) -> ctll::reject;

	static constexpr auto rule(class_named_name, ctll::term<'x'>) -> ctll::push<ctll::anything, ctll::term<'d'>, ctll::term<'i'>, ctll::term<'g'>, ctll::term<'i'>, ctll::term<'t'>, class_named_xdigit>;
	static constexpr auto rule(class_named_name, ctll::term<'d'>) -> ctll::push<ctll::anything, ctll::term<'i'>, ctll::term<'g'>, ctll::term<'i'>, ctll::term<'t'>, class_named_digit>;
	static constexpr auto rule(class_named_name, ctll::term<'b'>) -> ctll::push<ctll::anything, ctll::term<'l'>, ctll::term<'a'>, ctll::term<'n'>, ctll::term<'k'>, class_named_blank>;
	static constexpr auto rule(class_named_name, ctll::term<'c'>) -> ctll::push<ctll::anything, ctll::term<'n'>, ctll::term<'t'>, ctll::term<'r'>, ctll::term<'l'>, class_named_cntrl>;
	static constexpr auto rule(class_named_name, ctll::term<'w'>) -> ctll::push<ctll::anything, ctll::term<'o'>, ctll::term<'r'>, ctll::term<'d'>, class_named_word>;
	static constexpr auto rule(class_named_name, ctll::term<'l'>) -> ctll::push<ctll::anything, ctll::term<'o'>, ctll::term<'w'>, ctll::term<'e'>, ctll::term<'r'>, class_named_lower>;
	static constexpr auto rule(class_named_name, ctll::term<'s'>) -> ctll::push<ctll::anything, ctll::term<'p'>, ctll::term<'a'>, ctll::term<'c'>, ctll::term<'e'>, class_named_space>;
	static constexpr auto rule(class_named_name, ctll::term<'u'>) -> ctll::push<ctll::anything, ctll::term<'p'>, ctll::term<'p'>, ctll::term<'e'>, ctll::term<'r'>, class_named_upper>;
	static constexpr auto rule(class_named_name, ctll::term<'g'>) -> ctll::push<ctll::anything, ctll::term<'r'>, ctll::term<'a'>, ctll::term<'p'>, ctll::term<'h'>, class_named_graph>;
	static constexpr auto rule(class_named_name, ctll::term<'a'>) -> ctll::push<ctll::anything, f>;
	static constexpr auto rule(class_named_name, ctll::term<'p'>) -> ctll::push<ctll::anything, g>;

	static constexpr auto rule(content2, ctll::term<'\x29'>) -> ctll::epsilon;
	static constexpr auto rule(content2, ctll::epsilon) -> ctll::epsilon;
	static constexpr auto rule(content2, ctll::term<'|'>) -> ctll::push<ctll::anything, a>;

	static constexpr auto rule(content_in_capture, ctll::term<'\\'>) -> ctll::push<ctll::anything, backslash, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, ctll::term<'['>) -> ctll::push<ctll::anything, c, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, ctll::term<'\x28'>) -> ctll::push<ctll::anything, prepare_capture, block, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, ctll::term<'^'>) -> ctll::push<ctll::anything, push_assert_begin, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, ctll::term<'$'>) -> ctll::push<ctll::anything, push_assert_end, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, ctll::set<'!',',','-',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','Z',']','0','U','V','W','X','Y','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, _others) -> ctll::push<ctll::anything, push_character, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, ctll::term<'.'>) -> ctll::push<ctll::anything, push_character_anything, repeat, string2, content2>;
	static constexpr auto rule(content_in_capture, ctll::term<'\x29'>) -> ctll::push<push_empty>;
	static constexpr auto rule(content_in_capture, ctll::set<'*','+','?','_','\x7B','|','\x7D'>) -> ctll::reject;

	static constexpr auto rule(d, ctll::term<'<'>) -> ctll::push<ctll::anything, block_name, ctll::term<'>'>, content_in_capture, make_capture_with_name, ctll::term<'\x29'>>;
	static constexpr auto rule(d, ctll::term<':'>) -> ctll::push<reset_capture, ctll::anything, content_in_capture, ctll::term<'\x29'>>;
	static constexpr auto rule(d, ctll::term<'!'>) -> ctll::push<reset_capture, ctll::anything, start_lookahead_negative, content_in_capture, look_finish, ctll::term<'\x29'>>;
	static constexpr auto rule(d, ctll::term<'='>) -> ctll::push<reset_capture, ctll::anything, start_lookahead_positive, content_in_capture, look_finish, ctll::term<'\x29'>>;

	static constexpr auto rule(e, ctll::term<'d'>) -> ctll::push<ctll::anything, class_digit>;
	static constexpr auto rule(e, ctll::term<'D'>) -> ctll::push<ctll::anything, class_nondigit>;
	static constexpr auto rule(e, ctll::term<'N'>) -> ctll::push<ctll::anything, class_nonnewline>;
	static constexpr auto rule(e, ctll::term<'S'>) -> ctll::push<ctll::anything, class_nonspace>;
	static constexpr auto rule(e, ctll::term<'W'>) -> ctll::push<ctll::anything, class_nonword>;
	static constexpr auto rule(e, ctll::term<'s'>) -> ctll::push<ctll::anything, class_space>;
	static constexpr auto rule(e, ctll::term<'w'>) -> ctll::push<ctll::anything, class_word>;
	static constexpr auto rule(e, ctll::term<'u'>) -> ctll::push<ctll::anything, create_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, finish_hexdec, range>;
	static constexpr auto rule(e, ctll::term<'x'>) -> ctll::push<ctll::anything, j, range>;
	static constexpr auto rule(e, ctll::set<'$','\x28','\x29','*','+','-','.','?','A','B','C','E','F','G','H','I','J','K','L','M','O','P','Q','U','V','X','Y','Z','[','\\',']','^','b','c','h','i','j','k','l','m','o','p','q','v','y','z','\x7B','|','\x7D','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character>;
	static constexpr auto rule(e, ctll::term<'a'>) -> ctll::push<ctll::anything, push_character_alarm, range>;
	static constexpr auto rule(e, ctll::term<'e'>) -> ctll::push<ctll::anything, push_character_escape, range>;
	static constexpr auto rule(e, ctll::term<'f'>) -> ctll::push<ctll::anything, push_character_formfeed, range>;
	static constexpr auto rule(e, ctll::term<'n'>) -> ctll::push<ctll::anything, push_character_newline, range>;
	static constexpr auto rule(e, ctll::term<'0'>) -> ctll::push<ctll::anything, push_character_null, range>;
	static constexpr auto rule(e, ctll::term<'r'>) -> ctll::push<ctll::anything, push_character_return_carriage, range>;
	static constexpr auto rule(e, ctll::term<'t'>) -> ctll::push<ctll::anything, push_character_tab, range>;

	static constexpr auto rule(f, ctll::term<'s'>) -> ctll::push<ctll::anything, ctll::term<'c'>, ctll::term<'i'>, ctll::term<'i'>, class_named_ascii>;
	static constexpr auto rule(f, ctll::term<'l'>) -> ctll::push<ctll::anything, m>;

	static constexpr auto rule(g, ctll::term<'r'>) -> ctll::push<ctll::anything, ctll::term<'i'>, ctll::term<'n'>, ctll::term<'t'>, class_named_print>;
	static constexpr auto rule(g, ctll::term<'u'>) -> ctll::push<ctll::anything, ctll::term<'n'>, ctll::term<'c'>, ctll::term<'t'>, class_named_punct>;

	static constexpr auto rule(h, ctll::term<'^'>) -> ctll::push<ctll::anything, class_named_name, negate_class_named, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'x'>) -> ctll::push<ctll::anything, ctll::term<'d'>, ctll::term<'i'>, ctll::term<'g'>, ctll::term<'i'>, ctll::term<'t'>, class_named_xdigit, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'d'>) -> ctll::push<ctll::anything, ctll::term<'i'>, ctll::term<'g'>, ctll::term<'i'>, ctll::term<'t'>, class_named_digit, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'b'>) -> ctll::push<ctll::anything, ctll::term<'l'>, ctll::term<'a'>, ctll::term<'n'>, ctll::term<'k'>, class_named_blank, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'c'>) -> ctll::push<ctll::anything, ctll::term<'n'>, ctll::term<'t'>, ctll::term<'r'>, ctll::term<'l'>, class_named_cntrl, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'w'>) -> ctll::push<ctll::anything, ctll::term<'o'>, ctll::term<'r'>, ctll::term<'d'>, class_named_word, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'l'>) -> ctll::push<ctll::anything, ctll::term<'o'>, ctll::term<'w'>, ctll::term<'e'>, ctll::term<'r'>, class_named_lower, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'s'>) -> ctll::push<ctll::anything, ctll::term<'p'>, ctll::term<'a'>, ctll::term<'c'>, ctll::term<'e'>, class_named_space, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'u'>) -> ctll::push<ctll::anything, ctll::term<'p'>, ctll::term<'p'>, ctll::term<'e'>, ctll::term<'r'>, class_named_upper, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'g'>) -> ctll::push<ctll::anything, ctll::term<'r'>, ctll::term<'a'>, ctll::term<'p'>, ctll::term<'h'>, class_named_graph, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'a'>) -> ctll::push<ctll::anything, f, ctll::term<':'>, ctll::term<']'>>;
	static constexpr auto rule(h, ctll::term<'p'>) -> ctll::push<ctll::anything, g, ctll::term<':'>, ctll::term<']'>>;

	static constexpr auto rule(hexdec_repeat, ctll::term<'\x7D'>) -> ctll::epsilon;
	static constexpr auto rule(hexdec_repeat, ctll::set<'0','A','B','C','D','E','F','a','b','c','d','e','f','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_hexdec, hexdec_repeat>;

	static constexpr auto rule(i, ctll::term<'\\'>) -> ctll::push<ctll::anything, backslash_range, make_range>;
	static constexpr auto rule(i, ctll::set<'!','$','\x28','\x29','*','+',',','.',':','=','?','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','0','r','s','t','u','v','w','x','y','z','\x7B','\x7D','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, make_range>;
	static constexpr auto rule(i, _others) -> ctll::push<ctll::anything, push_character, make_range>;
	static constexpr auto rule(i, ctll::set<'-','<','>','[',']','^','|'>) -> ctll::reject;

	static constexpr auto rule(j, ctll::term<'\x7B'>) -> ctll::push<create_hexdec, ctll::anything, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, hexdec_repeat, ctll::term<'\x7D'>, finish_hexdec>;
	static constexpr auto rule(j, ctll::set<'0','A','B','C','D','E','F','a','b','c','d','e','f','1','2','3','4','5','6','7','8','9'>) -> ctll::push<create_hexdec, ctll::anything, push_hexdec, ctll::set<'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f'>, push_hexdec, finish_hexdec>;

	static constexpr auto rule(k, ctll::set<'0','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, create_number, number2, ctll::term<'\x7D'>, make_back_reference>;
	static constexpr auto rule(k, ctll::term<'-'>) -> ctll::push<ctll::anything, number, ctll::term<'\x7D'>, make_relative_back_reference>;
	static constexpr auto rule(k, ctll::set<'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'>) -> ctll::push<ctll::anything, push_name, block_name2, ctll::term<'\x7D'>, make_back_reference>;

	static constexpr auto rule(l, ctll::set<'0','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, create_number, number2, repeat_ab, ctll::term<'\x7D'>, mod>;
	static constexpr auto rule(l, ctll::term<'\x7D'>) -> ctll::push<repeat_at_least, ctll::anything, mod>;

	static constexpr auto rule(m, ctll::term<'p'>) -> ctll::push<ctll::anything, ctll::term<'h'>, ctll::term<'a'>, class_named_alpha>;
	static constexpr auto rule(m, ctll::term<'n'>) -> ctll::push<ctll::anything, ctll::term<'u'>, ctll::term<'m'>, class_named_alnum>;

	static constexpr auto rule(mod, ctll::set<'!','$','\x28','\x29',',','-','.',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','|','0','1','2','3','4','5','6','7','8','9'>) -> ctll::epsilon;
	static constexpr auto rule(mod, ctll::epsilon) -> ctll::epsilon;
	static constexpr auto rule(mod, _others) -> ctll::epsilon;
	static constexpr auto rule(mod, ctll::term<'?'>) -> ctll::push<ctll::anything, make_lazy>;
	static constexpr auto rule(mod, ctll::term<'+'>) -> ctll::push<ctll::anything, make_possessive>;
	static constexpr auto rule(mod, ctll::set<'*','_','\x7B','\x7D'>) -> ctll::reject;

	static constexpr auto rule(mod_opt, ctll::set<'!','$','\x28','\x29',',','-','.',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','|','0','1','2','3','4','5','6','7','8','9'>) -> ctll::epsilon;
	static constexpr auto rule(mod_opt, ctll::epsilon) -> ctll::epsilon;
	static constexpr auto rule(mod_opt, _others) -> ctll::epsilon;
	static constexpr auto rule(mod_opt, ctll::term<'?'>) -> ctll::push<ctll::anything, make_lazy>;
	static constexpr auto rule(mod_opt, ctll::set<'*','+','_','\x7B','\x7D'>) -> ctll::reject;

	static constexpr auto rule(number2, ctll::set<',','\x7D'>) -> ctll::epsilon;
	static constexpr auto rule(number2, ctll::set<'0','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_number, number2>;

	static constexpr auto rule(number, ctll::set<'0','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, create_number, number2>;

	static constexpr auto rule(range, ctll::set<'!','$','\x28','\x29','*','+',',','.',':','=','?','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','\x7B','\x7D','0','1','2','3','4','5','6','7','8','9'>) -> ctll::epsilon;
	static constexpr auto rule(range, ctll::epsilon) -> ctll::epsilon;
	static constexpr auto rule(range, _others) -> ctll::epsilon;
	static constexpr auto rule(range, ctll::term<'-'>) -> ctll::push<ctll::anything, i>;
	static constexpr auto rule(range, ctll::set<'<','>','^','|'>) -> ctll::reject;

	static constexpr auto rule(repeat, ctll::set<'!','$','\x28','\x29',',','-','.',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','|','0','1','2','3','4','5','6','7','8','9'>) -> ctll::epsilon;
	static constexpr auto rule(repeat, ctll::epsilon) -> ctll::epsilon;
	static constexpr auto rule(repeat, _others) -> ctll::epsilon;
	static constexpr auto rule(repeat, ctll::term<'?'>) -> ctll::push<ctll::anything, make_optional, mod_opt>;
	static constexpr auto rule(repeat, ctll::term<'\x7B'>) -> ctll::push<ctll::anything, number, b>;
	static constexpr auto rule(repeat, ctll::term<'+'>) -> ctll::push<ctll::anything, repeat_plus, mod>;
	static constexpr auto rule(repeat, ctll::term<'*'>) -> ctll::push<ctll::anything, repeat_star, mod>;
	static constexpr auto rule(repeat, ctll::set<'_','\x7D'>) -> ctll::reject;

	static constexpr auto rule(set2, ctll::term<']'>) -> ctll::epsilon;
	static constexpr auto rule(set2, ctll::term<'['>) -> ctll::push<ctll::anything, ctll::term<':'>, h, range, set_combine, set2>;
	static constexpr auto rule(set2, ctll::term<'\\'>) -> ctll::push<ctll::anything, e, set_combine, set2>;
	static constexpr auto rule(set2, ctll::set<'!','$','\x28','\x29','*','+',',','.',':','=','?','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','0','r','s','t','u','v','w','x','y','z','\x7B','\x7D','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, range, set_combine, set2>;
	static constexpr auto rule(set2, _others) -> ctll::push<ctll::anything, push_character, range, set_combine, set2>;
	static constexpr auto rule(set2, ctll::set<'-','<','>','^','|'>) -> ctll::reject;

	static constexpr auto rule(set, ctll::term<'['>) -> ctll::push<ctll::anything, ctll::term<':'>, h, range, set_start, set2>;
	static constexpr auto rule(set, ctll::term<'\\'>) -> ctll::push<ctll::anything, e, set_start, set2>;
	static constexpr auto rule(set, ctll::set<'!','$','\x28','\x29','*','+',',','.',':','=','?','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','0','r','s','t','u','v','w','x','y','z','\x7B','\x7D','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, range, set_start, set2>;
	static constexpr auto rule(set, _others) -> ctll::push<ctll::anything, push_character, range, set_start, set2>;
	static constexpr auto rule(set, ctll::term<'-'>) -> ctll::push<ctll::anything, push_character, set_start, set2>;
	static constexpr auto rule(set, ctll::set<'<','>',']','^','|'>) -> ctll::reject;

	static constexpr auto rule(string2, ctll::set<'\x29','|'>) -> ctll::epsilon;
	static constexpr auto rule(string2, ctll::epsilon) -> ctll::epsilon;
	static constexpr auto rule(string2, ctll::term<'\\'>) -> ctll::push<ctll::anything, backslash, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, ctll::term<'['>) -> ctll::push<ctll::anything, c, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, ctll::term<'\x28'>) -> ctll::push<ctll::anything, prepare_capture, block, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, ctll::term<'^'>) -> ctll::push<ctll::anything, push_assert_begin, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, ctll::term<'$'>) -> ctll::push<ctll::anything, push_assert_end, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, ctll::set<'!',',','-',':','<','=','>','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','Z',']','0','U','V','W','X','Y','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9'>) -> ctll::push<ctll::anything, push_character, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, _others) -> ctll::push<ctll::anything, push_character, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, ctll::term<'.'>) -> ctll::push<ctll::anything, push_character_anything, repeat, string2, make_sequence>;
	static constexpr auto rule(string2, ctll::set<'*','+','?','_','\x7B','\x7D'>) -> ctll::reject;

};

}

#endif //CTRE__PCRE__HPP

#ifndef CTRE__ATOMS__HPP
#define CTRE__ATOMS__HPP

#include <cstdint>

namespace ctre {
	
// special helpers for matching
struct accept { };
struct start_mark { };
struct end_mark { };
struct end_cycle_mark { };
struct end_lookahead_mark { };
template <size_t Id> struct numeric_mark { };

// actual AST of regexp
template <auto... Str> struct string { };
template <typename... Opts> struct select { };
template <typename... Content> struct optional { };
template <typename... Content> struct lazy_optional { };
template <typename... Content> struct sequence { };
struct empty { };

template <typename... Content> struct plus { };
template <typename... Content> struct star { };
template <size_t a, size_t b, typename... Content> struct repeat { };

template <typename... Content> struct lazy_plus { };
template <typename... Content> struct lazy_star { };
template <size_t a, size_t b, typename... Content> struct lazy_repeat { };

template <typename... Content> struct possessive_plus { };
template <typename... Content> struct possessive_star { };
template <size_t a, size_t b, typename... Content> struct possessive_repeat { };

template <size_t Index, typename... Content> struct capture { };

template <size_t Index, typename Name, typename... Content> struct capture_with_name { };

template <size_t Index> struct back_reference { };
template <typename Name> struct back_reference_with_name { };

template <typename Type> struct look_start { };

template <typename... Content> struct lookahead_positive { };
template <typename... Content> struct lookahead_negative { };

struct assert_begin { };
struct assert_end { };

}

#endif

#ifndef CTRE__ATOMS_CHARACTERS__HPP
#define CTRE__ATOMS_CHARACTERS__HPP

#ifndef CTRE__UTILITY__HPP
#define CTRE__UTILITY__HPP

#ifdef _MSC_VER
#define CTRE_FORCE_INLINE __forceinline
#define CTRE_FLATTEN
#else
#define CTRE_FORCE_INLINE inline __attribute__((always_inline))
#define CTRE_FLATTEN __attribute__((flatten))
#endif

#endif
#include <cstdint>

namespace ctre {
	
// sfinae check for types here

template <typename T> class MatchesCharacter {
	template <typename Y, typename CharT> static auto test(CharT c) -> decltype(Y::match_char(c), std::true_type());
	template <typename> static auto test(...) -> std::false_type;
public:
	template <typename CharT> static inline constexpr bool value = decltype(test<T>(std::declval<CharT>()))();
};

template <auto V> struct character {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT value) noexcept {
		return value == V;
	}
};

struct any {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT) noexcept { return true; }
};

template <typename... Content> struct negative_set {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}
};

template <typename... Content> struct set {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return (Content::match_char(value) || ... || false);
	}
};

template <auto... Cs> struct enumeration : set<character<Cs>...> { };

template <typename... Content> struct negate {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}
};

template <auto A, auto B> struct char_range {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT value) noexcept {
		return (value >= A) && (value <= B);
	}
};

struct word_chars : set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'>, character<'_'> > { };

struct space_chars : enumeration<' ', '\t', '\n', '\v', '\f', '\r'> {};

struct alphanum_chars : set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'> > { };

struct alpha_chars : set<char_range<'A','Z'>, char_range<'a','z'> > { };

struct xdigit_chars : set<char_range<'A','F'>, char_range<'a','f'>, char_range<'0','9'> > { };

struct punct_chars
    : enumeration<'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', ',', '-',
		  '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']',
		  '^', '_', '`', '{', '|', '}', '~'> {};

struct digit_chars : char_range<'0','9'> { };

struct ascii_chars : char_range<'\x00','\x7F'> { };

}

#endif

#ifndef CTRE__ID__HPP
#define CTRE__ID__HPP

#include <type_traits>

namespace ctre {
	
template <auto...> struct id { };
	
template <auto... Name> constexpr auto operator==(id<Name...>, id<Name...>) noexcept -> std::true_type { return {}; }

template <auto... Name1, auto... Name2> constexpr auto operator==(id<Name1...>, id<Name2...>) noexcept -> std::false_type { return {}; }
	
template <auto... Name, typename T> constexpr auto operator==(id<Name...>, T) noexcept -> std::false_type { return {}; }

}

#endif
#include <cstdint>
#include <limits>

namespace ctre {
	
template <size_t Counter> struct pcre_parameters {
	static constexpr size_t current_counter = Counter;
};
	
template <typename Stack = ctll::list<>, typename Parameters = pcre_parameters<0>> struct pcre_context {
	using stack_type = Stack;
	using parameters_type = Parameters;
	static constexpr inline auto stack = stack_type();
	static constexpr inline auto parameters = parameters_type();
	constexpr pcre_context() noexcept { }
	constexpr pcre_context(Stack, Parameters) noexcept { }
};

template <typename... Content, typename Parameters> pcre_context(ctll::list<Content...>, Parameters) -> pcre_context<ctll::list<Content...>, Parameters>;
	
template <size_t Value> struct number { };

template <size_t Id> struct capture_id { };
	
struct pcre_actions {
// i know it's ugly, but it's more readable
#ifndef CTRE__ACTIONS__ASSERTS__HPP
#define CTRE__ACTIONS__ASSERTS__HPP

// push_assert_begin
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_assert_begin, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(assert_begin(), subject.stack), subject.parameters};
}

// push_assert_end
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_assert_end, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(assert_end(), subject.stack), subject.parameters};
}

#endif
	
#ifndef CTRE__ACTIONS__BACKREFERENCE__HPP
#define CTRE__ACTIONS__BACKREFERENCE__HPP

// backreference with name
template <auto... Str, auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_back_reference, ctll::term<V>, pcre_context<ctll::list<id<Str...>, Ts...>, pcre_parameters<Counter>>) {	
	return pcre_context{ctll::push_front(back_reference_with_name<id<Str...>>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}

// with just a number
template <auto V, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_back_reference, ctll::term<V>, pcre_context<ctll::list<number<Id>, Ts...>, pcre_parameters<Counter>>) {
	// if we are looking outside of existing list of Ids ... reject input during parsing
	if constexpr (Counter < Id) {
		return ctll::reject{};
	} else {
		return pcre_context{ctll::push_front(back_reference<Id>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
	}
}

// relative backreference
template <auto V, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_relative_back_reference, ctll::term<V>, [[maybe_unused]] pcre_context<ctll::list<number<Id>, Ts...>, pcre_parameters<Counter>>) {	
	// if we are looking outside of existing list of Ids ... reject input during parsing
	if constexpr (Counter < Id) {
		return ctll::reject{};
	} else {
		constexpr size_t absolute_id = (Counter + 1) - Id;
		return pcre_context{ctll::push_front(back_reference<absolute_id>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
	}
}

#endif

#ifndef CTRE__ACTIONS__CAPTURE__HPP
#define CTRE__ACTIONS__CAPTURE__HPP

// prepare_capture
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::prepare_capture, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_id<Counter+1>(), ctll::list<Ts...>()), pcre_parameters<Counter+1>()};
}

// reset_capture
template <auto V, typename... Ts, size_t Id, size_t Counter> static constexpr auto apply(pcre::reset_capture, ctll::term<V>, pcre_context<ctll::list<capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<Ts...>(), pcre_parameters<Counter-1>()};
}

// capture
template <auto V, typename A, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture, ctll::term<V>, pcre_context<ctll::list<A, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture<Id, A>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}
// capture (sequence)
template <auto V, typename... Content, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture<Id, Content...>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}
// push_name
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_name, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(id<V>(), subject.stack), subject.parameters};
}
// push_name (concat)
template <auto... Str, auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_name, ctll::term<V>, pcre_context<ctll::list<id<Str...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(id<Str..., V>(), ctll::list<Ts...>()), subject.parameters};
}
// capture with name
template <auto... Str, auto V, typename A, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture_with_name, ctll::term<V>, pcre_context<ctll::list<A, id<Str...>, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_with_name<Id, id<Str...>, A>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}
// capture with name (sequence)
template <auto... Str, auto V, typename... Content, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture_with_name, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, id<Str...>, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_with_name<Id, id<Str...>, Content...>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}

#endif

#ifndef CTRE__ACTIONS__CHARACTERS__HPP
#define CTRE__ACTIONS__CHARACTERS__HPP

// push character
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<V>(), subject.stack), subject.parameters};
}
// push_any_character
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_anything, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(any(), subject.stack), subject.parameters};
}
// character_alarm
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_alarm, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x07'>(), subject.stack), subject.parameters};
}
// character_escape
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_escape, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x14'>(), subject.stack), subject.parameters};
}
// character_formfeed
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_formfeed, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x0C'>(), subject.stack), subject.parameters};
}
// push_character_newline
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_newline, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x0A'>(), subject.stack), subject.parameters};
}
// push_character_null
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_null, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\0'>(), subject.stack), subject.parameters};
}
// push_character_return_carriage
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_return_carriage, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x0D'>(), subject.stack), subject.parameters};
}
// push_character_tab
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_character_tab, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(character<'\x09'>(), subject.stack), subject.parameters};
}

#endif

#ifndef CTRE__ACTIONS__CLASS__HPP
#define CTRE__ACTIONS__CLASS__HPP

// class_digit
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_digit, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::digit_chars>(), subject.stack), subject.parameters};
}
// class_non_digit
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nondigit, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::digit_chars>(), subject.stack), subject.parameters};
}
// class_space
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_space, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::space_chars>(), subject.stack), subject.parameters};
}
// class_nonspace
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nonspace, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::space_chars>(), subject.stack), subject.parameters};
}
// class_word
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_word, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::word_chars>(), subject.stack), subject.parameters};
}
// class_nonword
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nonword, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<ctre::word_chars>(), subject.stack), subject.parameters};
}
// class_nonnewline
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_nonnewline, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::negative_set<character<'\n'>>(), subject.stack), subject.parameters};
}

#endif

#ifndef CTRE__ACTIONS__HEXDEC__HPP
#define CTRE__ACTIONS__HEXDEC__HPP

// hexdec character support (seed)
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::create_hexdec, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(number<0ull>(), subject.stack), subject.parameters};
}
// hexdec character support (push value)
template <auto V, size_t N, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_hexdec, ctll::term<V>, pcre_context<ctll::list<number<N>, Ts...>, Parameters> subject) {
	constexpr auto previous = N << 4ull;	
	if constexpr (V >= 'a' && V <= 'f') {
		return pcre_context{ctll::push_front(number<(previous + (V - 'a' + 10))>(), ctll::list<Ts...>()), subject.parameters};
	} else if constexpr (V >= 'A' && V <= 'F') {
		return pcre_context{ctll::push_front(number<(previous + (V - 'A' + 10))>(), ctll::list<Ts...>()), subject.parameters};
	} else {
		return pcre_context{ctll::push_front(number<(previous + (V - '0'))>(), ctll::list<Ts...>()), subject.parameters};
	}
}
// hexdec character support (convert to character)
template <auto V, size_t N, typename... Ts, typename Parameters> static constexpr auto apply(pcre::finish_hexdec, ctll::term<V>, pcre_context<ctll::list<number<N>, Ts...>, Parameters> subject) {
	if constexpr (N <= std::numeric_limits<unsigned char>::max()) {
		return pcre_context{ctll::push_front(character<(char)N>(), ctll::list<Ts...>()), subject.parameters};
	} else {
		return pcre_context{ctll::push_front(character<N>(), ctll::list<Ts...>()), subject.parameters};
	} 
}	

#endif

#ifndef CTRE__ACTIONS__LOOKAHEAD__HPP
#define CTRE__ACTIONS__LOOKAHEAD__HPP

// lookahead positive start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_lookahead_positive, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<look_start<lookahead_positive<>>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead positive end
template <auto V, typename Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<Look, look_start<lookahead_positive<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_positive<Look>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead positive end (sequence)
template <auto V, typename... Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<ctre::sequence<Look...>, look_start<lookahead_positive<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_positive<Look...>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead negative start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_lookahead_negative, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<look_start<lookahead_negative<>>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead negative end
template <auto V, typename Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<Look, look_start<lookahead_negative<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_negative<Look>, Ts...>(), pcre_parameters<Counter>()};
}

// lookahead negative end (sequence)
template <auto V, typename... Look, typename... Ts, size_t Counter> static constexpr auto apply(pcre::look_finish, ctll::term<V>, pcre_context<ctll::list<ctre::sequence<Look...>, look_start<lookahead_negative<>>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<lookahead_negative<Look...>, Ts...>(), pcre_parameters<Counter>()};
}

#endif

#ifndef CTRE__ACTIONS__NAMED_CLASS__HPP
#define CTRE__ACTIONS__NAMED_CLASS__HPP

// class_named_alnum
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_alnum, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::alphanum_chars(), subject.stack), subject.parameters};
}
// class_named_alpha
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_alpha, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::alpha_chars(), subject.stack), subject.parameters};
}
// class_named_digit
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_digit, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::digit_chars(), subject.stack), subject.parameters};
}
// class_named_ascii
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_ascii, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::ascii_chars(), subject.stack), subject.parameters};
}
// class_named_blank
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_blank, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::enumeration<' ','\t'>(), subject.stack), subject.parameters};
}
// class_named_cntrl
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_cntrl, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::set<ctre::char_range<'\x00','\x1F'>, ctre::character<'\x7F'>>(), subject.stack), subject.parameters};
}
// class_named_graph
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_graph, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::char_range<'\x21','\x7E'>(), subject.stack), subject.parameters};
}
// class_named_lower
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_lower, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::char_range<'a','z'>(), subject.stack), subject.parameters};
}
// class_named_upper
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_upper, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::char_range<'A','Z'>(), subject.stack), subject.parameters};
}
// class_named_print
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_print, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(ctre::char_range<'\x20','\x7E'>(), subject.stack), subject.parameters};
}
// class_named_space
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_space, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(space_chars(), subject.stack), subject.parameters};
}
// class_named_word
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_word, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(word_chars(), subject.stack), subject.parameters};
}
// class_named_punct
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_punct, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(punct_chars(), subject.stack), subject.parameters};
}
// class_named_xdigit
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::class_named_xdigit, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(xdigit_chars(), subject.stack), subject.parameters};
}

#endif

#ifndef CTRE__ACTIONS__OPTIONS__HPP
#define CTRE__ACTIONS__OPTIONS__HPP

// empty option for alternate
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_empty, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(empty(), subject.stack), subject.parameters};
}

// empty option for empty regex
template <typename Parameters> static constexpr auto apply(pcre::push_empty, ctll::epsilon, pcre_context<ctll::list<>, Parameters> subject) {
	return pcre_context{ctll::push_front(empty(), subject.stack), subject.parameters};
}

// make_alternate (A|B)
template <auto V, typename A, typename B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_alternate, ctll::term<V>, pcre_context<ctll::list<B, A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(select<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_alternate (As..)|B => (As..|B)
template <auto V, typename A, typename... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_alternate, ctll::term<V>, pcre_context<ctll::list<ctre::select<Bs...>, A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(select<A,Bs...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_optional
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(optional<A>(), ctll::list<Ts...>()), subject.parameters};
}

template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_optional, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(optional<Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (optional)
template <auto V, typename... Subject, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<optional<Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(lazy_optional<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

#endif

#ifndef CTRE__ACTIONS__REPEAT__HPP
#define CTRE__ACTIONS__REPEAT__HPP

// repeat 1..N
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_plus, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(plus<A>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat 1..N (sequence)
template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_plus, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(plus<Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat 0..N
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_star, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(star<A>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat 0..N (sequence)
template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_star, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(star<Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// create_number (seed)
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::create_number, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(number<static_cast<size_t>(V - '0')>(), subject.stack), subject.parameters};
}
// push_number
template <auto V, size_t N, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_number, ctll::term<V>, pcre_context<ctll::list<number<N>, Ts...>, Parameters> subject) {
	constexpr size_t previous = N * 10ull;	
	return pcre_context{ctll::push_front(number<(previous + (V - '0'))>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat A..B
template <auto V, typename Subject, size_t A, size_t B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_ab, ctll::term<V>, pcre_context<ctll::list<number<B>, number<A>, Subject, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(repeat<A,B,Subject>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat A..B (sequence)
template <auto V, typename... Content, size_t A, size_t B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_ab, ctll::term<V>, pcre_context<ctll::list<number<B>, number<A>, sequence<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(repeat<A,B,Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat_exactly 
template <auto V, typename Subject, size_t A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_exactly, ctll::term<V>, pcre_context<ctll::list<number<A>, Subject, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(repeat<A,A,Subject>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat_exactly A..B (sequence)
template <auto V, typename... Content, size_t A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_exactly, ctll::term<V>, pcre_context<ctll::list<number<A>, sequence<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(repeat<A,A,Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// repeat_at_least (A+) 
template <auto V, typename Subject, size_t A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_at_least, ctll::term<V>, pcre_context<ctll::list<number<A>, Subject, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(repeat<A,0,Subject>(), ctll::list<Ts...>()), subject.parameters};
}
// repeat_at_least (A+) (sequence)
template <auto V, typename... Content, size_t A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::repeat_at_least, ctll::term<V>, pcre_context<ctll::list<number<A>, sequence<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(repeat<A,0,Content...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (plus)
template <auto V, typename... Subject, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<plus<Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(lazy_plus<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (star)
template <auto V, typename... Subject, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<star<Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(lazy_star<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_lazy (repeat<A,B>)
template <auto V, typename... Subject, size_t A, size_t B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_lazy, ctll::term<V>, pcre_context<ctll::list<repeat<A,B,Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(lazy_repeat<A,B,Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_possessive (plus)
template <auto V, typename... Subject, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_possessive, ctll::term<V>, pcre_context<ctll::list<plus<Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(possessive_plus<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_possessive (star)
template <auto V, typename... Subject, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_possessive, ctll::term<V>, pcre_context<ctll::list<star<Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(possessive_star<Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

// make_possessive (repeat<A,B>)
template <auto V, typename... Subject, size_t A, size_t B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_possessive, ctll::term<V>, pcre_context<ctll::list<repeat<A,B,Subject...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(possessive_repeat<A,B,Subject...>(), ctll::list<Ts...>()), subject.parameters};
}

#endif

#ifndef CTRE__ACTIONS__SEQUENCE__HPP
#define CTRE__ACTIONS__SEQUENCE__HPP

// make_sequence
template <auto V, typename A, typename B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<B,A,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (concat)
template <auto V, typename... As, typename B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<sequence<As...>,B,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(sequence<B,As...>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (make string)
template <auto V, auto A, auto B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<character<B>,character<A>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(string<A,B>(), ctll::list<Ts...>()), subject.parameters};
}
// make_sequence (concat string)
template <auto V, auto... As, auto B, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_sequence, ctll::term<V>, pcre_context<ctll::list<string<As...>,character<B>,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(string<B,As...>(), ctll::list<Ts...>()), subject.parameters};
}

#endif

#ifndef CTRE__ACTIONS__SET__HPP
#define CTRE__ACTIONS__SET__HPP

// UTILITY
// add into set if not exists
template <template <typename...> typename SetType, typename T, typename... As, bool Exists = (std::is_same_v<T, As> || ... || false)> static constexpr auto push_back_into_set(T, SetType<As...>) -> ctll::conditional<Exists, SetType<As...>, SetType<As...,T>> { return {}; }

//template <template <typename...> typename SetType, typename A, typename BHead, typename... Bs> struct set_merge_helper {
//	using step = decltype(push_back_into_set<SetType>(BHead(), A()));
//	using type = ctll::conditional<(sizeof...(Bs) > 0), set_merge_helper<SetType, step, Bs...>, step>;
//};
//
//// add set into set if not exists
//template <template <typename...> typename SetType, typename... As, typename... Bs> static constexpr auto push_back_into_set(SetType<As...>, SetType<Bs...>) -> typename set_merge_helper<SetType, SetType<As...>, Bs...>::type { return pcre_context{{};), subject.parameters}}
//
//template <template <typename...> typename SetType, typename... As> static constexpr auto push_back_into_set(SetType<As...>, SetType<>) -> SetType<As...> { return pcre_context{{};), subject.parameters}}

// END OF UTILITY

// set_start
template <auto V, typename A,typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_start, ctll::term<V>, pcre_context<ctll::list<A,Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(set<A>(), ctll::list<Ts...>()), subject.parameters};
}
// set_make
template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_make, ctll::term<V>, pcre_context<ctll::list<set<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(set<Content...>(), ctll::list<Ts...>()), subject.parameters};
}
// set_make_negative
template <auto V, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_make_negative, ctll::term<V>, pcre_context<ctll::list<set<Content...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(negative_set<Content...>(), ctll::list<Ts...>()), subject.parameters};
}
// set{A...} + B = set{A,B}
template <auto V, typename A, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<A,set<Content...>,Ts...>, Parameters> subject) {
	auto new_set = push_back_into_set<set>(A(), set<Content...>());
	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
}
// TODO checkme
//// set{A...} + set{B...} = set{A...,B...}
//template <auto V, typename... As, typename... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<set<As...>,set<Bs...>,Ts...>, Parameters> subject) {
//	auto new_set = push_back_into_set<set>(set<As...>(), set<Bs...>());
//	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
//}

// negative_set{A...} + B = negative_set{A,B}
template <auto V, typename A, typename... Content, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<A,negative_set<Content...>,Ts...>, Parameters> subject) {
	auto new_set = push_back_into_set<set>(A(), set<Content...>());
	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
}
// TODO checkme
//// negative_set{A...} + negative_set{B...} = negative_set{A...,B...}
//template <auto V, typename... As, typename... Bs, typename... Ts, typename Parameters> static constexpr auto apply(pcre::set_combine, ctll::term<V>, pcre_context<ctll::list<negative_set<As...>,negative_set<Bs...>,Ts...>, Parameters> subject) {
//	auto new_set = push_back_into_set<negative_set>(negative_set<As...>(), negative_set<Bs...>());
//	return pcre_context{ctll::push_front(new_set, ctll::list<Ts...>()), subject.parameters};
//}
// negate_class_named: [[^:digit:]] = [^[:digit:]]
template <auto V, typename A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::negate_class_named, ctll::term<V>, pcre_context<ctll::list<A, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(negate<A>(), ctll::list<Ts...>()), subject.parameters};
}

// add range to set
template <auto V, auto B, auto A, typename... Ts, typename Parameters> static constexpr auto apply(pcre::make_range, ctll::term<V>, pcre_context<ctll::list<character<B>,character<A>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(char_range<A,B>(), ctll::list<Ts...>()), subject.parameters};
}

#endif
	

};

}

#endif

#ifndef CTRE__EVALUATION__HPP
#define CTRE__EVALUATION__HPP

#ifndef CTRE__RETURN_TYPE__HPP
#define CTRE__RETURN_TYPE__HPP

#include <type_traits>
#include <tuple>
#include <string_view>
#include <string>

namespace ctre {
	
struct not_matched_tag_t { };

static constexpr inline auto not_matched = not_matched_tag_t{};
	
template <size_t Id, typename Name = void> struct captured_content {
	template <typename Iterator> class storage {
		Iterator _begin{};
		Iterator _end{};
		
		bool _matched{false};
	public:
		using char_type = typename std::iterator_traits<Iterator>::value_type;
		
		using name = Name;
	
		constexpr CTRE_FORCE_INLINE storage() noexcept {}
	
		constexpr CTRE_FORCE_INLINE void matched() noexcept {
			_matched = true;
		}
		constexpr CTRE_FORCE_INLINE void unmatch() noexcept {
			_matched = false;
		}
		constexpr CTRE_FORCE_INLINE void set_start(Iterator pos) noexcept {
			_begin = pos;
		}
		constexpr CTRE_FORCE_INLINE storage & set_end(Iterator pos) noexcept {
			_end = pos;
			return *this;
		}
		constexpr CTRE_FORCE_INLINE Iterator get_end() const noexcept {
			return _end;
		}
		
	
		constexpr auto begin() const noexcept {
			return _begin;
		}
		constexpr auto end() const noexcept {
			return _end;
		}
	
		constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
			return _matched;
		}

		constexpr CTRE_FORCE_INLINE auto size() const noexcept {
			return static_cast<size_t>(std::distance(_begin, _end));
		}

		constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
			return std::basic_string_view<char_type>(&*_begin, static_cast<size_t>(std::distance(_begin, _end)));
		}
		
		constexpr CTRE_FORCE_INLINE auto to_string() const noexcept {
			return std::basic_string<char_type>(&*_begin, static_cast<size_t>(std::distance(_begin, _end)));
		}
		
		constexpr CTRE_FORCE_INLINE operator std::basic_string_view<char_type>() const noexcept {
			return to_view();
		}
		
		constexpr CTRE_FORCE_INLINE explicit operator std::basic_string<char_type>() const noexcept {
			return to_string();
		}
		
		constexpr CTRE_FORCE_INLINE static size_t get_id() noexcept {
			return Id;
		}
	};
};

struct capture_not_exists_tag { };

static constexpr inline auto capture_not_exists = capture_not_exists_tag{};

template <typename... Captures> struct captures;

template <typename Head, typename... Tail> struct captures<Head, Tail...>: captures<Tail...> {
	Head head{};
	constexpr CTRE_FORCE_INLINE captures() noexcept { }
	template <size_t id> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		if constexpr (id == Head::get_id()) {
			return true;
		} else {
			return captures<Tail...>::template exists<id>();
		}
	}
	template <typename Name> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		if constexpr (std::is_same_v<Name, typename Head::name>) {
			return true;
		} else {
			return captures<Tail...>::template exists<Name>();
		}
	}
	template <size_t id> CTRE_FORCE_INLINE constexpr auto & select() noexcept {
		if constexpr (id == Head::get_id()) {
			return head;
		} else {
			return captures<Tail...>::template select<id>();
		}
	}
	template <typename Name> CTRE_FORCE_INLINE constexpr auto & select() noexcept {
		if constexpr (std::is_same_v<Name, typename Head::name>) {
			return head;
		} else {
			return captures<Tail...>::template select<Name>();
		}
	}
	template <size_t id> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		if constexpr (id == Head::get_id()) {
			return head;
		} else {
			return captures<Tail...>::template select<id>();
		}
	}
	template <typename Name> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		if constexpr (std::is_same_v<Name, typename Head::name>) {
			return head;
		} else {
			return captures<Tail...>::template select<Name>();
		}
	}
};

template <> struct captures<> {
	constexpr CTRE_FORCE_INLINE captures() noexcept { }
	template <size_t> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		return false;
	}
	template <typename> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		return false;
	}
	template <size_t> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		return capture_not_exists;
	}
	template <typename> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		return capture_not_exists;
	}
};

template <typename Iterator, typename... Captures> class regex_results {
	captures<captured_content<0>::template storage<Iterator>, typename Captures::template storage<Iterator>...> _captures{};
public:
	using char_type = typename std::iterator_traits<Iterator>::value_type;
	
	constexpr CTRE_FORCE_INLINE regex_results() noexcept { }
	constexpr CTRE_FORCE_INLINE regex_results(not_matched_tag_t) noexcept { }
	
	// special constructor for deducting
	constexpr CTRE_FORCE_INLINE regex_results(Iterator, ctll::list<Captures...>) noexcept { }
	
	template <size_t Id, typename = std::enable_if_t<decltype(_captures)::template exists<Id>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		return _captures.template select<Id>();
	}
	template <typename Name, typename = std::enable_if_t<decltype(_captures)::template exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		return _captures.template select<Name>();
	}
	static constexpr size_t size() noexcept {
		return sizeof...(Captures) + 1;
	}
	constexpr CTRE_FORCE_INLINE regex_results & matched() noexcept {
		_captures.template select<0>().matched();
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_results & unmatch() noexcept {
		_captures.template select<0>().unmatch();
		return *this;
	}
	constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
		return bool(_captures.template select<0>());
	}
	
	constexpr CTRE_FORCE_INLINE operator std::basic_string_view<char_type>() const noexcept {
		return to_view();
	}
	
	constexpr CTRE_FORCE_INLINE explicit operator std::basic_string<char_type>() const noexcept {
		return to_string();
	}
	
	constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
		return _captures.template select<0>().to_view();
	}
	
	constexpr CTRE_FORCE_INLINE auto to_string() const noexcept {
		return _captures.template select<0>().to_string();
	}
	
	constexpr CTRE_FORCE_INLINE regex_results & set_start_mark(Iterator pos) noexcept {
		_captures.template select<0>().set_start(pos);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_results & set_end_mark(Iterator pos) noexcept {
		_captures.template select<0>().set_end(pos);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE Iterator get_end_position() const noexcept {
		return _captures.template select<0>().get_end();
	}
	template <size_t Id> CTRE_FORCE_INLINE constexpr regex_results & start_capture(Iterator pos) noexcept {
		_captures.template select<Id>().set_start(pos);
		return *this;
	}
	template <size_t Id> CTRE_FORCE_INLINE constexpr regex_results & end_capture(Iterator pos) noexcept {
		_captures.template select<Id>().set_end(pos).matched();
		return *this;
	}
};

template <typename Iterator, typename... Captures> regex_results(Iterator, ctll::list<Captures...>) -> regex_results<Iterator, Captures...>;

}

// support for structured bindings

namespace std {
	template <typename... Captures> class tuple_size<ctre::regex_results<Captures...>> : public std::integral_constant<size_t, ctre::regex_results<Captures...>::size()> { };
	
	template <size_t N, typename... Captures> class tuple_element<N, ctre::regex_results<Captures...>> {
	public:
		using type = decltype(
			std::declval<const ctre::regex_results<Captures...> &>().template get<N>()
		);
	};
}

#endif

#ifndef CTRE__FIND_CAPTURES__HPP
#define CTRE__FIND_CAPTURES__HPP

namespace ctre {

template <typename Pattern> constexpr auto find_captures(Pattern) noexcept {
	return find_captures(ctll::list<Pattern>(), ctll::list<>());
}

template <typename... Output> constexpr auto find_captures(ctll::list<>, ctll::list<Output...> output) noexcept {
	return output;
}

template <auto... String, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<string<String...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}

template <typename... Options, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<select<Options...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Options..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<optional<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_optional<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<sequence<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Tail, typename Output> constexpr auto find_captures(ctll::list<empty, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}

template <typename... Tail, typename Output> constexpr auto find_captures(ctll::list<assert_begin, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}

template <typename... Tail, typename Output> constexpr auto find_captures(ctll::list<assert_end, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}

// , typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<char>)
template <typename CharacterLike, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<CharacterLike, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<plus<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<star<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <size_t A, size_t B, typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<repeat<A,B,Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_plus<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_star<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <size_t A, size_t B, typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_repeat<A,B,Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<possessive_plus<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<possessive_star<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <size_t A, size_t B, typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<possessive_repeat<A,B,Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lookahead_positive<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lookahead_negative<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <size_t Id, typename... Content, typename... Tail, typename... Output> constexpr auto find_captures(ctll::list<capture<Id,Content...>, Tail...>, ctll::list<Output...>) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), ctll::list<Output..., captured_content<Id>>());
}

template <size_t Id, typename Name, typename... Content, typename... Tail, typename... Output> constexpr auto find_captures(ctll::list<capture_with_name<Id,Name,Content...>, Tail...>, ctll::list<Output...>) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), ctll::list<Output..., captured_content<Id, Name>>());
}

}

#endif

namespace ctre {

// calling with pattern prepare stack and triplet of iterators
template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto match_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	return evaluate(begin, begin, end, return_type{}, ctll::list<start_mark, Pattern, assert_end, end_mark, accept>());
}

template <typename Iterator, typename EndIterator, typename Pattern> 
constexpr inline auto search_re(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
	using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
	
	auto it = begin;
	for (; end != it; ++it) {
		if (auto out = evaluate(begin, it, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>())) {
			return out;
		}
	}
	
	// in case the RE is empty
	return evaluate(begin, it, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>());
}

// if we found "accept" object on stack => ACCEPT
template <typename R, typename Iterator, typename EndIterator> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R captures, ctll::list<accept>) noexcept {
	return captures.matched();
}

// mark start of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<start_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_start_mark(current), ctll::list<Tail...>());
}

// mark end of outer capture
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<end_mark, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.set_end_mark(current), ctll::list<Tail...>());
}

// mark end of cycle
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator current, const EndIterator, R captures, ctll::list<end_cycle_mark>) noexcept {
	return captures.set_end_mark(current).matched();
}

// matching everything which behave as a one character matcher

template <typename R, typename Iterator, typename EndIterator, typename CharacterLike, typename... Tail, typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<decltype(*std::declval<Iterator>())>)>> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<CharacterLike, Tail...>) noexcept {
	if (end == current) return not_matched;
	if (!CharacterLike::match_char(*current)) return not_matched;
	return evaluate(begin, current+1, end, captures, ctll::list<Tail...>());
}

// matching strings in patterns

template <typename Iterator> struct string_match_result {
	Iterator current;
	bool match;
};

template <auto Head, auto... String, typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> evaluate_match_string(Iterator current, const EndIterator end) noexcept {
	if ((end != current) && (Head == *current)) {
		if constexpr (sizeof...(String) > 0) {
			return evaluate_match_string<String...>(++current, end);
		} else {
			return {++current, true};
		}
	} else {
		return {++current, false}; // not needed but will optimize
	}
}

template <typename R, typename Iterator, typename EndIterator, auto... String, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<string<String...>, Tail...>) noexcept {
	if constexpr (sizeof...(String) == 0) {
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	} else if (auto tmp = evaluate_match_string<String...>(current, end); tmp.match) {
		return evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// matching select in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadOptions, typename... TailOptions, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<select<HeadOptions, TailOptions...>, Tail...>) noexcept {
	if (auto r = evaluate(begin, current, end, captures, ctll::list<HeadOptions, Tail...>())) {
		return r;
	} else {
		return evaluate(begin, current, end, captures, ctll::list<select<TailOptions...>, Tail...>());
	}
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R, ctll::list<select<>, Tail...>) noexcept {
	// no previous option was matched => REJECT
	return not_matched;
}

// matching optional in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<optional<Content...>, Tail...>) noexcept {
	if (auto r1 = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return r2;
	} else {
		return not_matched;
	}
}

// lazy optional
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_optional<Content...>, Tail...>) noexcept {
	if (auto r1 = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return r1;
	} else if (auto r2 = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, Tail...>())) {
		return r2;
	} else {
		return not_matched;
	}
}

// matching sequence in patterns
template <typename R, typename Iterator, typename EndIterator, typename HeadContent, typename... TailContent, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<sequence<HeadContent, TailContent...>, Tail...>) noexcept {
	if constexpr (sizeof...(TailContent) > 0) {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, sequence<TailContent...>, Tail...>());
	} else {
		return evaluate(begin, current, end, captures, ctll::list<HeadContent, Tail...>());
	}
}

// matching empty in patterns
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<empty, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// matching asserts
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<assert_begin, Tail...>) noexcept {
	if (begin != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<assert_end, Tail...>) noexcept {
	if (end != current) {
		return not_matched;
	}
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// lazy repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_repeat<A,B,Content...>, Tail...>) noexcept {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto outer_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			captures = outer_result.unmatch();
			current = outer_result.get_end_position();
		} else {
			return not_matched;
		}
	}
	
	if (auto outer_result = evaluate(begin, current, end, captures, ctll::list<Tail...>())) {
		return outer_result;
	} else {
		for (; (i < B) || (B == 0); ++i) {
			if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
				if (auto outer_result = evaluate(begin, inner_result.get_end_position(), end, inner_result.unmatch(), ctll::list<Tail...>())) {
					return outer_result;
				} else {
					captures = inner_result.unmatch();
					current = inner_result.get_end_position();
					continue;
				}
			} else {
				return not_matched;
			}
		}
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	}
}

// possessive repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_repeat<A,B,Content...>, Tail...>) noexcept {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		} else {
			return not_matched;
		}
	}
	
	for (; (i < B) || (B == 0); ++i) {
		// try as many of inner as possible and then try outer once
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			current = inner_result.get_end_position();
		} else {
			return evaluate(begin, current, end, captures, ctll::list<Tail...>());
		}
	}
	
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}

// (gready) repeat
template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr inline R evaluate_recursive(size_t i, const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	if ((i < B) || (B == 0)) {
		 
		// a*ab
		// aab
		
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			// TODO MSVC issue:
			// if I uncomment this return it will not fail in constexpr (but the matching result will not be correct)
			//  return inner_result
			// I tried to add all constructors to R but without any success 
			if (auto rec_result = evaluate_recursive(i+1, begin, inner_result.get_end_position(), end, inner_result.unmatch(), stack)) {
				return rec_result;
			}
		}
	} 
	return evaluate(begin, current, end, captures, ctll::list<Tail...>());
}	

template <typename R, typename Iterator, typename EndIterator, size_t A, size_t B, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<repeat<A,B,Content...>, Tail...> stack) {
	// A..B
	size_t i{0};
	for (; i < A && (A != 0); ++i) {
		if (auto inner_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_cycle_mark>())) {
			captures = inner_result.unmatch();
			current = inner_result.get_end_position();
		} else {
			return not_matched;
		}
	}
	
	return evaluate_recursive(i, begin, current, end, captures, stack);
}

// repeat lazy_star
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<0,0,Content...>, Tail...>());
}

// repeat (lazy_plus)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lazy_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<lazy_repeat<1,0,Content...>, Tail...>());
}

// repeat (possessive_star)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<0,0,Content...>, Tail...>());
}

// repeat (possessive_plus)
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<possessive_plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<possessive_repeat<1,0,Content...>, Tail...>());
}

// repeat (greedy) star
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<star<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<0,0,Content...>, Tail...>());
}

// repeat (greedy) plus
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<plus<Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures, ctll::list<repeat<1,0,Content...>, Tail...>());
}

// capture (numeric ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture<Id, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// capture end mark (numeric and string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<numeric_mark<Id>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template end_capture<Id>(current), ctll::list<Tail...>());
}

// capture (string ID)
template <typename R, typename Iterator, typename EndIterator, size_t Id, typename Name, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
	return evaluate(begin, current, end, captures.template start_capture<Id>(current), ctll::list<sequence<Content...>, numeric_mark<Id>, Tail...>());
}

// backreference support (match agains content of iterators)
template <typename Iterator, typename EndIterator> constexpr CTRE_FORCE_INLINE string_match_result<Iterator> match_against_range(Iterator current, const EndIterator end, Iterator range_current, const Iterator range_end) noexcept {
	while (current != end && range_current != range_end) {
		if (*current == *range_current) {
			current++;
			range_current++;
		} else {
			return {current, false};
		}
	}
	return {current, range_current == range_end};
}

// backreference with name
template <typename R, typename Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<back_reference_with_name<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto tmp = match_against_range(current, end, ref.begin(), ref.end()); tmp.match) {
			return evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

// backreference
template <typename R, size_t Id, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<back_reference<Id>, Tail...>) noexcept {
	
	if (const auto ref = captures.template get<Id>()) {
		if (auto tmp = match_against_range(current, end, ref.begin(), ref.end()); tmp.match) {
			return evaluate(begin, tmp.current, end, captures, ctll::list<Tail...>());
		}
	}
	return not_matched;
}

// end of lookahead
template <typename R, typename Iterator, typename EndIterator, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator, Iterator, const EndIterator, R captures, ctll::list<end_lookahead_mark>) noexcept {
	return captures.matched();
}

// lookahead positive
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lookahead_positive<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		captures = lookahead_result.unmatch();
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	} else {
		return not_matched;
	}
}

// lookahead negative
template <typename R, typename Iterator, typename EndIterator, typename... Content, typename... Tail> 
constexpr CTRE_FORCE_INLINE R evaluate(const Iterator begin, Iterator current, const EndIterator end, R captures, ctll::list<lookahead_negative<Content...>, Tail...>) noexcept {
	
	if (auto lookahead_result = evaluate(begin, current, end, captures, ctll::list<sequence<Content...>, end_lookahead_mark>())) {
		return not_matched;
	} else {
		return evaluate(begin, current, end, captures, ctll::list<Tail...>());
	}
}

}

#endif

#ifndef CTRE__WRAPPER__HPP
#define CTRE__WRAPPER__HPP

#include <string_view>
#include <string>

namespace ctre {
	
struct zero_terminated_string_end_iterator {
	constexpr inline zero_terminated_string_end_iterator() = default;
	constexpr CTRE_FORCE_INLINE bool operator==(const char * ptr) const noexcept {
		return *ptr == '\0';
	} 
	constexpr CTRE_FORCE_INLINE bool operator==(const wchar_t * ptr) const noexcept {
		return *ptr == 0;
	} 
	constexpr CTRE_FORCE_INLINE bool operator!=(const char * ptr) const noexcept {
		return *ptr != '\0';
	} 
	constexpr CTRE_FORCE_INLINE bool operator!=(const wchar_t * ptr) const noexcept {
		return *ptr != 0;
	} 
};

template <typename RE> struct regular_expression {
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto match_2(IteratorBegin begin, IteratorEnd end) noexcept {
		return match_re(begin, end, RE());
	}
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto search_2(IteratorBegin begin, IteratorEnd end) noexcept {
		return search_re(begin, end, RE());
	}
	constexpr CTRE_FORCE_INLINE regular_expression() noexcept { };
	constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept { };
	template <typename Iterator> constexpr CTRE_FORCE_INLINE static auto match(Iterator begin, Iterator end) noexcept {
		return match_re(begin, end, RE());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const char * s) noexcept {
		return match_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const wchar_t * s) noexcept {
		return match_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const std::string & s) noexcept {
		return match_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const std::wstring & s) noexcept {
		return match_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::wstring_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::u16string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::u32string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	template <typename Iterator> constexpr CTRE_FORCE_INLINE static auto search(Iterator begin, Iterator end) noexcept {
		return search_re(begin, end, RE());
	}
	constexpr CTRE_FORCE_INLINE static auto search(const char * s) noexcept {
		return search_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(const wchar_t * s) noexcept {
		return search_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(const std::string & s) noexcept {
		return search_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(const std::wstring & s) noexcept {
		return search_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(std::string_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto search(std::wstring_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto search(std::u16string_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto search(std::u32string_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
};

template <typename RE> regular_expression(RE) -> regular_expression<RE>;

}

#endif

namespace ctre {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into ctll::basic_fixed_string
// in C++20 we have `class nontype template parameters`

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... input> static inline constexpr auto _fixed_string_reference = ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});
#endif	

namespace literals {
	
// clang and GCC <9 supports LITERALS with packs

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#define CTRE_ENABLE_LITERALS
#endif

#ifdef __INTEL_COMPILER
// not enable literals
#elif defined __GNUC__
#if not(__GNUC__ == 9 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ == 0)
#define CTRE_ENABLE_LITERALS
#endif
#endif

#ifdef CTRE_ENABLE_LITERALS
	
// add this when we will have concepts
// requires ctll::parser<ctre::pcre, _fixed_string_reference<CharT, charpack...>, ctre::pcre_actions>::template correct_with<pcre_context<>>

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}

// this will need to be fixed with C++20
#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_id() noexcept {
	return id<charpack...>();
}
#endif

#endif

}

namespace test_literals {
	
#ifdef CTRE_ENABLE_LITERALS

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr inline auto operator""_ctre_test() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr inline auto operator""_ctre_test() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	return ctll::parser<ctre::pcre, _input>::template correct_with<>;
}

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr inline auto operator""_ctre_gen() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr inline auto operator""_ctre_gen() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type::stack_type();
}

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_syntax() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_syntax() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	return ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template correct_with<pcre_context<>>;
}

#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // literals

} // ctre

#endif

#ifndef CTRE_V2__CTRE__FUNCTIONS__HPP
#define CTRE_V2__CTRE__FUNCTIONS__HPP

namespace ctre {

#if !__cpp_nontype_template_parameter_class
// avoiding CTAD limitation in C++17
template <typename CharT, size_t N> class pattern: public ctll::basic_fixed_string<CharT, N> {
	using parent = ctll::basic_fixed_string<CharT, N>;
public:
	constexpr pattern(const CharT (&input)[N]) noexcept: parent(input) { }
};

template <typename CharT, size_t N> pattern(const CharT (&)[N]) -> pattern<CharT, N>;

// for better examples
template <typename CharT, size_t N> class fixed_string: public ctll::basic_fixed_string<CharT, N> {
	using parent = ctll::basic_fixed_string<CharT, N>;
public:
	constexpr fixed_string(const CharT (&input)[N]) noexcept: parent(input) { }
};

template <typename CharT, size_t N> fixed_string(const CharT (&)[N]) -> fixed_string<CharT, N>;
#endif

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {
constexpr auto _input = input; // workaround for GCC 9 bug 88092
#else
template <auto & input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {	
constexpr auto & _input = input; 
#endif
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}

// in moment when we get C++20 support this will start to work :)

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(Args && ... args) noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression<re>(re());
	return re_obj.match(std::forward<Args>(args)...);
}
#else
template <auto & input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(Args && ... args) noexcept {
	constexpr auto & _input = input; 
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression<re>(re());
	return re_obj.match(std::forward<Args>(args)...);
}
#endif

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(Args && ... args) noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(std::forward<Args>(args)...);
}
#else
template <auto & input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(Args && ... args) noexcept {
	constexpr auto & _input = input; 
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(std::forward<Args>(args)...);
}
#endif

}

#endif

#ifndef CTRE_V2__CTRE__ITERATOR__HPP
#define CTRE_V2__CTRE__ITERATOR__HPP

namespace ctre {

struct regex_end_iterator {
	constexpr regex_end_iterator() noexcept { }
};

template <typename BeginIterator, typename EndIterator, typename RE> struct regex_iterator {
	BeginIterator current;
	const EndIterator end;
	decltype(RE::search_2(std::declval<BeginIterator>(), std::declval<EndIterator>())) current_match;

	constexpr regex_iterator(BeginIterator begin, EndIterator end) noexcept: current{begin}, end{end}, current_match{RE::search_2(current, end)} {
		if (current_match) {
			current = current_match.template get<0>().end();
		}
	}
	constexpr const auto & operator*() const noexcept {
		return current_match;
	}
	constexpr regex_iterator & operator++() noexcept {
		current_match = RE::search_2(current, end);
		if (current_match) {
			current = current_match.template get<0>().end();
		}
		return *this;
	}
	constexpr regex_iterator operator++(int) noexcept {
		auto previous = *this;
		current_match = RE::search_2(current, end);
		if (current_match) {
			current = current_match.template get<0>().end();
		}
		return previous;
	}
};

template <typename BeginIterator, typename EndIterator, typename RE> constexpr bool operator!=(const regex_iterator<BeginIterator, EndIterator, RE> & left, regex_end_iterator) {
	return bool(left.current_match);
}

template <typename BeginIterator, typename EndIterator, typename RE> constexpr bool operator!=(regex_end_iterator, const regex_iterator<BeginIterator, EndIterator, RE> & right) {
	return bool(right.current_match);
}

template <typename BeginIterator, typename EndIterator, typename RE> constexpr auto iterator(BeginIterator begin, EndIterator end, RE) noexcept {
	return regex_iterator<BeginIterator, EndIterator, RE>(begin, end);
}

constexpr auto iterator() noexcept {
	return regex_end_iterator{};
}

template <typename Subject, typename RE> constexpr auto iterator(const Subject & subject, RE re) noexcept {
	return iterator(subject.begin(), subject.end(), re);
}

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename BeginIterator, typename EndIterator> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto iterator(BeginIterator begin, EndIterator end) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return iterator(begin, end, re());
}
#endif

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename Subject> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto iterator(const Subject & subject) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return iterator(subject.begin(), subject.end(), re());
}
#endif

} // ctre

#endif

#ifndef CTRE_V2__CTRE__RANGE__HPP
#define CTRE_V2__CTRE__RANGE__HPP

namespace ctre {

template <typename BeginIterator, typename EndIterator, typename RE> struct regex_range {
	BeginIterator _begin;
	const EndIterator _end;
	constexpr regex_range(BeginIterator begin, EndIterator end) noexcept: _begin{begin}, _end{end} { }
	
	constexpr auto begin() const noexcept {
		return regex_iterator<BeginIterator, EndIterator, RE>(_begin, _end);
	}
	constexpr auto end() const noexcept {
		return regex_end_iterator{};
	}
};

template <typename BeginIterator, typename EndIterator, typename RE> constexpr auto range(BeginIterator begin, EndIterator end, RE) noexcept {
	return regex_range<BeginIterator, EndIterator, RE>(begin, end);
}

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename BeginIterator, typename EndIterator> constexpr auto range(BeginIterator begin, EndIterator end) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return range(begin, end, re());
}
#endif

template <typename Subject, typename RE> constexpr auto range(const Subject & subject, RE re) noexcept {
	return range(subject.begin(), subject.end(), re);
}

template <typename RE> constexpr auto range(const char * subject, RE re) noexcept {
	return range(subject, zero_terminated_string_end_iterator(), re);
}

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename Subject> constexpr auto range(const Subject & subject) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return range(subject.begin(), subject.end(), re());
}
#endif

}

#endif

#ifndef CTRE_V2__CTRE__OPERATORS__HPP
#define CTRE_V2__CTRE__OPERATORS__HPP

template <typename A, typename B> constexpr auto operator|(ctre::regular_expression<A>, ctre::regular_expression<B>) -> ctre::regular_expression<ctre::select<A,B>> {
	return {};
}

template <typename A, typename B> constexpr auto operator>>(ctre::regular_expression<A>, ctre::regular_expression<B>) -> ctre::regular_expression<ctre::sequence<A,B>> {
	return {};
}

#endif

#endif
