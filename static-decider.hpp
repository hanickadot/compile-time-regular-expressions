#ifndef STATIC_DECIDER_HPP
#define STATIC_DECIDER_HPP

#include "static-stack.hpp"

namespace Static {
	
struct AcceptInput { };
struct RejectInput { };
struct ReadChar { };

template <typename T> struct Action {
	static const char * identify() { return "Action<>"; }
	template <char current, typename Stack, typename ... Args> static constexpr void action(Stack & stack, Args && ... args) { T::template action<current>(stack, std::forward<Args>(args)...); }
};

#define SYMBOL(x) struct x { static const char * identify() { return #x; } }

template <typename Symbol, char...> struct Table {
	using Move = RejectInput;
};

template <> struct Table<EmptyStackSymbol> {
	using Move = AcceptInput;
};

template <typename T, char... string> struct Table<Action<T>, string...> {
	using Move = Epsilon;
};

template <typename T, char... string> struct Table<Holder<T>, string...> {
	using Move = Epsilon;
};

template <char ...> struct TapeHelper;

template <char firstCharacter, char ... rest> struct TapeHelper<firstCharacter, rest...> {
	static constexpr const size_t size = sizeof...(rest) + 1;
	static constexpr const char first = firstCharacter;
	static constexpr const bool empty = false;
	using Read = TapeHelper<rest...>;
};

template <> struct TapeHelper<> {
	static constexpr const size_t size = 0;
	static constexpr const char first = {};
	static constexpr const bool empty = true;
	using Read = TapeHelper<>;
};

template <bool c> struct Constant {
	static constexpr const bool correct = c;
	template <typename ... T> bool run(T && ...) {
		return c;
	}
};

template <typename Stack, typename TapeHelper> class Decider {
protected:
	using TopOfStack = typename Stack::Top;
	using CurrentCell = typename std::conditional<TapeHelper::empty, Table<TopOfStack>, Table<TopOfStack, TapeHelper::first>>::type;
	using CurrentMove = typename CurrentCell::Move;
public:		
	static constexpr const bool currentRejecting = std::is_same<CurrentMove, RejectInput>::value;
	static constexpr const bool currentAccepting = std::is_same<CurrentMove, AcceptInput>::value;
	static constexpr const bool currentReading = std::is_same<CurrentMove, ReadChar>::value;
protected:
	using NextDecider = typename std::conditional<currentRejecting, Constant<false>, typename std::conditional<currentAccepting, Constant<true>, typename std::conditional<currentReading, Decider<typename Stack::Pop, typename TapeHelper::Read>, Decider<typename Stack::Pop::template Push<CurrentMove>, TapeHelper>>::type>::type>::type;
public:
	static constexpr const bool correct = currentAccepting ? true : ( currentRejecting ? false : NextDecider::correct );
protected:
	Stack stack{};
public:
	constexpr Decider() = default;
	constexpr Decider(const Stack && orig): stack{orig} {}
protected:
	template <typename T, typename ... Args> constexpr void action(Holder<T> && holder, Args && ... args) { holder.action(stack, std::forward<Args>(args)...); }
	template <typename T, typename ... Args> constexpr void action(const Action<T> &&, Args && ... args) { Action<T>::template action<TapeHelper::first>(stack, std::forward<Args>(args)...); }
	template <typename ... T> constexpr void action(T ...) const { }
	
	template <typename ... T> constexpr bool innerRun(const RejectInput &&, T && ... args) {
		return false;
	}
	template <typename ... T> constexpr bool innerRun(const AcceptInput &&, T && ... args) {
		return false;
	}
	template <typename ... T> constexpr bool innerRun(const ReadChar &&, T && ... args) {
		return NextDecider{stack.pop()}.run(std::forward<T>(args)...);
	}
	template <typename N, typename ... T> constexpr bool innerRun(const N &&, T && ... args) {
		return NextDecider{stack.pop().push(CurrentMove{})}.run(std::forward<T>(args)...);
	}
public:
	template <typename ... T> constexpr bool run(T && ... args) {
		action(stack.top(), std::forward<T>(args)...);
		return innerRun(CurrentMove{}, args...);
	}
};

template <typename StartSymbol, char... string> class Parser {
public:
	using type = Decider<Stack<StartSymbol>, TapeHelper<string...>>;
	type parser{};
	static constexpr const bool correct = type::correct;
	template <typename ... T> constexpr bool run(T && ... args) {
		return parser.run(std::forward<T>(args)...);
	}
};


}

#endif
