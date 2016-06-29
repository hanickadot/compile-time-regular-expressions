#include "static-decider.hpp"

#include <iostream>

SYMBOL(S);

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
			//printf("[%d] push a = %lld <- %lld\n",id,a,v);
			a = v;
		} else {
			//printf("[%d] push b = %lld <- %lld\n",id,b,v);
			b = v;
		}
	}
	template <typename Stack, typename... T> void action(Stack & stack, T && ...) {
		stack.template nearest<Operands>().push(a);
		//printf("[%d] operand: %lld %lld\n",id,a,b);
	}
};

struct Number {
	int64_t value{0};
	template <typename Stack, typename... T> void action(Stack & stack, T && ...) {
		printf("number: %lld\n",value);
		stack.template nearest<Operands>().push(value);
	}
};

template <char c> struct Print {
	template <char current, typename Stack, typename... T> static void action(Stack & stack, T && ... args) {
		printf("%c\n",c);
	}
};

struct Digit {
	template <char current, typename Stack, typename... T> static void action(Stack & stack, T && ... args) {
		static_assert(current >= '0');
		static_assert(current <= '9');
		stack.template nearest<Number>().value = stack.template nearest<Number>().value * 10 + (current - '0');
	}
};

struct Plus {
	template <typename Stack, typename... T> static void action(Stack & stack, T && ... args) {
		auto & ref = stack.template nearest<Operands>();
		printf("plus %lld + %lld => %lld\n",ref.a,ref.b,ref.a+ref.b);
		ref.a = ref.a + ref.b;
	}
};

struct Minus {
	template <typename Stack, typename... T> static void action(Stack & stack, T && ... args) {
		auto & ref = stack.template nearest<Operands>();
		printf("minus %lld - %lld => %lld\n",ref.a,ref.b,ref.a-ref.b);
		ref.a = ref.a - ref.b;
	}
};

struct Times {
	template <typename Stack, typename... T> static void action(Stack & stack, T && ... args) {
		auto & ref = stack.template nearest<Operands>();
		printf("times %lld * %lld => %lld\n",ref.a,ref.b,ref.a*ref.b);
		ref.a = ref.a * ref.b;
	}
};

struct Divide {
	template <typename Stack, typename... T> static void action(Stack & stack, T && ... args) {
		auto & ref = stack.template nearest<Operands>();
		printf("divide %lld / %lld => %lld\n",ref.a,ref.b,ref.b ? ref.a/ref.b : 0);
		ref.a = ref.b ? ref.a / ref.b : 0;
	}
};



template <char... c> struct Static::Table<S, c...> { using Move = Static::String<E, Holder<Operands>>;};

template <> struct Static::Table<E, '('> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '0'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '1'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '2'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '3'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '4'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '5'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '6'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '7'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '8'> { using Move = Static::String<T,E2>; };
template <> struct Static::Table<E, '9'> { using Move = Static::String<T,E2>; };

template <> struct Static::Table<E2, ')'> { using Move = Epsilon; };
template <> struct Static::Table<E2, '+'> { using Move = Static::String<Char_Plus, T, Holder<Plus>, E2>; };
template <> struct Static::Table<E2, '-'> { using Move = Static::String<Char_Minus, T, Holder<Minus>, E2>; };
template <> struct Static::Table<E2> { using Move = Epsilon; };

template <> struct Static::Table<F,'('> { using Move = Static::String<Char_Open, E, Char_Close>; };
template <> struct Static::Table<F,'0'> { using Move = Num; };
template <> struct Static::Table<F,'1'> { using Move = Num; };
template <> struct Static::Table<F,'2'> { using Move = Num; };
template <> struct Static::Table<F,'3'> { using Move = Num; };
template <> struct Static::Table<F,'4'> { using Move = Num; };
template <> struct Static::Table<F,'5'> { using Move = Num; };
template <> struct Static::Table<F,'6'> { using Move = Num; };
template <> struct Static::Table<F,'7'> { using Move = Num; };
template <> struct Static::Table<F,'8'> { using Move = Num; };
template <> struct Static::Table<F,'9'> { using Move = Num; };

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

template <> struct Static::Table<Num,'0'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'1'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'2'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'3'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'4'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'5'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'6'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'7'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'8'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };
template <> struct Static::Table<Num,'9'> { using Move = Static::String<Action<Digit>,Char_Num,Num2,Holder<Number>>; };


template <> struct Static::Table<T2,')'> { using Move = Epsilon; };
template <> struct Static::Table<T2,'+'> { using Move = Epsilon; };
template <> struct Static::Table<T2,'-'> { using Move = Epsilon; };
template <> struct Static::Table<T2,'*'> { using Move = Static::String<Char_Star, F, Holder<Times>, T2>; };
template <> struct Static::Table<T2,'/'> { using Move = Static::String<Char_Slash, F, Holder<Divide>, T2>; };
template <> struct Static::Table<T2> { using Move = Epsilon; };

template <> struct Static::Table<T, '('> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '0'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '1'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '2'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '3'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '4'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '5'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '6'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '7'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '8'> { using Move = Static::String<F, T2>; };
template <> struct Static::Table<T, '9'> { using Move = Static::String<F, T2>; };

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

template <char... string> struct PrintHelper;

template <char first, char ... string> struct PrintHelper<first, string...> {
static void print() {
	putchar(first);
	PrintHelper<string...>::print();
}
};

template <> struct PrintHelper<> {
static void print() {
	putchar('\n');
}
};

template<typename CharT, CharT ... string> constexpr auto operator""_expr() {
	static_assert(Static::Parser<S,string...>::value);
	PrintHelper<string...>::print();
	Static::Parser<S,string...>{}.run();
}

int main() {
	"3+4*5"_expr;
}
