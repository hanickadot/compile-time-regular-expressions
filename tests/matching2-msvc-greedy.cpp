#define CTRE_MSVC_GREEDY_WORKAROUND

#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

#if !CTRE_CNTTP_COMPILER_CHECK
#define CTRE_CREATE(pattern) (pattern ## _ctre)
#define CTRE_SYNTAX(pattern) (pattern ## _ctre_syntax)
#else

template <ctll::fixed_string input> constexpr auto create() {
	constexpr auto _input = input;
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<ctre::pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}

template <ctll::fixed_string input> constexpr bool syntax() {
	constexpr auto _input = input;
	
	return ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template correct_with<ctre::pcre_context<>>;
}


#define CTRE_CREATE(pattern) create<pattern>()
#define CTRE_SYNTAX(pattern) syntax<pattern>()

#endif

using namespace ctre::literals;
using namespace ctre::test_literals;
using namespace std::string_view_literals;

static_assert(CTRE_CREATE("").search("abc"sv));
static_assert(CTRE_CREATE("abc").match("abc"sv));

static_assert(CTRE_CREATE("a").match("a"sv));
static_assert(CTRE_CREATE("a").search("abc"sv));
static_assert(CTRE_CREATE("b").search("abc"sv));
static_assert(!CTRE_CREATE("^b").match("abc"sv));
static_assert(!CTRE_CREATE("b").match("a"sv));
static_assert(CTRE_CREATE(".").match("a"sv));
static_assert(CTRE_CREATE(".").search("abc"sv));
static_assert(CTRE_CREATE("[\\-]").match("-"sv));
static_assert(CTRE_CREATE("[\\\\]").match("\\"sv));
static_assert(CTRE_CREATE("[a-z]").match("a"sv));
static_assert(CTRE_CREATE("[a-z]").match("f"sv));
static_assert(CTRE_CREATE("[a-z]").match("z"sv));
static_assert(!CTRE_CREATE("[a-z]").match("Z"sv));
static_assert(CTRE_CREATE("\\u0050").match("P"sv));
static_assert(CTRE_CREATE("[\\u0050-\\u0051]").match("Q"sv));
static_assert(CTRE_CREATE("\u0050").match("P"sv)); // be aware!

static_assert(CTRE_CREATE("^[\\x30-\\x39]+?$").match("123456789"sv));
static_assert(CTRE_CREATE("[a-z0-9]").match("0"sv));
static_assert(!CTRE_CREATE("[a-z0-9]").match("A"sv));
static_assert(CTRE_CREATE("[[:xdigit:]]").match("0"sv));
static_assert(CTRE_CREATE("[[:xdigit:]]").match("9"sv));
static_assert(CTRE_CREATE("[[:xdigit:]]").match("a"sv));
static_assert(CTRE_CREATE("[[:xdigit:]]").match("A"sv));
static_assert(CTRE_CREATE("[[:xdigit:]]").match("f"sv));
static_assert(!CTRE_CREATE("[[:xdigit:]]").match("g"sv));
static_assert(CTRE_CREATE("abcdef").match("abcdef"sv));
static_assert(!CTRE_CREATE("abcdef").match("abcGef"sv));
static_assert(CTRE_CREATE("").match(""sv));
static_assert(CTRE_CREATE("(?:a|b|c)").match("a"sv));
static_assert(CTRE_CREATE("(?:a|b|c)").match("b"sv));
static_assert(CTRE_CREATE("(?:a|b|c)").match("c"sv));
static_assert(!CTRE_CREATE("(?:a|b|c)").match("d"sv));
static_assert(CTRE_CREATE("(?:xy)?").match("xy"sv));
static_assert(CTRE_CREATE("(?:xy)?").match(""sv));
static_assert(CTRE_CREATE("(?:xy)?").search("zxy"sv));
static_assert(CTRE_CREATE("(?:xy)?$").search("zxy"sv));
static_assert(!CTRE_CREATE("~(?:xy)?$").match("zxy"sv));

static_assert(CTRE_CREATE("^abc").match("abc"sv));
static_assert(CTRE_CREATE("^def$").match("def"sv));
static_assert(!CTRE_CREATE("a^").match("a"sv));
static_assert(!CTRE_CREATE("$a").match("a"sv));

static_assert(CTRE_CREATE("a+?").search("aaax"sv));
static_assert(CTRE_CREATE("a+?").search("ax"sv));
static_assert(!CTRE_CREATE("a+?").match("x"sv));

static_assert(CTRE_CREATE("a++").search("aaax"sv));
static_assert(CTRE_CREATE("a++").search("ax"sv));
static_assert(!CTRE_CREATE("a++").match("x"sv));

static_assert(CTRE_CREATE("a*?x").match("aaax"sv));
static_assert(CTRE_CREATE("a*?x").match("ax"sv));
static_assert(CTRE_CREATE("a*?x").match("x"sv));
static_assert(!CTRE_CREATE("a*?x").match("y"sv));

static_assert(CTRE_CREATE("a*+x").match("aaax"sv));
static_assert(CTRE_CREATE("a*+x").match("ax"sv));
static_assert(CTRE_CREATE("a*+x").match("x"sv));
static_assert(!CTRE_CREATE("a*+x").match("y"sv));

static_assert(!CTRE_CREATE("a*+ab").match("aaab"sv));
static_assert(!CTRE_CREATE("a++ab").match("aaab"sv));
static_assert(!CTRE_CREATE("a*+ab").match("ab"sv));
static_assert(!CTRE_CREATE("a++ab").match("aab"sv));

static_assert(CTRE_CREATE("a*+ba").match("aaba"sv));
static_assert(CTRE_CREATE("a++ba").match("aaba"sv));
static_assert(CTRE_CREATE("a*+ba").match("ba"sv));
static_assert(CTRE_CREATE("a++ba").match("aba"sv));

static_assert(CTRE_CREATE("a{3,}x").match("aaax"sv));
static_assert(CTRE_CREATE("a{3,}x").match("aaaax"sv));

static_assert(CTRE_CREATE("^a{5}").match("aaaaa"sv));
static_assert(CTRE_CREATE("^a{5}").search("aaaaaa"sv));
static_assert(!CTRE_CREATE("^a{5}$").match("aaaaaa"sv));

static_assert(CTRE_CREATE("a*").match("aaa"sv));
static_assert(CTRE_CREATE("a+").match("aaa"sv));
static_assert(CTRE_CREATE("a*").match(""sv));
static_assert(CTRE_CREATE("a+").match("a"sv));

static_assert(CTRE_CREATE("a*$").match("aaa"sv));
static_assert(CTRE_CREATE("a+$").match("aaa"sv));
static_assert(CTRE_CREATE("a*$").match(""sv));
static_assert(CTRE_CREATE("a+$").match("a"sv));

static_assert(CTRE_CREATE("a*xb").match("aaxb"sv));
static_assert(CTRE_CREATE("a+xb").match("aaxb"sv));
static_assert(CTRE_CREATE("a*xb").match("xb"sv));
static_assert(CTRE_CREATE("a+xb").match("axb"sv));

static_assert(CTRE_CREATE("a*ab").match("aaab"sv));
static_assert(CTRE_CREATE("a+ab").match("aaab"sv));
static_assert(CTRE_CREATE("a*ab").match("ab"sv));
static_assert(CTRE_CREATE("a+ab").match("aab"sv));

static_assert(!CTRE_CREATE("^a{2,5}ab").match("aab"sv));
static_assert(CTRE_CREATE("^a{2,5}ab").match("aaab"sv));
static_assert(CTRE_CREATE("^a{2,5}ab").match("aaaab"sv));
static_assert(CTRE_CREATE("^a{2,5}ab").match("aaaaab"sv));
static_assert(CTRE_CREATE("^a{2,5}ab").match("aaaaaab"sv));
static_assert(!CTRE_CREATE("^a{2,5}ab").match("aaaaaaab"sv));

static_assert(CTRE_CREATE("[a-z]+[^a-z]+").match("abcdef123456"sv));

static_assert(CTRE_CREATE("(abc)").match("abc"sv));
static_assert(CTRE_CREATE("(abc)+").match("abc"sv));
static_assert(CTRE_CREATE("(abc)+").match("abcabc"sv));
static_assert(CTRE_CREATE("(abc)+").match("abcabcabc"sv));

static_assert(CTRE_CREATE("(?<name>abc)").match("abc"sv));
static_assert(CTRE_CREATE("(?<name>abc)+").match("abc"sv));
static_assert(CTRE_CREATE("(?<name>abc)+").match("abcabc"sv));
static_assert(CTRE_CREATE("(?<name>abc)+").match("abcabcabc"sv));
static_assert(!CTRE_CREATE("(?<name>abc)+").match("name"sv));

static_assert(std::string_view{CTRE_CREATE("^([a-z]+)").search("abcdef1234"sv)} == "abcdef"sv);
static_assert(std::string_view{CTRE_CREATE("^([a-z]+)1234").match("abcdef1234"sv)} == "abcdef1234"sv);
static_assert(std::string_view{CTRE_CREATE("^([a-z])").search("abcdef1234"sv)} == "a"sv);

static_assert(CTRE_CREATE("^([0-9]+[a-z]+)+").match("123abc456def"sv));
static_assert(CTRE_CREATE("^([0-9]+[a-z]+)+").match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert(CTRE_CREATE("^([0-9]+[a-z]+)+").match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert(CTRE_CREATE("^([0-9]++[a-z]++)+").match("123abc456def"sv));
static_assert(CTRE_CREATE("^([0-9]++[a-z]++)+").match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert(CTRE_CREATE("^([0-9]++[a-z]++)+").match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert(CTRE_CREATE("^([0-9]+?[a-z]+?)+").search("123abc456def"sv));
static_assert(CTRE_CREATE("^([0-9]+?[a-z]+?)+").search("123abc456def"sv).template get<1>() == "123a"sv);
static_assert(CTRE_CREATE("^([0-9]+?[a-z]+?)+").search("123abc456def"sv).template get<0>() == "123a"sv);

static_assert(CTRE_CREATE("^([0-9]+?[a-z]++)+").match("123abc456def"sv));
static_assert(CTRE_CREATE("^([0-9]+?[a-z]++)+").match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert(CTRE_CREATE("^([0-9]+?[a-z]++)+").match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert(CTRE_CREATE("^([a-z]{2})([a-z]{2})").match("abcd"sv).template get<2>() == "cd"sv);
// FIXME  ID support
//static_assert(CTRE_CREATE("^([a-z]{2})(?<second>[a-z]{2})").match("abcd"sv).template get<decltype("second")_id)>() == "cd"sv);

static_assert(CTRE_CREATE("^([a-z]+):\\g{1}$").match("abc:abc"sv));
static_assert(CTRE_CREATE("^([a-z]+):\\g{1}$").match("abc:abc"sv).template get<1>() == "abc"sv);
static_assert(!CTRE_CREATE("^([a-z]+):\\g{1}$").match("abc:abce"sv));
static_assert(CTRE_CREATE("^([a-z]+)\\g{1}$").match("abcabc"sv));
static_assert(!CTRE_CREATE("^([a-z]+)\\g{1}$").match("abcabcd"sv));
static_assert(CTRE_SYNTAX("^([a-z]+)\\g{-1}$"));
static_assert(CTRE_CREATE("^([a-z]+)\\g{-1}$").match("abcabc"sv));
static_assert(!CTRE_SYNTAX("^([a-z]+)\\g{-2}$"));
// TODO check for existence of named capture too

static_assert(CTRE_CREATE("^(?<text>[a-z]+):\\g{text}$").match("abc:abc"sv));

static_assert(CTRE_CREATE("^abc$").match("abc"sv));
static_assert(CTRE_CREATE("^abc$").match(L"abc"sv));
// static_assert(CTRE_CREATE("^abc$").match(u8"abc"sv)); // GCC9.0.1 doesn't support a char8_t string_view literals
static_assert(CTRE_CREATE("^abc$").match(u"abc"sv));
static_assert(CTRE_CREATE("^abc$").match(U"abc"sv));

static_assert(CTRE_CREATE(R"(\(\))").match("()"sv));
static_assert(CTRE_CREATE("\\[\\]").match("[]"sv));
static_assert(CTRE_CREATE(R"(\[\])").match("[]"sv));

static_assert(CTRE_CREATE(R"(\[([A-Z]*?)\])").match("[]"sv));
static_assert(CTRE_CREATE(R"(\[([A-Z]*?)\])").match("[URL]"sv));

static_assert(CTRE_CREATE(R"(\[([\s\S]*?)\]\(([\s\S]*?)\))").match("[URL](https://cpp.fail/ctre)"));

static_assert(CTRE_CREATE("abc").match("abc"));
static_assert(CTRE_CREATE("[_]").match("_"));
static_assert(CTRE_CREATE("[()]").match("("));
static_assert(CTRE_CREATE("[$]").match("$"));
static_assert(CTRE_CREATE("[*]").match("*"));
static_assert(CTRE_CREATE("[+]").match("+"));
static_assert(CTRE_CREATE("[?]").match("?"));
static_assert(CTRE_CREATE("[{}]").match("{"));
static_assert(CTRE_CREATE("[(-)]").match("("));
static_assert(CTRE_CREATE("[(-)]").match(")"));

static_assert(CTRE_CREATE("[A-Z_a-z]").match("a"));
static_assert(CTRE_CREATE("[A-Z_a-z]").match("_"));
static_assert(CTRE_CREATE("[A-Z_a-z]").match("Z"));
// FIXME: maybe in future I will allow this again
// static_assert(CTRE_CREATE("[-]").match("-"));
// static_assert(CTRE_CREATE("[-x]").match("x"));
// FIXME: due current limitation of LL1 grammar parser I can make this work "[x-]" without significant change in grammar
static_assert(CTRE_CREATE("<").match("<"));
static_assert(CTRE_CREATE("(<)").match("<"));
static_assert(CTRE_CREATE("(<>)").match("<>"));
static_assert(CTRE_CREATE("(<>?)").match("<"));
static_assert(CTRE_CREATE("(<?>)").match(">"));
static_assert(CTRE_CREATE("()").match(""));


static_assert((CTRE_CREATE("[a-z]") >> CTRE_CREATE("[0-9]")).match("a9"));
static_assert((CTRE_CREATE("a") | CTRE_CREATE("b")).match("a"));
static_assert((CTRE_CREATE("a") | CTRE_CREATE("b")).match("b"));
static_assert(!(CTRE_CREATE("a") | CTRE_CREATE("b")).match("c"));

static_assert(CTRE_CREATE("((a)(b))").match("ab"sv).template get<0>() == "ab"sv);
static_assert(CTRE_CREATE("((a)(b))").match("ab"sv).template get<1>() == "ab"sv);
static_assert(CTRE_CREATE("((a)(b))").match("ab"sv).template get<2>() == "a"sv);
static_assert(CTRE_CREATE("((a)(b))").match("ab"sv).template get<3>() == "b"sv);

static_assert(CTRE_CREATE("^x(?=y)").search("xy"sv).template get<0>() == "x"sv);
static_assert(CTRE_CREATE("^x(?!a)").search("xy"sv).template get<0>() == "x"sv);

static_assert(CTRE_CREATE("a(?!3)[0-9]").match("a0"sv));
static_assert(CTRE_CREATE("a(?!3)[0-9]").match("a9"sv));
static_assert(!CTRE_CREATE("a(?!3)[0-9]").match("a3"sv));

static_assert(!CTRE_CREATE(".*(.)\\g{1}.*").match("abcdefghijk"sv));
static_assert(CTRE_CREATE(".*(.)\\g{1}.*").match("aabcdefghijk"sv));
static_assert(CTRE_CREATE(".*(.)\\g{1}.*").match("abcdeffghijk"sv));

static_assert(CTRE_CREATE("(?=.*(.)\\g{1})[a-z]+").match("abcdeffghijk"sv));
static_assert(!CTRE_CREATE("(?=.*(.)\\g{1}{2})[a-z]+").match("abcddeffghijk"sv));
static_assert(CTRE_CREATE("(?=.*(.)\\g{1}{2})[a-z]+").match("abcdddeffghijk"sv));

static_assert( CTRE_CREATE("(?!.*(.)\\g{1})[a-z]+").match("abcdefgh"sv));
static_assert(!CTRE_CREATE("(?!.*(.)\\g{1})[a-z]+").match("abcdeefgh"sv));

static_assert(CTRE_CREATE("_").match("_"sv));
static_assert(CTRE_CREATE("[<]").match("<"sv));
static_assert(CTRE_CREATE("[>]").match(">"sv));
static_assert(CTRE_CREATE("[<>]").match("<"sv));
static_assert(CTRE_CREATE("[<>]+").match("><"sv));

static_assert(CTRE_CREATE("<[a-z]+>").match("<aloha>"sv));
static_assert(CTRE_CREATE("(<[a-z]+>)\\g{1}").match("<aloha><aloha>"sv));

// issue #60
static_assert(CTRE_CREATE("[^\\^]").match("a"sv));
static_assert(CTRE_CREATE("[^^]").match("a"sv));
static_assert(CTRE_CREATE("[\\-]").match("-"sv));
static_assert(CTRE_CREATE("[\\--\\-]").match("-"sv));

// msvc
static_assert(CTRE_CREATE("[a-z]+abc").match("xxxabc"));
