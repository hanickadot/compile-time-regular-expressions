#include <utility>
#include <iostream>

namespace Syntax {
	
template <typename... T> struct Stack;

// Some special symbols and/or actions
struct EmptyStackSymbol { };
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
	template <bool doActions, typename... Args> static constexpr bool parse(Args && ... args) {
		return Parser<CurrentStack, s...>::template parse<doActions>(std::forward<Args>(args)...);
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
	
	template <bool doActions, typename... T, typename... Args> static constexpr bool decide(Stack<T...> move, Args && ... args) {
		return Parser<decltype(CurrentStack::Pop::push(move)), s...>::template parse<doActions>(std::forward<Args>(args)...);
	}
	template <bool doActions, typename... Args> static constexpr bool decide(Epsilon move, Args && ... args) {
		return Parser<decltype(CurrentStack::Pop::push(move)), s...>::template parse<doActions>(std::forward<Args>(args)...);
	}
	template <bool, typename... Args> static constexpr bool decide(RejectInput, Args && ...) {
		return false;
	}
	template <bool, typename... Args> static constexpr bool decide(AcceptInput, Args && ...) {
		return true;
	}
	template <bool doActions, typename... Args> static constexpr bool decide(ReadChar, Args && ... args) {
		return MoveInput<typename CurrentStack::Pop, s...>::template parse<doActions>(std::forward<Args>(args)...);
	}
	
	friend class ParserActionDecider<haveAction, CurrentStack, s...>;
public:
	template <bool doActions, typename... Args> static constexpr bool parse(Args && ... args) {
		return ParserActionDecider<haveAction, CurrentStack, s...>::template parse<doActions>(std::forward<Args>(args)...);
	}
};

template <typename CurrentStack, char... s> struct ParserActionDecider<false,CurrentStack,s...> {
	template <bool doActions, typename... Args> static constexpr bool parse(Args && ... args) {
		return Parser<CurrentStack,s...>::template decide<doActions>(typename Parser<CurrentStack,s...>::Move::N{}, std::forward<Args>(args)...);
	}
};

template <typename CurrentStack, char... s> struct ParserActionDecider<true,CurrentStack,s...> {
	template <bool doActions, typename... Args> static constexpr bool parse(Args && ... args) {
		if (doActions) CurrentStack::Top::action();
		return Parser<CurrentStack,s...>::template decide<doActions>(Epsilon{}, std::forward<Args>(args)...);
	}
};
	
}

