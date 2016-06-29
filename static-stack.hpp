#ifndef STATIC_STACK_HPP
#define STATIC_STACK_HPP

#include <cstdint>
#include <utility>
#include <ostream>

namespace Static {

struct EmptyStackSymbol {
	static const char * identify() { return "empty-stack-symbol"; }
};	

template <typename T> struct Holder {
	T data;
	static const char * identify() { return "Holder<>"; }
	template <typename Stack, typename... Args> void action(Stack & stack, Args && ... args) { data.action(stack, std::forward<Args>(args)...); }
	Holder() = default;
};

template <typename ... Content> class Stack;
template <typename ... Content> using String = Stack<Content...>;
using Epsilon = Stack<>;

}

template <typename ... T> std::ostream & operator<<(std::ostream & , const Static::Stack<T...> &);

namespace Static {

template <typename ... Current> struct StackHelper {
	template <typename ... Rest> struct Push {
		using Type = Stack<Rest...,Current...>;
	};
	template <typename ... Rest> struct Push<Stack<Rest...>> {
		using Type = Stack<Rest...,Current...>;
	};
};

//template <typename... Wanted> constexpr auto & nearest() {
//	return NearestHelper<Wanted...>::get(topItem, content);
//}

template <typename... Wanted> class NearestHelper;

template <typename TopItem, typename ... Wanted> struct NearestSelector;

template <typename TopItem, typename First, typename ... Rest> struct NearestSelector<TopItem, First, Rest...> {
	static constexpr const bool have = NearestSelector<TopItem, Rest...>::have;
	
};

template <typename Wanted, typename ... Rest> struct NearestSelector<Wanted,Wanted,Rest...> {
	static constexpr const bool have = true;
};

template <typename Wanted, typename ... Rest> struct NearestSelector<Holder<Wanted>,Wanted,Rest...> {
	static constexpr const bool have = true;
};

template <typename TopItem> struct NearestSelector<TopItem> {
	static constexpr const bool have = false;
};

template <bool have, typename... Wanted> struct NearestReturn;

template <typename... Wanted> struct NearestReturn<true, Wanted...> {
	template <typename TopItem, typename... Content> static auto & get(Holder<TopItem> & top, Stack<Content...> &) {
		return top.data;
	}
	template <typename TopItem, typename... Content> static auto & get(TopItem & top, Stack<Content...> &) {
		return top;
	}
};

template <typename... Wanted> struct NearestReturn<false, Wanted...> {
	template <typename TopItem, typename... Content> static auto & get(TopItem &, Stack<Content...> & stack) {
		return stack.template nearest<Wanted...>();
	}
};


template <typename... Wanted> class NearestHelper {
public:
	template <typename TopItem, typename... Content> static auto & get(TopItem & top, Stack<Content...> & stack) {
		return NearestReturn<NearestSelector<TopItem, Wanted...>::have, Wanted...>::get(top, stack);
	}
};


	//template <typename TopItem, typename... Content> static constexpr Wanted & get(TopItem &, Stack<Content...> & rest) {
	//	static_assert(sizeof...(Content) != 0, "Wanted type is not present!");
	//	return rest.template nearest<Wanted>();
	//}
	//static constexpr Wanted & get(Wanted & wanted, Stack<Content...> &) {
	//	return wanted;
	//}
	//static constexpr Wanted & get(Holder<Wanted> & wanted, Stack<Content...> &) {
	//	return wanted.data;
	//}

	

template <typename TopItem, typename ... Content> class Stack<TopItem, Content...> {
protected:
	TopItem topItem{};
	Stack<Content...> content{};
public:
	static constexpr const size_t size = sizeof...(Content) + 1;
	using Top = TopItem;
	static constexpr const bool empty = false;
	template <typename ... Item> using Push = typename StackHelper<TopItem, Content...>::template Push<Item...>::Type;
	using Pop = Stack<Content...>;
	
	constexpr Stack(const TopItem & ltop, const Content & ... lcontent): topItem{ltop}, content{lcontent...} { }
	template <typename ... Rest> constexpr Stack(const TopItem & ltop, const Rest & ... rest): topItem{ltop}, content{rest...} { }
	constexpr Stack(const TopItem & ltop, const Stack & lcontent): topItem{ltop}, content{lcontent} { }
	
	constexpr Stack() { }
	constexpr Stack(const Stack &) = default;
	constexpr Stack(Stack &&) = default;
	
	constexpr Top top() const { return topItem; }
	constexpr Pop pop() const { return Pop{content}; }
	
protected:
	template <typename ... Item> constexpr Push<Item...> push2(Stack<> && stack, Item && ... item) const {
		return Push<Item...>{std::forward<Item>(item)..., topItem, content};
	}
	template <typename ... Item, typename ... Rest> constexpr Push<Item..., Rest...> push2(Stack<Rest...> && stack, Item && ... item) const {
		return push2(stack.pop(), std::forward<Item>(item)..., stack.top());
	}
	
public:	
	template <typename ... Item> constexpr Push<Item...> push(const Item & ... item) const {
		return Push<Item...>{item..., topItem, content};
	}
	
	template <typename ... Item> constexpr Push<Item...> push(const Stack<Item...> & stack) const {
		return push2(stack.pop(), stack.top());
	}
	
	constexpr Push<> push(const Stack<> & stack) const {
		return Push<>{topItem, content};
	}
		
	template <typename... Wanted> constexpr auto & nearest() {
		return NearestHelper<Wanted...>::get(topItem, content);
	}
	
	std::ostream & operator<<(std::ostream & stream) const {
		return stream << topItem << " [" << content << "]";
	}
	
	template <typename ... T> constexpr bool operator==(const Stack<T...> &) const {
		return false;
	}
	
	constexpr bool operator==(const Stack<TopItem, Content...> & right) const {
		return topItem == right.topItem && content == right.content;
	}
};

template <> class Stack<> {
public:
	static constexpr const size_t size = 0;
	using Top = EmptyStackSymbol;
	static constexpr const bool empty = true;
	template <typename ... Item> using Push = typename StackHelper<>::template Push<Item...>::Type;
	using Pop = Stack<>;
	
	constexpr Stack() { }
	constexpr Stack(const Stack &) = default;
	constexpr Stack(Stack &&) = default;
	
	constexpr Top top() const { return Top{}; }
	constexpr Pop pop() const { return Pop{}; }
	
protected:
	template <typename ... Item> constexpr Push<Item...> push2(Stack<> && stack, Item && ... item) const {
		return Push<Item...>{std::forward<Item>(item)...};
	}
	template <typename ... Item, typename ... Rest> constexpr Push<Item..., Rest...> push2(Stack<Rest...> && stack, Item && ... item) const {
		return push2(stack.pop(), std::forward<Item>(item)..., stack.top());
	}
	
public:	
	template <typename ... Item> constexpr Push<Item...> push(const Item & ... item) const {
		return Push<Item...>{std::forward<Item>(item)...};
	}
	
	template <typename ... Item> constexpr Push<Item...> push(const Stack<Item...> & stack) const {
		return push2(stack.pop(), stack.top());
	}
	
	constexpr Push<> push(const Stack<> & stack) const {
		return Push<>{};
	}
	
	std::ostream & operator<<(std::ostream & stream) const {
		return stream << " empty";
	} 
	
	template <typename ... T> constexpr bool operator==(const Stack<T...> &) const {
		return false;
	}
	
	constexpr bool operator==(const Stack<> &) const {
		return true;
	}
};

}

template <typename ... T> std::ostream & operator<<(std::ostream & stream, const Static::Stack<T...> & stack) {
	return stack.operator<<(stream);
}

#endif
