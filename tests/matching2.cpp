#include <ctre.hpp>
#include <string_view>

using namespace ctre::literals;
using namespace ctre::test_literals;
using namespace std::string_view_literals;

static_assert(""_ctre.search("abc"sv));
static_assert("abc"_ctre.match("abc"sv));

static_assert("a"_ctre.match("a"sv));
static_assert("a"_ctre.search("abc"sv));
static_assert("b"_ctre.search("abc"sv));
static_assert(!"^b"_ctre.match("abc"sv));
static_assert(!"b"_ctre.match("a"sv));
static_assert("."_ctre.match("a"sv));
static_assert("."_ctre.search("abc"sv));
static_assert("[\\-]"_ctre.match("-"sv));
static_assert("[\\\\]"_ctre.match("\\"sv));
static_assert("[a-z]"_ctre.match("a"sv));
static_assert("[a-z]"_ctre.match("f"sv));
static_assert("[a-z]"_ctre.match("z"sv));
static_assert(!"[a-z]"_ctre.match("Z"sv));
static_assert("\\u0050"_ctre.match("P"sv));
static_assert("\u0050"_ctre.match("P"sv)); // be aware!
static_assert("^[\\x30-\\x39]+?$"_ctre.match("123456789"sv));
static_assert("[a-z0-9]"_ctre.match("0"sv));
static_assert(!"[a-z0-9]"_ctre.match("A"sv));
static_assert("[[:xdigit:]]"_ctre.match("0"sv));
static_assert("[[:xdigit:]]"_ctre.match("9"sv));
static_assert("[[:xdigit:]]"_ctre.match("a"sv));
static_assert("[[:xdigit:]]"_ctre.match("A"sv));
static_assert("[[:xdigit:]]"_ctre.match("f"sv));
static_assert(!"[[:xdigit:]]"_ctre.match("g"sv));
static_assert("abcdef"_ctre.match("abcdef"sv));
static_assert(!"abcdef"_ctre.match("abcGef"sv));
static_assert(""_ctre.match(""sv));
static_assert("(?:a|b|c)"_ctre.match("a"sv));
static_assert("(?:a|b|c)"_ctre.match("b"sv));
static_assert("(?:a|b|c)"_ctre.match("c"sv));
static_assert(!"(?:a|b|c)"_ctre.match("d"sv));
static_assert("(?:xy)?"_ctre.match("xy"sv));
static_assert("(?:xy)?"_ctre.match(""sv));
static_assert("(?:xy)?"_ctre.search("zxy"sv));
static_assert("(?:xy)?$"_ctre.search("zxy"sv));
static_assert(!"~(?:xy)?$"_ctre.match("zxy"sv));

static_assert("^abc"_ctre.match("abc"sv));
static_assert("^def$"_ctre.match("def"sv));
static_assert(!"a^"_ctre.match("a"sv));
static_assert(!"$a"_ctre.match("a"sv));

static_assert("a+?"_ctre.search("aaax"sv));
static_assert("a+?"_ctre.search("ax"sv));
static_assert(!"a+?"_ctre.match("x"sv));

static_assert("a++"_ctre.search("aaax"sv));
static_assert("a++"_ctre.search("ax"sv));
static_assert(!"a++"_ctre.match("x"sv));

static_assert("a*?x"_ctre.match("aaax"sv));
static_assert("a*?x"_ctre.match("ax"sv));
static_assert("a*?x"_ctre.match("x"sv));
static_assert(!"a*?x"_ctre.match("y"sv));

static_assert("a*+x"_ctre.match("aaax"sv));
static_assert("a*+x"_ctre.match("ax"sv));
static_assert("a*+x"_ctre.match("x"sv));
static_assert(!"a*+x"_ctre.match("y"sv));

static_assert(!"a*+ab"_ctre.match("aaab"sv));
static_assert(!"a++ab"_ctre.match("aaab"sv));
static_assert(!"a*+ab"_ctre.match("ab"sv));
static_assert(!"a++ab"_ctre.match("aab"sv));

static_assert("a*+ba"_ctre.match("aaba"sv));
static_assert("a++ba"_ctre.match("aaba"sv));
static_assert("a*+ba"_ctre.match("ba"sv));
static_assert("a++ba"_ctre.match("aba"sv));

static_assert("a{3,}x"_ctre.match("aaax"sv));
static_assert("a{3,}x"_ctre.match("aaaax"sv));

static_assert("^a{5}"_ctre.match("aaaaa"sv));
static_assert("^a{5}"_ctre.search("aaaaaa"sv));
static_assert(!"^a{5}$"_ctre.match("aaaaaa"sv));

static_assert("a*"_ctre.match("aaa"sv));
static_assert("a+"_ctre.match("aaa"sv));
static_assert("a*"_ctre.match(""sv));
static_assert("a+"_ctre.match("a"sv));

static_assert("a*$"_ctre.match("aaa"sv));
static_assert("a+$"_ctre.match("aaa"sv));
static_assert("a*$"_ctre.match(""sv));
static_assert("a+$"_ctre.match("a"sv));

static_assert("a*xb"_ctre.match("aaxb"sv));
static_assert("a+xb"_ctre.match("aaxb"sv));
static_assert("a*xb"_ctre.match("xb"sv));
static_assert("a+xb"_ctre.match("axb"sv));

static_assert("a*ab"_ctre.match("aaab"sv));
static_assert("a+ab"_ctre.match("aaab"sv));
static_assert("a*ab"_ctre.match("ab"sv));
static_assert("a+ab"_ctre.match("aab"sv));

static_assert(!"^a{2,5}ab"_ctre.match("aab"sv));
static_assert("^a{2,5}ab"_ctre.match("aaab"sv));
static_assert("^a{2,5}ab"_ctre.match("aaaab"sv));
static_assert("^a{2,5}ab"_ctre.match("aaaaab"sv));
static_assert("^a{2,5}ab"_ctre.match("aaaaaab"sv));
static_assert(!"^a{2,5}ab"_ctre.match("aaaaaaab"sv));

static_assert("(abc)"_ctre.match("abc"sv));
static_assert("(abc)+"_ctre.match("abc"sv));
static_assert("(abc)+"_ctre.match("abcabc"sv));
static_assert("(abc)+"_ctre.match("abcabcabc"sv));

static_assert("(?<name>abc)"_ctre.match("abc"sv));
static_assert("(?<name>abc)+"_ctre.match("abc"sv));
static_assert("(?<name>abc)+"_ctre.match("abcabc"sv));
static_assert("(?<name>abc)+"_ctre.match("abcabcabc"sv));
static_assert(!"(?<name>abc)+"_ctre.match("name"sv));

static_assert(std::string_view{"^([a-z]+)"_ctre.search("abcdef1234"sv)} == "abcdef"sv);
static_assert(std::string_view{"^([a-z]+)1234"_ctre.match("abcdef1234"sv)} == "abcdef1234"sv);
static_assert(std::string_view{"^([a-z])"_ctre.search("abcdef1234"sv)} == "a"sv);

static_assert("^([0-9]+[a-z]+)+"_ctre.match("123abc456def"sv));
static_assert("^([0-9]+[a-z]+)+"_ctre.match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert("^([0-9]+[a-z]+)+"_ctre.match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert("^([0-9]++[a-z]++)+"_ctre.match("123abc456def"sv));
static_assert("^([0-9]++[a-z]++)+"_ctre.match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert("^([0-9]++[a-z]++)+"_ctre.match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert("^([0-9]+?[a-z]+?)+"_ctre.search("123abc456def"sv));
static_assert("^([0-9]+?[a-z]+?)+"_ctre.search("123abc456def"sv).template get<1>() == "123a"sv);
static_assert("^([0-9]+?[a-z]+?)+"_ctre.search("123abc456def"sv).template get<0>() == "123a"sv);

static_assert("^([0-9]+?[a-z]++)+"_ctre.match("123abc456def"sv));
static_assert("^([0-9]+?[a-z]++)+"_ctre.match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert("^([0-9]+?[a-z]++)+"_ctre.match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert("^([a-z]{2})([a-z]{2})"_ctre.match("abcd"sv).template get<2>() == "cd"sv);
static_assert("^([a-z]{2})(?<second>[a-z]{2})"_ctre.match("abcd"sv).template get<decltype("second"_ctre_id)>() == "cd"sv);

static_assert("^([a-z]+):\\g{1}$"_ctre.match("abc:abc"sv));
static_assert("^([a-z]+):\\g{1}$"_ctre.match("abc:abc"sv).template get<1>() == "abc"sv);
static_assert(!"^([a-z]+):\\g{1}$"_ctre.match("abc:abce"sv));
static_assert("^([a-z]+)\\g{1}$"_ctre.match("abcabc"sv));
static_assert(!"^([a-z]+)\\g{1}$"_ctre.match("abcabcd"sv));
static_assert("^([a-z]+)\\g{-1}$"_ctre_syntax);
static_assert("^([a-z]+)\\g{-1}$"_ctre.match("abcabc"sv));
static_assert(!"^([a-z]+)\\g{-2}$"_ctre_syntax);
// TODO check for existence of named capture too

static_assert("^(?<text>[a-z]+):\\g{text}$"_ctre.match("abc:abc"sv));

static_assert("^abc$"_ctre.match("abc"sv));
static_assert("^abc$"_ctre.match(L"abc"sv));
static_assert("^abc$"_ctre.match(u8"abc"sv));
static_assert("^abc$"_ctre.match(u"abc"sv));
static_assert("^abc$"_ctre.match(U"abc"sv));

static_assert(R"(\(\))"_ctre.match("()"sv));
static_assert("\\[\\]"_ctre.match("[]"sv));
static_assert(R"(\[\])"_ctre.match("[]"sv));

static_assert(R"(\[([A-Z]*?)\])"_ctre.match("[]"sv));
static_assert(R"(\[([A-Z]*?)\])"_ctre.match("[URL]"sv));

static_assert(R"(\[([\s\S]*?)\]\(([\s\S]*?)\))"_ctre.match("[URL](https://cpp.fail/ctre)"));

static_assert("abc"_ctre.match("abc"));
static_assert("[_]"_ctre.match("_"));
static_assert("[()]"_ctre.match("("));
static_assert("[$]"_ctre.match("$"));
static_assert("[*]"_ctre.match("*"));
static_assert("[+]"_ctre.match("+"));
static_assert("[?]"_ctre.match("?"));
static_assert("[{}]"_ctre.match("{"));
static_assert("[(-)]"_ctre.match("("));
static_assert("[(-)]"_ctre.match(")"));

static_assert("[A-Z_a-z]"_ctre.match("a"));
static_assert("[A-Z_a-z]"_ctre.match("_"));
static_assert("[A-Z_a-z]"_ctre.match("Z"));
static_assert("[-]"_ctre.match("-"));
static_assert("[-x]"_ctre.match("x"));
// FIXME: due current limitation of LL1 grammar parser I can make this work "[x-]" without significant change in grammar
static_assert("<"_ctre.match("<"));
static_assert("(<)"_ctre.match("<"));
static_assert("(<>)"_ctre.match("<>"));
static_assert("(<>?)"_ctre.match("<"));
static_assert("(<?>)"_ctre.match(">"));
static_assert("()"_ctre.match(""));


static_assert(("[a-z]"_ctre >> "[0-9]"_ctre).match("a9"));
static_assert(("a"_ctre | "b"_ctre).match("a"));
static_assert(("a"_ctre | "b"_ctre).match("b"));
static_assert(!("a"_ctre | "b"_ctre).match("c"));

static_assert("((a)(b))"_ctre.match("ab"sv).template get<0>() == "ab"sv);
static_assert("((a)(b))"_ctre.match("ab"sv).template get<1>() == "ab"sv);
static_assert("((a)(b))"_ctre.match("ab"sv).template get<2>() == "a"sv);
static_assert("((a)(b))"_ctre.match("ab"sv).template get<3>() == "b"sv);

