#include <utility>
#include <iostream>

namespace Syntax {
	
template <typename... T> struct Stack;

// Empty symbol
struct Symbol {
//	template <typename... T> static void action(T...) {}
};

// Some special symbols and/or actions
struct EmptyStackSymbol: Symbol { };
struct AcceptInput { };
struct RejectInput { };
struct ReadChar { };

using Epsilon = Stack<>;
template <typename... T> using String = Stack<T...>;

// Syntax movements
template <typename TOS, char... f> struct Move { using N = RejectInput; };

// Empty stack + empty input = ACCEPT
template <> struct Move<EmptyStackSymbol> { using N = AcceptInput; };

// Stack
template <typename First, typename... T> struct Stack<First, T...> {
	static constexpr const unsigned int deep = sizeof...(T) + 1;
	static constexpr const bool empty = false;
	using Pop = Stack<T...>;
	using Top = First;
	
	template <typename... Params> static auto push(Stack<Params...>) {
		return Stack<Params..., First, T...>{};
	}
	template <typename... Params> static auto push(Params...) {
		return Stack<Params..., First, T...>{};
	}
	static auto push() {
		return Stack<First, T...>{};
	}
};

// Empty Stack
template <> struct Stack<> {
	static constexpr const unsigned int deep = 0;
	static constexpr const bool empty = true;
	using Pop = Stack<>;
	using Top = EmptyStackSymbol;
	
	template <typename... Params> static auto push(Stack<Params...>) {
		return Stack<Params...>{};
	}
	template <typename... Params> static auto push(Params...) {
		return Stack<Params...>{};
	}
	static auto push() {
		return Stack<>{};
	}
};

template <typename TOS, char... s> struct DecideMove;
template <typename TOS, char f, char... s> struct DecideMove<TOS, f, s...> { using Move = Move<TOS, f>; };
template <typename TOS> struct DecideMove<TOS> { using Move = Move<TOS>; };


template <typename T> class HaveAction {
	struct Fallback { int action; }; // add member name "X"
	struct Derived : T, Fallback { };

	template<typename U, U> struct Check;

	using ArrayOfOne = char[1];
	using ArrayOfTwo = char[2];

	template <typename U> static ArrayOfOne & func(Check<int Fallback::*, &U::action> *);
	template <typename U> static ArrayOfTwo & func(...);

public:
	static constexpr const bool value = sizeof(func<Derived>(0)) == 2;
};


template <typename CurrentStack, char... s> struct Parser;

template <typename CurrentStack, char f, char... s> struct MoveInput {
	static constexpr bool parse() {
		return Parser<CurrentStack, s...>::parse();
	}
};

template <char... s> struct FirstChar {
	static constexpr const char value = '\0';
};

template <char f, char... s> struct FirstChar<f,s...> {
	static constexpr const char value = f;
};

template <bool haveAction, typename CurrentStack, char... s> class ParserActionDecider { };

template <typename CurrentStack, char... s> class Parser {
protected:
	using Move = typename DecideMove<typename CurrentStack::Top, s...>::Move;
	static constexpr const bool haveAction = HaveAction<typename CurrentStack::Top>::value;
	template <typename... T> static constexpr bool decide(Stack<T...> move) {
		return Parser<decltype(CurrentStack::Pop::push(move)), s...>::parse();
	}
	static constexpr bool decide(Epsilon move) {
		return Parser<decltype(CurrentStack::Pop::push(move)), s...>::parse();
	}
	static constexpr bool decide(RejectInput) {
		return false;
	}
	template <typename... T> static constexpr bool decide() {
		return false;
	}
	static constexpr bool decide(AcceptInput) {
		return true;
	}
	static constexpr bool decide(ReadChar) {
		return MoveInput<typename CurrentStack::Pop, s...>::parse();
	}
	friend class ParserActionDecider<haveAction, CurrentStack, s...>;
public:
	static constexpr bool parse() {
		return ParserActionDecider<haveAction, CurrentStack, s...>::parse();
	}
};

template <typename CurrentStack, char... s> struct ParserActionDecider<false,CurrentStack,s...> {
	static constexpr bool parse() {
		return Parser<CurrentStack,s...>::decide(typename Parser<CurrentStack,s...>::Move::N{});
	}
};

template <typename CurrentStack, char... s> struct ParserActionDecider<true,CurrentStack,s...> {
	static constexpr bool parse() {
		CurrentStack::Top::action();
		return Parser<CurrentStack,s...>::decide(Epsilon{});
	}
};
	
}

