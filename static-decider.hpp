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

template <typename T> struct Builder {
	template <char c, typename ... Content> static constexpr auto build(const Stack<Content...> && stack) {
		return T::template build<c>(std::forward<const Stack<Content...>>(stack));
	}
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

template <typename T, char... string> struct Table<Builder<T>, string...> {
	using Move = Epsilon;
};

template <char ...> struct Input;

template <char firstCharacter, char ... rest> struct Input<firstCharacter, rest...> {
	static constexpr const size_t size = sizeof...(rest) + 1;
	static constexpr const char first = firstCharacter;
	static constexpr const bool empty = false;
	using Read = Input<rest...>;
};

template <> struct Input<> {
	static constexpr const size_t size = 0;
	static constexpr const char first = {};
	static constexpr const bool empty = true;
	using Read = Input<>;
};

template <bool c> struct Constant {
	static constexpr const bool correct = c;
	template <typename ... T> bool run(T && ...) {
		return c;
	}
	template <typename ... Seed> using TypeStack = Stack<Seed...>;
};

template <typename Stack, typename Input> class Decider {
protected:
	using TopOfStack = typename Stack::Top;
	using CurrentCell = typename std::conditional<Input::empty, Table<TopOfStack>, Table<TopOfStack, Input::first>>::type;
	using CurrentMove = typename CurrentCell::Move;
public:		
	static constexpr const bool currentRejecting = std::is_same<CurrentMove, RejectInput>::value;
	static constexpr const bool currentAccepting = std::is_same<CurrentMove, AcceptInput>::value;
	static constexpr const bool currentReading = std::is_same<CurrentMove, ReadChar>::value;
protected:
	using NextDecider = typename std::conditional<currentRejecting, Constant<false>, typename std::conditional<currentAccepting, Constant<true>, typename std::conditional<currentReading, Decider<typename Stack::Pop, typename Input::Read>, Decider<typename Stack::Pop::template Push<CurrentMove>, Input>>::type>::type>::type;
public:
	static constexpr const bool correct = currentAccepting ? true : ( currentRejecting ? false : NextDecider::correct );
protected:
	Stack stack{};
	template <typename ... Seed> static constexpr auto getSeed(const Static::Stack<Seed...> &&) {
		return typename NextDecider::template TypeStack<Seed...>{};
	}
public:
	//template <typename ... Seed, typename ... Rest> static constexpr auto build(const Rest && ...) {
	//	return Stack<Seed...>{};
	//}
	//template <typename ... Seed, typename BuilderType> static constexpr auto build(const Builder<BuilderType> &&) {
	//	return Builder<BuilderType>::build<Seed...>();
	//}
	
	
	
	template <typename ... Seed, typename T> static constexpr auto build(const Builder<T> && builder) {
		return decltype(getSeed(Builder<T>::template build<Input::first>(Static::Stack<Seed...>{}))){};
	}
	
	template <typename ... Seed, typename T> static constexpr auto build(const T &&) {
		return typename NextDecider::template TypeStack<Seed...>{};
	}
	
	template <typename ... Seed> using TypeStack = decltype(build<Seed...>(TopOfStack{}));
	template <typename ... Seed> using Type = typename TypeStack<Seed...>::Top;
	using OutputType = Type<>;
	
	constexpr Decider() = default;
	constexpr Decider(const Stack && orig): stack{orig} {}
protected:
	template <typename T, typename ... Args> constexpr void action(Holder<T> && holder, Args && ... args) { holder.action(stack, std::forward<Args>(args)...); }
	template <typename T, typename ... Args> constexpr void action(const Action<T> &&, Args && ... args) { Action<T>::template action<Input::first>(stack, std::forward<Args>(args)...); }
	template <typename ... T> constexpr void action(T ...) const { }
	
	template <typename ... T> constexpr void innerRun(const RejectInput &&, T && ...) { }
	template <typename ... T> constexpr void innerRun(const AcceptInput &&, T && ...) { }
	template <typename ... T> constexpr void innerRun(const ReadChar &&, T && ... args) {
		NextDecider{stack.pop()}.run(std::forward<T>(args)...);
	}
	template <typename N, typename ... T> constexpr void innerRun(const N &&, T && ... args) {
		NextDecider{stack.pop().push(CurrentMove{})}.run(std::forward<T>(args)...);
	}
public:
	template <typename ... T> constexpr void run(T && ... args) {
		action(stack.top(), std::forward<T>(args)...);
		innerRun(CurrentMove{}, args...);
	}
};

template <typename StartSymbol, char... string> class Parser {
public:
	using type = Decider<Stack<StartSymbol>, Input<string...>>;
	type parser{};
	static constexpr const bool correct = type::correct;
	template <typename ... T> constexpr void run(T && ... args) {
		parser.run(std::forward<T>(args)...);
	}
};


}

#endif
