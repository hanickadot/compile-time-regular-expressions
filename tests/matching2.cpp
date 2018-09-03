#include <ctre.hpp>
#include <string_view>

using namespace ctre::literals;
using namespace ctre::test_literals;
using namespace std::string_view_literals;

static_assert(""_pcre.match("abc"sv));
static_assert("abc"_pcre.match("abc"sv));

static_assert("a"_pcre.match("a"sv));
static_assert("a"_pcre.match("abc"sv));
static_assert("b"_pcre.match("abc"sv));
static_assert(!"^b"_pcre.match("abc"sv));
static_assert(!"b"_pcre.match("a"sv));
static_assert("."_pcre.match("a"sv));
static_assert("."_pcre.match("abc"sv));
static_assert("[a-z]"_pcre.match("a"sv));
static_assert("[a-z]"_pcre.match("f"sv));
static_assert("[a-z]"_pcre.match("z"sv));
static_assert(!"[a-z]"_pcre.match("Z"sv));
static_assert("^[\\x30-\\x39]+?$"_pcre.match("123456789"sv));
static_assert("[a-z0-9]"_pcre.match("0"sv));
static_assert(!"[a-z0-9]"_pcre.match("A"sv));
static_assert("[[:xdigit:]]"_pcre.match("0"sv));
static_assert("[[:xdigit:]]"_pcre.match("9"sv));
static_assert("[[:xdigit:]]"_pcre.match("a"sv));
static_assert("[[:xdigit:]]"_pcre.match("A"sv));
static_assert("[[:xdigit:]]"_pcre.match("f"sv));
static_assert(!"[[:xdigit:]]"_pcre.match("g"sv));
static_assert("abcdef"_pcre.match("abcdef"sv));
static_assert(!"abcdef"_pcre.match("abcGef"sv));
static_assert(""_pcre.match(""sv));
static_assert("(?:a|b|c)"_pcre.match("a"sv));
static_assert("(?:a|b|c)"_pcre.match("b"sv));
static_assert("(?:a|b|c)"_pcre.match("c"sv));
static_assert(!"(?:a|b|c)"_pcre.match("d"sv));
static_assert("(?:xy)?"_pcre.match("xy"sv));
static_assert("(?:xy)?"_pcre.match(""sv));
static_assert("(?:xy)?"_pcre.match("zxy"sv));
static_assert("(?:xy)?$"_pcre.match("zxy"sv));
static_assert(!"~(?:xy)?$"_pcre.match("zxy"sv));

static_assert("^abc"_pcre.match("abc"sv));
static_assert("^def$"_pcre.match("def"sv));
static_assert(!"a^"_pcre.match("a"sv));
static_assert(!"$a"_pcre.match("a"sv));

static_assert("a+?"_pcre.match("aaax"sv));
static_assert("a+?"_pcre.match("ax"sv));
static_assert(!"a+?"_pcre.match("x"sv));

static_assert("a++"_pcre.match("aaax"sv));
static_assert("a++"_pcre.match("ax"sv));
static_assert(!"a++"_pcre.match("x"sv));

static_assert("a*?x"_pcre.match("aaax"sv));
static_assert("a*?x"_pcre.match("ax"sv));
static_assert("a*?x"_pcre.match("x"sv));
static_assert(!"a*?x"_pcre.match("y"sv));

static_assert("a*+x"_pcre.match("aaax"sv));
static_assert("a*+x"_pcre.match("ax"sv));
static_assert("a*+x"_pcre.match("x"sv));
static_assert(!"a*+x"_pcre.match("y"sv));

static_assert(!"a*+ab"_pcre.match("aaab"sv));
static_assert(!"a++ab"_pcre.match("aaab"sv));
static_assert(!"a*+ab"_pcre.match("ab"sv));
static_assert(!"a++ab"_pcre.match("aab"sv));

static_assert("a*+ba"_pcre.match("aaba"sv));
static_assert("a++ba"_pcre.match("aaba"sv));
static_assert("a*+ba"_pcre.match("ba"sv));
static_assert("a++ba"_pcre.match("aba"sv));

static_assert("a{3,}x"_pcre.match("aaax"sv));
static_assert("a{3,}x"_pcre.match("aaaax"sv));

static_assert("^a{5}"_pcre.match("aaaaa"sv));
static_assert("^a{5}"_pcre.match("aaaaaa"sv));
static_assert(!"^a{5}$"_pcre.match("aaaaaa"sv));

static_assert("a*"_pcre.match("aaa"sv));
static_assert("a+"_pcre.match("aaa"sv));
static_assert("a*"_pcre.match(""sv));
static_assert("a+"_pcre.match("a"sv));

static_assert("a*$"_pcre.match("aaa"sv));
static_assert("a+$"_pcre.match("aaa"sv));
static_assert("a*$"_pcre.match(""sv));
static_assert("a+$"_pcre.match("a"sv));

static_assert("a*xb"_pcre.match("aaxb"sv));
static_assert("a+xb"_pcre.match("aaxb"sv));
static_assert("a*xb"_pcre.match("xb"sv));
static_assert("a+xb"_pcre.match("axb"sv));

static_assert("a*ab"_pcre.match("aaab"sv));
static_assert("a+ab"_pcre.match("aaab"sv));
static_assert("a*ab"_pcre.match("ab"sv));
static_assert("a+ab"_pcre.match("aab"sv));

static_assert(!"^a{2,5}ab"_pcre.match("aab"sv));
static_assert("^a{2,5}ab"_pcre.match("aaab"sv));
static_assert("^a{2,5}ab"_pcre.match("aaaab"sv));
static_assert("^a{2,5}ab"_pcre.match("aaaaab"sv));
static_assert("^a{2,5}ab"_pcre.match("aaaaaab"sv));
static_assert(!"^a{2,5}ab"_pcre.match("aaaaaaab"sv));

static_assert("(abc)"_pcre.match("abc"sv));
static_assert("(abc)+"_pcre.match("abc"sv));
static_assert("(abc)+"_pcre.match("abcabc"sv));
static_assert("(abc)+"_pcre.match("abcabcabc"sv));

static_assert("(?<name>abc)"_pcre.match("abc"sv));
static_assert("(?<name>abc)+"_pcre.match("abc"sv));
static_assert("(?<name>abc)+"_pcre.match("abcabc"sv));
static_assert("(?<name>abc)+"_pcre.match("abcabcabc"sv));
static_assert(!"(?<name>abc)+"_pcre.match("name"sv));

static_assert(std::string_view{"^([a-z]+)"_pcre.match("abcdef1234"sv)} == "abcdef"sv);
static_assert(std::string_view{"^([a-z]+)1234"_pcre.match("abcdef1234"sv)} == "abcdef1234"sv);
static_assert(std::string_view{"^([a-z])"_pcre.match("abcdef1234"sv)} == "a"sv);

static_assert("^([0-9]+[a-z]+)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]+[a-z]+)+"_pcre.match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert("^([0-9]+[a-z]+)+"_pcre.match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert("^([0-9]++[a-z]++)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]++[a-z]++)+"_pcre.match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert("^([0-9]++[a-z]++)+"_pcre.match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert("^([0-9]+?[a-z]+?)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]+?[a-z]+?)+"_pcre.match("123abc456def"sv).template get<1>() == "123a"sv);
static_assert("^([0-9]+?[a-z]+?)+"_pcre.match("123abc456def"sv).template get<0>() == "123a"sv);

static_assert("^([0-9]+?[a-z]++)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]+?[a-z]++)+"_pcre.match("123abc456def"sv).template get<1>() == "456def"sv);
static_assert("^([0-9]+?[a-z]++)+"_pcre.match("123abc456def"sv).template get<0>() == "123abc456def"sv);

static_assert("^([a-z]{2})([a-z]{2})"_pcre.match("abcd"sv).template get<2>() == "cd"sv);
static_assert("^([a-z]{2})(?<second>[a-z]{2})"_pcre.match("abcd"sv).template get<decltype("second"_ctre_id)>() == "cd"sv);

static_assert("^([a-z]+):\\g{1}$"_pcre.match("abc:abc"sv));
static_assert("^([a-z]+):\\g{1}$"_pcre.match("abc:abc"sv).template get<1>() == "abc"sv);
static_assert(!"^([a-z]+):\\g{1}$"_pcre.match("abc:abce"sv));
static_assert("^([a-z]+)\\g{1}$"_pcre.match("abcabc"sv));
static_assert(!"^([a-z]+)\\g{1}$"_pcre.match("abcabcd"sv));
static_assert("^([a-z]+)\\g{-1}$"_pcre_syntax);
static_assert("^([a-z]+)\\g{-1}$"_pcre.match("abcabc"sv));
static_assert(!"^([a-z]+)\\g{-2}$"_pcre_syntax);
// TODO check for existence of named capture too

static_assert("^(?<text>[a-z]+):\\g{text}$"_pcre.match("abc:abc"sv));

static_assert("^abc$"_pcre.match("abc"sv));
static_assert("^abc$"_pcre.match(L"abc"sv));
static_assert("^abc$"_pcre.match(u8"abc"sv));
static_assert("^abc$"_pcre.match(u"abc"sv));
static_assert("^abc$"_pcre.match(U"abc"sv));
