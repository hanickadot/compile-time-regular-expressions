#include "static-decider.hpp"

#include <iostream>

SYMBOL(S);
SYMBOL(Part);
SYMBOL(Symbol);

SYMBOL(Char_Anything);
SYMBOL(Char_Begin);
SYMBOL(Char_End);
SYMBOL(Char_ABC);

namespace RegExp {
	
	template <typename ...> struct RegExp {
	};

	template <char c> struct Char {
	};

	struct Anything {

	};

	struct Begin {

	};

	struct End {

	};
	
}



template <typename What> struct Add {
	template <char c, typename ... Definition, typename ... Content> static constexpr auto build(const Static::Stack<RegExp::RegExp<Definition...>, Content...> &&) {
		return Static::Stack<RegExp::RegExp<Definition..., What>,Content...>{};
	}
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) {
		return Static::Stack<Content...,What>{};
	}
};

template <template <char c> typename What> struct AddChar {
	template <char c, typename ... Definition, typename ... Content> static constexpr auto build(const Static::Stack<RegExp::RegExp<Definition...>, Content...> &&) {
		return Static::Stack<RegExp::RegExp<Definition..., What<c>>,Content...>{};
	}
	template <char c, typename ... Content> static constexpr auto build(const Static::Stack<Content...> &&) {
		return Static::Stack<Content...,What<c>>{};
	}
};

template <char ... c> struct Static::Table<S, c...> { using Move = Part; };

template <char c> struct Static::Table<Part, c> { using Move = Static::String<Symbol, Part>; };
template <> struct Static::Table<Part> { using Move = Epsilon; };

template <> struct Static::Table<Symbol,'.'> { using Move = Static::String<Builder<Add<RegExp::Anything>>, Char_Anything>; };
template <> struct Static::Table<Symbol,'^'> { using Move = Static::String<Builder<Add<RegExp::Begin>>, Char_Begin>; };
template <> struct Static::Table<Symbol,'$'> { using Move = Static::String<Builder<Add<RegExp::End>>, Char_End>; };

template <char c> struct Static::Table<Symbol,c> { using Move = Static::String<Builder<AddChar<RegExp::Char>>, Char_ABC>; };

template <> struct Static::Table<Char_Anything,'.'> { using Move = ReadChar; };
template <> struct Static::Table<Char_Begin,'^'> { using Move = ReadChar; };
template <> struct Static::Table<Char_End,'$'> { using Move = ReadChar; };

template <char c> struct Static::Table<Char_ABC,c> { using Move = ReadChar; };
template <> struct Static::Table<Char_ABC,'.'> { using Move = RejectInput; };
template <> struct Static::Table<Char_ABC,'^'> { using Move = RejectInput; };
template <> struct Static::Table<Char_ABC,'$'> { using Move = RejectInput; };


template<typename CharT, CharT ... string> constexpr auto operator""_re() {
	using namespace Static;
	using namespace RegExp;
	
	using Parser = Decider<Stack<S>, Input<string...>>;
	static_assert(Parser::correct);
	return typename Parser::template Type<RegExp<>>{};
}

int main() {
	using namespace RegExp;
	static_assert( std::is_same<decltype("x$"_re), RegExp<Char<'x'>,End>> ::value);
	static_assert( std::is_same<decltype("^x$"_re), RegExp<Begin,Char<'x'>,End>> ::value);
	static_assert( std::is_same<decltype("^xy$"_re), RegExp<Begin,Char<'x'>,Char<'y'>,End>> ::value);
	static_assert( std::is_same<decltype("^."_re), RegExp<Begin,Anything>> ::value);

}
