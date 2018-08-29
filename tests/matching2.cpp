#include <ctre.hpp>
#include <string_view>

using namespace ctre::literals;
using namespace std::string_view_literals;

static_assert(""_pcre == "abc"sv);
static_assert("abc"_pcre == "abc"sv);

static_assert("a"_pcre == "a"sv);
static_assert("a"_pcre == "abc"sv);
static_assert("b"_pcre == "abc"sv);
static_assert("^b"_pcre != "abc"sv);
static_assert("b"_pcre != "a"sv);
static_assert("."_pcre == "a"sv);
static_assert("."_pcre == "abc"sv);
static_assert("[a-z]"_pcre == "a"sv);
static_assert("[a-z]"_pcre == "f"sv);
static_assert("[a-z]"_pcre == "z"sv);
static_assert("[a-z]"_pcre != "Z"sv);
static_assert("[a-z0-9]"_pcre == "0"sv);
static_assert("[a-z0-9]"_pcre != "A"sv);
static_assert("[[:xdigit:]]"_pcre == "0"sv);
static_assert("[[:xdigit:]]"_pcre == "9"sv);
static_assert("[[:xdigit:]]"_pcre == "a"sv);
static_assert("[[:xdigit:]]"_pcre == "A"sv);
static_assert("[[:xdigit:]]"_pcre == "f"sv);
static_assert("[[:xdigit:]]"_pcre != "g"sv);
static_assert("abcdef"_pcre == "abcdef"sv);
static_assert("abcdef"_pcre != "abcGef"sv);
static_assert(""_pcre == ""sv);
static_assert("(?:a|b|c)"_pcre == "a"sv);
static_assert("(?:a|b|c)"_pcre == "b"sv);
static_assert("(?:a|b|c)"_pcre == "c"sv);
static_assert("(?:a|b|c)"_pcre != "d"sv);
static_assert("(?:xy)?"_pcre == "xy"sv);
static_assert("(?:xy)?"_pcre == ""sv);
static_assert("(?:xy)?"_pcre == "zxy"sv);
static_assert("(?:xy)?$"_pcre == "zxy"sv);
static_assert("~(?:xy)?$"_pcre != "zxy"sv);

static_assert("^abc"_pcre == "abc"sv);
static_assert("^def$"_pcre == "def"sv);
static_assert("a^"_pcre != "a"sv);
static_assert("$a"_pcre != "a"sv);

static_assert("a+?"_pcre == "aaax"sv);
static_assert("a+?"_pcre == "ax"sv);
static_assert("a+?"_pcre != "x"sv);

static_assert("a++"_pcre == "aaax"sv);
static_assert("a++"_pcre == "ax"sv);
static_assert("a++"_pcre != "x"sv);

static_assert("a*?x"_pcre == "aaax"sv);
static_assert("a*?x"_pcre == "ax"sv);
static_assert("a*?x"_pcre == "x"sv);
static_assert("a*?x"_pcre != "y"sv);

static_assert("a*+x"_pcre == "aaax"sv);
static_assert("a*+x"_pcre == "ax"sv);
static_assert("a*+x"_pcre == "x"sv);
static_assert("a*+x"_pcre != "y"sv);

static_assert("a*+ab"_pcre != "aaab"sv);
static_assert("a++ab"_pcre != "aaab"sv);
static_assert("a*+ab"_pcre != "ab"sv);
static_assert("a++ab"_pcre != "aab"sv);

static_assert("a*+ba"_pcre == "aaba"sv);
static_assert("a++ba"_pcre == "aaba"sv);
static_assert("a*+ba"_pcre == "ba"sv);
static_assert("a++ba"_pcre == "aba"sv);

static_assert("a{3,}x"_pcre == "aaax"sv);
static_assert("a{3,}x"_pcre == "aaaax"sv);

static_assert("^a{5}"_pcre == "aaaaa"sv);
static_assert("^a{5}"_pcre == "aaaaaa"sv);
static_assert("^a{5}$"_pcre != "aaaaaa"sv);

static_assert("a*"_pcre == "aaa"sv);
static_assert("a+"_pcre == "aaa"sv);
static_assert("a*"_pcre == ""sv);
static_assert("a+"_pcre == "a"sv);

static_assert("a*$"_pcre == "aaa"sv);
static_assert("a+$"_pcre == "aaa"sv);
static_assert("a*$"_pcre == ""sv);
static_assert("a+$"_pcre == "a"sv);

static_assert("a*xb"_pcre == "aaxb"sv);
static_assert("a+xb"_pcre == "aaxb"sv);
static_assert("a*xb"_pcre == "xb"sv);
static_assert("a+xb"_pcre == "axb"sv);

static_assert("a*ab"_pcre == "aaab"sv);
static_assert("a+ab"_pcre == "aaab"sv);
static_assert("a*ab"_pcre == "ab"sv);
static_assert("a+ab"_pcre == "aab"sv);

static_assert("^a{2,5}ab"_pcre != "aab"sv);
static_assert("^a{2,5}ab"_pcre == "aaab"sv);
static_assert("^a{2,5}ab"_pcre == "aaaab"sv);
static_assert("^a{2,5}ab"_pcre == "aaaaab"sv);
static_assert("^a{2,5}ab"_pcre == "aaaaaab"sv);
static_assert("^a{2,5}ab"_pcre != "aaaaaaab"sv);

static_assert("(abc)"_pcre == "abc"sv);
static_assert("(abc)+"_pcre == "abc"sv);
static_assert("(abc)+"_pcre == "abcabc"sv);
static_assert("(abc)+"_pcre == "abcabcabc"sv);

static_assert("(?<name>abc)"_pcre == "abc"sv);
static_assert("(?<name>abc)+"_pcre == "abc"sv);
static_assert("(?<name>abc)+"_pcre == "abcabc"sv);
static_assert("(?<name>abc)+"_pcre == "abcabcabc"sv);
static_assert("(?<name>abc)+"_pcre != "name"sv);

static_assert(std::string_view{"^([a-z]+)"_pcre.match("abcdef1234"sv)} == "abcdef"sv);
static_assert(std::string_view{"^([a-z]+)1234"_pcre.match("abcdef1234"sv)} == "abcdef1234"sv);
static_assert(std::string_view{"^([a-z])"_pcre.match("abcdef1234"sv)} == "a"sv);

static_assert("^([0-9]+[a-z]+)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]+[a-z]+)+"_pcre.match("123abc456def"sv).template get<1>().to_view() == "456def"sv);
static_assert("^([0-9]+[a-z]+)+"_pcre.match("123abc456def"sv).template get<0>().to_view() == "123abc456def"sv);

static_assert("^([0-9]++[a-z]++)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]++[a-z]++)+"_pcre.match("123abc456def"sv).template get<1>().to_view() == "456def"sv);
static_assert("^([0-9]++[a-z]++)+"_pcre.match("123abc456def"sv).template get<0>().to_view() == "123abc456def"sv);

static_assert("^([0-9]+?[a-z]+?)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]+?[a-z]+?)+"_pcre.match("123abc456def"sv).template get<1>().to_view() == "123a"sv);
static_assert("^([0-9]+?[a-z]+?)+"_pcre.match("123abc456def"sv).template get<0>().to_view() == "123a"sv);

static_assert("^([0-9]+?[a-z]++)+"_pcre.match("123abc456def"sv));
static_assert("^([0-9]+?[a-z]++)+"_pcre.match("123abc456def"sv).template get<1>().to_view() == "456def"sv);
static_assert("^([0-9]+?[a-z]++)+"_pcre.match("123abc456def"sv).template get<0>().to_view() == "123abc456def"sv);

static_assert("^([a-z]{2})([a-z]{2})"_pcre.match("abcd"sv).template get<2>().to_view() == "cd"sv);
static_assert("^([a-z]{2})(?<second>[a-z]{2})"_pcre.match("abcd"sv).template get<decltype("second"_ctre_id)>().to_view() == "cd"sv);

static_assert("^([a-z]+):\\g{1}$"_pcre == "abc:abc"sv);
static_assert("^([a-z]+):\\g{1}$"_pcre.match("abc:abc"sv).template get<1>().to_view() == "abc"sv);
static_assert("^([a-z]+):\\g{1}$"_pcre != "abc:abce"sv);
static_assert("^([a-z]+)\\g{1}$"_pcre == "abcabc"sv);
static_assert("^([a-z]+)\\g{1}$"_pcre != "abcabcd"sv);

static_assert("^(?<text>[a-z]+):\\g{text}$"_pcre == "abc:abc"sv);



