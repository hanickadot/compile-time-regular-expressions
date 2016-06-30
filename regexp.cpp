#include "static-decider.hpp"

#include <iostream>

SYMBOL(RegExp);
SYMBOL(Part);
SYMBOL(Symbol);

SYMBOL(Char_Anything);
SYMBOL(Char_Begin);
SYMBOL(Char_End);
SYMBOL(Char_ABC);

template <char c> struct Char {
};

template <typename ...> struct RegExpBase {
};

struct Anything {

};

struct Begin {

};

struct End {

};

template <typename What> struct Add {
	template <char c, typename ... Definition, typename ... Content> static constexpr auto build(const Static::Stack<RegExpBase<Definition...>, Content...> &&) {
		return Static::Stack<RegExpBase<Definition..., What>,Content...>{};
	}
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) {
		return Static::Stack<Content...,What>{};
	}
};

template <template <char c> typename What> struct AddChar {
	template <char c, typename ... Definition, typename ... Content> static constexpr auto build(const Static::Stack<RegExpBase<Definition...>, Content...> &&) {
		return Static::Stack<RegExpBase<Definition..., What<c>>,Content...>{};
	}
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) {
		return Static::Stack<Content...,What<c>>{};
	}
};

template <char ... c> struct Static::Table<RegExp, c...> { using Move = Part; };

template <char c> struct Static::Table<Part, c> { using Move = Static::String<Symbol, Part>; };
template <> struct Static::Table<Part> { using Move = Epsilon; };

template <> struct Static::Table<Symbol,'.'> { using Move = Static::String<Builder<Add<Anything>>, Char_Anything>; };
template <> struct Static::Table<Symbol,'^'> { using Move = Static::String<Builder<Add<Begin>>, Char_Begin>; };
template <> struct Static::Table<Symbol,'$'> { using Move = Static::String<Builder<Add<End>>, Char_End>; };

template <char c> struct Static::Table<Symbol,c> { using Move = Static::String<Builder<AddChar<Char>>, Char_ABC>; };

template <> struct Static::Table<Char_Anything,'.'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Begin,'^'> { using Move = ReadChar; };
template <> struct Static::Table<Char_End,'$'> { using Move = ReadChar; };

template <char c> struct Static::Table<Char_ABC,c> { using Move = ReadChar; };
template <> struct Static::Table<Char_ABC,'.'> { using Move = RejectInput; };
template <> struct Static::Table<Char_ABC,'^'> { using Move = RejectInput; };
template <> struct Static::Table<Char_ABC,'$'> { using Move = RejectInput; };


template<typename CharT, CharT ... string> constexpr auto operator""_re() {
	using namespace Static;
	using Parser = Decider<Stack<RegExp>, Input<string...>>;
	static_assert(Parser::correct);
	return typename Parser::template Type<RegExpBase<>>{};
}

int main() {
	static_assert( std::is_same<decltype("x$"_re), RegExpBase<Char<'x'>,End>> ::value);
	static_assert( std::is_same<decltype("^x$"_re), RegExpBase<Begin,Char<'x'>,End>> ::value);
	static_assert( std::is_same<decltype("^xy$"_re), RegExpBase<Begin,Char<'x'>,Char<'y'>,End>> ::value);
	static_assert( std::is_same<decltype("^."_re), RegExpBase<Begin,Anything>> ::value);

}
