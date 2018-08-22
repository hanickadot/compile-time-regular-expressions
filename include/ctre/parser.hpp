#ifndef CTRE_V2__CTRE__PARSER__HPP
#define CTRE_V2__CTRE__PARSER__HPP

#include "type-stack.hpp"

#define RULE static constexpr auto rule
#define NONTERM(name) struct name { }
#define START_NONTERM(name) struct name { }; using _start = name
#define SUBJECT_TYPE(name) using _subject_type = name

namespace ctre {

// basic formal language constructs

template <auto v> struct term {
	static constexpr auto value = v;
};

struct accept { constexpr explicit operator bool() noexcept { return true; } };

struct reject { constexpr explicit operator bool() noexcept { return false; } };

struct action { 
	struct action_tag { };
};

template <auto A, decltype(A) B> struct range {
	constexpr inline range() noexcept { };
	//template <auto V> constexpr range(term<V>) noexcept requires (A <= V) && (V <= B);
	template <auto V, typename = std::enable_if_t<(A <= V) && (V <= B)>> constexpr inline range(term<V>) noexcept;
};

template <auto... Def> struct set {
	constexpr inline set() noexcept { };
	//template <auto V> constexpr set(term<V>) noexcept requires ((Def == V) || ... || false);
	template <auto V, typename = std::enable_if_t<((Def == V) || ... || false)>> constexpr inline set(term<V>) noexcept;
};


template <auto... Def> struct neg_set {
	constexpr inline neg_set() noexcept { };
	//template <auto V> constexpr set(term<V>) noexcept requires ((Def == V) || ... || false);
	template <auto V, typename = std::enable_if_t<((Def != V) && ... && true)>> constexpr inline neg_set(term<V>) noexcept;
};

struct anything {
	constexpr inline anything() noexcept { };
	template <auto V> constexpr anything(term<V>) noexcept;
};

template <typename T> struct IsExplicitlyConvertibleToBool {
	template <typename Y> static constexpr auto test(Y * y) -> decltype(bool(*y), std::true_type{});
	template <typename> static constexpr auto test(...) -> std::false_type;
	static constexpr const bool value = decltype(test<T>(nullptr))();
};

template <typename T> struct IsStepForward {
	template <typename Y> static constexpr auto test(Y *) -> decltype(Y::move_forward(), std::true_type{});
	template <typename> static constexpr auto test(...) -> std::false_type;
	static constexpr const bool value = decltype(test<T>(nullptr))();
};

template <typename T, typename Subject, typename Current> struct IsActionItemWithTerm {
	template <typename Y> static constexpr auto test(Y * y) -> decltype((*y)(Subject(), Current()), std::true_type{});
	template <typename> static constexpr auto test(...) -> std::false_type;
	static constexpr const bool value = decltype(test<T>(nullptr))();
};

template <typename T, typename Subject> struct IsActionItem {
	template <typename Y> static constexpr auto test(Y * y) -> decltype((*y)(Subject()), std::true_type{});
	template <typename> static constexpr auto test(...) -> std::false_type;
	static constexpr const bool value = decltype(test<T>(nullptr))();
};

template <typename T> struct IsAction {
	template <typename Y> static constexpr auto test(Y * y) -> decltype(std::declval<typename Y::action_tag>(), std::true_type{});
	template <typename> static constexpr auto test(...) -> std::false_type;
	static constexpr const bool value = decltype(test<T>(nullptr))();
};

// everything else can be used as a nonterminal

template <typename Grammar> struct augment_grammar: public Grammar {
	using typename Grammar::_start;
	using Grammar::rule; // Grammar rules should have same priority
	
	// default behaviour is reject if there is unexpected state
	static constexpr auto rule(...) -> reject;
	
	// if there are two same terms on top of the stack and current input, you should move forward
	template <auto A> static constexpr auto rule(term<A>, term<A>) -> pop_input;
	
	template <typename Expected, auto V> static constexpr auto rule(Expected, term<V>) -> std::enable_if_t<std::is_constructible_v<Expected, term<V>>, pop_input>;
	//template <auto A, auto B, auto V, typename = std::enable_if_t<((A <= V) && (V <= B))>> static constexpr auto rule(range<A,B>, term<V>) -> pop_input;
	//
	//template <auto... Def, auto V, typename = std::enable_if_t<((V == Def) || ... || false)>> static constexpr auto rule(set<Def...>, term<V>) -> pop_input;
	//
	//template <auto V> static constexpr auto rule(anything, term<V>) -> pop_input;
	//
	//template <auto... Def, auto V, typename = std::enable_if_t<((V != Def) && ... && true)>> static constexpr auto rule(neg_set<Def...>, term<V>) -> pop_input;
	
	
	// empty stack and empty input means we are accepting
	static constexpr auto rule(epsilon, epsilon) -> accept;
};

struct empty_subject { };

template <typename Subject, bool Value> struct parse_result {
	//static constexpr size_t steps{Steps};
	Subject subject{};
	constexpr __attribute__((always_inline)) parse_result(Subject subject) noexcept: subject{subject} { }
	constexpr __attribute__((always_inline)) operator bool() const noexcept {
		return Value;
	}
};

template <bool Value> struct parse_result<empty_subject, Value> {
	constexpr __attribute__((always_inline)) parse_result() noexcept { };
	constexpr __attribute__((always_inline)) parse_result(empty_subject) noexcept { };
	constexpr __attribute__((always_inline)) operator bool() const noexcept {
		return Value;
	}
};
	
//template <typename G, FixedString input> struct parser { // in c++20
template <typename G, const auto & input> struct parser {
	using grammar = augment_grammar<G>;
	static constexpr size_t size = input.size();
	template <size_t pos> static inline constexpr auto current() {
		if constexpr (pos < size) {
			return term<input[pos]>();
		} else {
			return epsilon();
		}
	}
	template <size_t pos> static inline constexpr auto previous() {
		if constexpr (pos == 0) {
			return term<input[0]>();
		} else if constexpr (pos <= size) {
			return term<input[pos-1]>();
		} else {
			return epsilon();
		}
	}
	template <typename A, typename B> static inline constexpr auto rule(A a,B b) {
		return decltype(grammar().rule(a,b))();
	}
	template <size_t pos = 0, typename Head> static inline constexpr auto get_move(Head head) {
		return rule(Head(), current<pos>());
	}
	template <typename Subject = typename G::_subject_type> static inline constexpr auto decide(const Subject subject = Subject{}) {
		return decide(list<typename G::_start>(), 1, subject);
	}
	template <size_t pos = 0, typename Stack, typename Subject = empty_subject> static inline constexpr auto decide(Stack stack, unsigned step, Subject subject) {
		auto head_of_stack = head(stack);
		
		if constexpr (IsAction<decltype(head_of_stack)>::value) {
			// item on top of the stack is action modifiing subject
			// if it's applicable call operator() and modify subject accordingly
			if constexpr (IsActionItemWithTerm<decltype(head_of_stack), Subject, decltype(previous<pos>())>::value) {
				// modify the subject via action.operator(subject) -> new subject
				return decide<pos>(pop(stack), step+1, head_of_stack(subject, previous<pos>()));
			} else if constexpr (IsActionItem<decltype(head_of_stack), Subject>::value) {
				// modify the subject via action.operator(subject, previous) -> new subject
				return decide<pos>(pop(stack), step+1, head_of_stack(subject));
			} else if constexpr (std::is_same_v<Subject, empty_subject>) {
				// in case that action can't be called and subject is empty
			 	return decide<pos>(pop(stack), step+1, subject);
			} else {
				// else reject input
				return parse_result<Subject,false>(subject);
			}
		} else {
			// "else" branch because return type deduction is not working with standalone "if constexpr"
			auto m = get_move<pos>(head_of_stack);
			if constexpr (is_quick(current<pos>(), m)) {
				// quick mean = push<TERM_OR_RANGE, ...>
				// only nonaccepting/rejecting states are quick :)
				return decide<pos+1>(pop_and_push_quick(m, stack), step+1, subject);
			} else if constexpr (IsExplicitlyConvertibleToBool<decltype(m)>::value) {
				// accept or reject state
				return parse_result<Subject,bool(m)>(subject);
			} else {
				// move forward with parsing
				// if decltype(m) is pop_char then move a char forward
				return decide<pos+IsStepForward<decltype(m)>::value>(pop_and_push(m, stack), step+1, subject);
			}
		}
	}
};

}

#endif
