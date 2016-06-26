#include "static-parser.hpp"
#include <cstdio>

struct NT_E {};
struct NT_E2 {};
struct NT_F {};
struct NT_T {};
struct NT_T2 {};
struct Symbol_OpenBrace {};
struct Symbol_CloseBrace {};
struct Symbol_Times {};
struct Symbol_Divide {};
struct Symbol_Plus {};
struct Symbol_Minus {};
struct Symbol_Var {};

struct PrintMinus {
	static void action() {
		printf("minus ");
	}
};

struct PrintPlus {
	static void action() {
		printf("plus ");
	}
};

struct PrintTimes {
	static void action() {
		printf("times ");
	}
};

struct PrintDivide {
	static void action() {
		printf("divide ");
	}
};

struct PrintVar {
	static void action() {
		printf("[variable] ");
	}
};

template <char... c> struct Syntax::Move<PrintMinus, c...> { using N = Epsilon; };

template <> struct Syntax::Move<NT_E, '('> { using N = String<NT_T, NT_E2>; };
template <> struct Syntax::Move<NT_E, 'x'> { using N = String<NT_T, NT_E2>; };
template <> struct Syntax::Move<NT_E> { using N = Epsilon; };

template <> struct Syntax::Move<NT_E2, ')'> { using N = Epsilon; };
template <> struct Syntax::Move<NT_E2, '+'> { using N = String<Symbol_Plus, NT_T, PrintPlus, NT_E2>; };
template <> struct Syntax::Move<NT_E2, '-'> { using N = String<Symbol_Minus, NT_T, PrintMinus, NT_E2>; };
template <> struct Syntax::Move<NT_E2> { using N = Epsilon; };

template <> struct Syntax::Move<NT_F, '('> { using N = String<Symbol_OpenBrace, NT_E, Symbol_CloseBrace>; };
template <> struct Syntax::Move<NT_F, 'x'> { using N = String<Symbol_Var, PrintVar>; };

template <> struct Syntax::Move<NT_T, '('> { using N = String<NT_F, NT_T2>; };
template <> struct Syntax::Move<NT_T, 'x'> { using N = String<NT_F, NT_T2>; };

template <> struct Syntax::Move<NT_T2, ')'> { using N = Epsilon; };
template <> struct Syntax::Move<NT_T2, '+'> { using N = Epsilon; };
template <> struct Syntax::Move<NT_T2, '-'> { using N = Epsilon; };
template <> struct Syntax::Move<NT_T2, '*'> { using N = String<Symbol_Times, NT_F, PrintTimes, NT_T2>; };
template <> struct Syntax::Move<NT_T2, '/'> { using N = String<Symbol_Divide, NT_F, PrintDivide, NT_T2>; };
template <> struct Syntax::Move<NT_T2> { using N = Epsilon; };

template <> struct Syntax::Move<Symbol_OpenBrace, '('> { using N = ReadChar; };
template <> struct Syntax::Move<Symbol_CloseBrace, ')'> { using N = ReadChar; };
template <> struct Syntax::Move<Symbol_Times, '*'> { using N = ReadChar; };
template <> struct Syntax::Move<Symbol_Divide, '/'> { using N = ReadChar; };
template <> struct Syntax::Move<Symbol_Plus, '+'> { using N = ReadChar; };
template <> struct Syntax::Move<Symbol_Minus, '-'> { using N = ReadChar; };
template <> struct Syntax::Move<Symbol_Var, 'x'> { using N = ReadChar; };

template<typename CharT, CharT ... string> constexpr auto operator""_math() {
	return Syntax::Parser<Syntax::Stack<NT_E>, string...>{};
}

int main() {
	if ("x*x+x"_math.template parse<true>()) {
		puts("\nACCEPTED");
	} else {
		puts("\nREJECTED");
	}
}
