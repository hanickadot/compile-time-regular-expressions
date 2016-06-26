#include "static-parser.hpp"
#include <cstdio>

namespace Regexp {
	struct S {};
	struct item {};
	struct items2 {};
	struct items {};
	
	struct OpenBrace {};
	struct CloseBrace {};
	struct Comma {};
	struct X {};
}

template <> struct Syntax::Move<Regexp::S,'('> { using N = String<Regexp::item>; };
template <> struct Syntax::Move<Regexp::S,'x'> { using N = String<Regexp::item>; };

template <> struct Syntax::Move<Regexp::item,'('> { using N = String<Regexp::OpenBrace, Regexp::items, Regexp::CloseBrace>; };
template <> struct Syntax::Move<Regexp::item,'x'> { using N = String<Regexp::X>; };

template <> struct Syntax::Move<Regexp::items2,')'> { using N = Epsilon; };
template <> struct Syntax::Move<Regexp::items2,','> { using N = String<Regexp::Comma, Regexp::item, Regexp::items2>; };

template <> struct Syntax::Move<Regexp::items,'('> { using N = String<Regexp::item, Regexp::items2>; };
template <> struct Syntax::Move<Regexp::items,')'> { using N = Epsilon; };
template <> struct Syntax::Move<Regexp::items,'x'> { using N = String<Regexp::item, Regexp::items2>; };

template <> struct Syntax::Move<Regexp::OpenBrace,'('> { using N = ReadChar; };
template <> struct Syntax::Move<Regexp::CloseBrace,')'> { using N = ReadChar; };
template <> struct Syntax::Move<Regexp::Comma,','> { using N = ReadChar; };
template <> struct Syntax::Move<Regexp::X,'x'> { using N = ReadChar; };


template <typename Syntax> class RegularExpression {
public:
	bool operator()(const char * input) {
		return Syntax::template parse<true>();
	}
};

template<typename CharT, CharT ... string> constexpr auto operator""_regexp() {
	using RegexpDef = Syntax::Parser<Syntax::Stack<Regexp::S>, string...>;
	static_assert(RegexpDef::template parse<false>(), "bad syntax of regular expression");
	return RegularExpression<RegexpDef>{};
}

int main(int argc, char ** argv) {
	if ("(x,(x),(x,x,x))"_regexp(argv[1])) {
		puts("MATCH");
	} else {
		puts("not match");
	}
}
