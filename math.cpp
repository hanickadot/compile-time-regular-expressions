#include "static-decider.hpp"

#include <iostream>

SYMBOL(E);
SYMBOL(E2);
SYMBOL(F);
SYMBOL(T2);
SYMBOL(T);
SYMBOL(Num);
SYMBOL(Num2);

SYMBOL(Char_Open);
SYMBOL(Char_Close);
SYMBOL(Char_Star);
SYMBOL(Char_Slash);
SYMBOL(Char_Plus);
SYMBOL(Char_Minus);
SYMBOL(Char_Num);

struct Operands {
	static unsigned int getID() {
		static unsigned int i{0};
		return i++;
	}
	unsigned int id{getID()};
	bool usage{false};
	int64_t a{0};
	int64_t b{0};
	void push(int64_t v) {
		if (!usage) {
			usage = true;
			////printf("[%d] push a = %lld <- %lld\n",id,a,v);
			a = v;
		} else {
			////printf("[%d] push b = %lld <- %lld\n",id,b,v);
			b = v;
		}
	}
	template <typename Stack, typename... T> constexpr void action(Stack & stack, T && ...) {
		stack.template nearest<Operands>().push(a);
		////printf("[%d] operand: %lld %lld\n",id,a,b);
	}
};

struct Term;
struct Expression;
struct Fact;


struct Fact {
	int64_t value{0};
	template <typename Stack, typename... T> constexpr void action(Stack & stack, T && ...) {
		//printf("fact: %llu\n",value);
		stack.template nearest<Term>().value = value;
	}
};

struct Term {
	int64_t value{0};
	template <typename Stack, typename... T> constexpr void action(Stack & stack, T && ...) {
		//printf("term: %llu\n",value);
		stack.template nearest<Expression>().value = value;
	}
};

struct Expression {
	int64_t value{0};
	struct Fallback { };
	template <typename Stack, typename... T> constexpr void found(Fallback &, Stack & stack, int64_t & output, T && ...) {
		//printf("return: %llu\n",value);
		output = value;
	}
	template <typename Stack, typename... T> constexpr void found(Fact & fact, Stack & stack, T && ...) {
		//printf("subexpression: %llu\n",value);
		fact.value = value;
	}
	template <typename Stack, typename... T> constexpr void action(Stack & stack, T && ... args) {
		Fallback fb;
		found(stack.template nearest<Fact>(fb), stack, std::forward<T>(args)...);
	}
};



struct Number {
	int64_t value{0};
	template <typename Stack, typename... T> constexpr void action(Stack & stack, T && ...) {
		stack.template nearest<Fact>().value = value;
	}
};



struct Digit {
	template <char current, typename Stack, typename... T> static constexpr void action(Stack & stack, T && ... args) {
		static_assert(current >= '0');
		static_assert(current <= '9');
		stack.template nearest<Number>().value = stack.template nearest<Number>().value * 10 + (current - '0');
	}
};

struct Plus {
	template <typename Stack, typename... T> static constexpr void action(Stack & stack, T && ... args) {
		auto & se = stack.template nearest<Expression>();
		auto & te = stack.template nearest<Term>();
		//printf("[PLUS] %lld + %lld\n",se.value,te.value);
		te.value = se.value + te.value;
	}
};

struct Minus {
	template <typename Stack, typename... T> static constexpr void action(Stack & stack, T && ... args) {
		auto & se = stack.template nearest<Expression>();
		auto & te = stack.template nearest<Term>();
		//printf("[MINUS] %lld - %lld\n",se.value,te.value);
		te.value = se.value - te.value;
	}
};

struct Times {
	template <typename Stack, typename... T> static constexpr void action(Stack & stack, T && ... args) {
		auto & a = stack.template nearest<Term>();
		auto & b = stack.template nearest<Fact>();
		//printf("[TIMES] %lld * %lld\n",a.value,b.value);
		b.value = a.value * b.value;
	}
};

struct Divide {
	template <typename Stack, typename... T> static constexpr void action(Stack & stack, T && ... args) {
		auto & a = stack.template nearest<Term>();
		auto & b = stack.template nearest<Fact>();
		//printf("[DIVIDE] %lld / %lld\n",a.value,b.value);
		b.value = b.value ? (a.value / b.value) : 0;
	}
};


template <> struct Static::Table<E, '('> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '0'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '1'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '2'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '3'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '4'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '5'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '6'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '7'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '8'> { using Move = Static::String<T, Holder<Term>, E2>; };
template <> struct Static::Table<E, '9'> { using Move = Static::String<T, Holder<Term>, E2>; };

template <> struct Static::Table<E2, ')'> { using Move = Epsilon; };
template <> struct Static::Table<E2, '+'> { using Move = Static::String<Char_Plus, T, Holder<Plus>, Holder<Term>, E2>; };
template <> struct Static::Table<E2, '-'> { using Move = Static::String<Char_Minus, T, Holder<Minus>, Holder<Term>, E2>; };
template <> struct Static::Table<E2> { using Move = Epsilon; };

template <> struct Static::Table<F,'('> { using Move = Static::String<Char_Open, E, Holder<Expression>, Char_Close>; };
template <> struct Static::Table<F,'0'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'1'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'2'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'3'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'4'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'5'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'6'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'7'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'8'> { using Move = Static::String<Num,Holder<Number>>; };
template <> struct Static::Table<F,'9'> { using Move = Static::String<Num,Holder<Number>>; };

template <> struct Static::Table<Num2,')'> { using Move = Epsilon; };
template <> struct Static::Table<Num2,'*'> { using Move = Epsilon; };
template <> struct Static::Table<Num2,'+'> { using Move = Epsilon; };
template <> struct Static::Table<Num2,'-'> { using Move = Epsilon; };
template <> struct Static::Table<Num2,'/'> { using Move = Epsilon; };
template <> struct Static::Table<Num2,'0'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'1'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'2'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'3'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'4'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'5'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'6'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'7'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'8'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2,'9'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num2> { using Move = Epsilon; };

template <> struct Static::Table<Num,'0'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'1'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'2'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'3'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'4'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'5'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'6'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'7'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'8'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };
template <> struct Static::Table<Num,'9'> { using Move = Static::String<Action<Digit>,Char_Num,Num2>; };


template <> struct Static::Table<T2,')'> { using Move = Epsilon; };
template <> struct Static::Table<T2,'+'> { using Move = Epsilon; };
template <> struct Static::Table<T2,'-'> { using Move = Epsilon; };
template <> struct Static::Table<T2,'*'> { using Move = Static::String<Char_Star, F, Holder<Times>, Holder<Fact>, T2>; };
template <> struct Static::Table<T2,'/'> { using Move = Static::String<Char_Slash, F, Holder<Divide>, Holder<Fact>, T2>; };
template <> struct Static::Table<T2> { using Move = Epsilon; };

template <> struct Static::Table<T, '('> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '0'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '1'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '2'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '3'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '4'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '5'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '6'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '7'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '8'> { using Move = Static::String<F, Holder<Fact>, T2>; };
template <> struct Static::Table<T, '9'> { using Move = Static::String<F, Holder<Fact>, T2>; };

template <> struct Static::Table<Char_Open, '('> { using Move = ReadChar; };
template <> struct Static::Table<Char_Close, ')'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Star, '*'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Slash, '/'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Plus, '+'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Minus, '-'> { using Move = ReadChar; };

template <> struct Static::Table<Char_Num, '0'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '1'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '2'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '3'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '4'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '5'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '6'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '7'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '8'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Num, '9'> { using Move = ReadChar; };


template<typename CharT, CharT ... string> constexpr auto operator""_expr() {
	static_assert(std::is_same<CharT, char>::value);
	using namespace Static;
	using Parser = Decider<Stack<E, Holder<Expression>>, Input<string...>>;
	static_assert(Parser::correct);
	int64_t output{0};
	Parser{}.run(output);
	return output;
}

int main() {
	static_assert("2*(3+2)+32"_expr == 42);
	static_assert("2*3+2"_expr == 8);
	static_assert("42"_expr == 42);
	static_assert("(((42)))"_expr == 42);
	static_assert("2-1"_expr == 1);
	
	int64_t value = "42*(2-1)"_expr;
	printf("%lld\n",value);
}
