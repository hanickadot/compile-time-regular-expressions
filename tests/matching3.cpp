#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

using namespace ctre::literals;
using namespace ctre::test_literals;
using namespace std::string_view_literals;

#if CTRE_CNTTP_COMPILER_CHECK

#define TEST_MATCH(id, pattern, subject) static_assert(ctre::match<pattern>(subject))

#define TEST_SEARCH(id, pattern, subject) static_assert(ctre::search<pattern>(subject))

#define TEST_NOT_MATCH(id, pattern, subject) static_assert(!ctre::match<pattern>(subject))

#define TEST_NOT_SEARCH(id, pattern, subject) static_assert(!ctre::search<pattern>(subject))

#else

#define TEST_MATCH(id, pattern, subject) static constexpr inline auto _ptn ## id = ctll::fixed_string(pattern); static_assert(ctre::re<_ptn ## id>().match(subject))

#define TEST_SEARCH(id, pattern, subject) static constexpr inline auto _ptn ## id = ctll::fixed_string(pattern); static_assert(ctre::re<_ptn ## id>().search(subject))

#define TEST_NOT_MATCH(id, pattern, subject) static constexpr inline auto _ptn ## id = ctll::fixed_string(pattern); static_assert(!ctre::re<_ptn ## id>().match(subject))

#define TEST_NOT_SEARCH(id, pattern, subject) static constexpr inline auto _ptn ## id = ctll::fixed_string(pattern); static_assert(!ctre::re<_ptn ## id>().search(subject))
#endif

TEST_MATCH(1, "blabla","blabla");
TEST_NOT_MATCH(2,"blabla","blabla2");

TEST_SEARCH(3, "", "abc");
TEST_MATCH(4, "abc", "abc");

TEST_MATCH(5, "a", "a");
TEST_SEARCH(6, "a", "abc");
TEST_SEARCH(7, "b", "abc");
TEST_NOT_MATCH(8, "^b", "abc");
TEST_NOT_MATCH(9, "b", "a");
TEST_MATCH(10, ".", "a");
TEST_SEARCH(11, ".", "abc");
TEST_MATCH(12, "[\\-]", "-");
TEST_MATCH(13, "[\\\\]", "\\");
TEST_MATCH(14, "[a-z]", "a");
TEST_MATCH(15, "[a-z]", "f");
TEST_MATCH(16, "[a-z]", "z");
TEST_NOT_MATCH(17, "[a-z]", "Z");
TEST_MATCH(18, "^[\\x30-\\x39]+?$", "123456789");
TEST_MATCH(19, "[a-z0-9]", "0");
TEST_NOT_MATCH(20, "[a-z0-9]", "A");
TEST_MATCH(21, "[[:xdigit:]]", "0");
TEST_MATCH(22, "[[:xdigit:]]", "9");
TEST_MATCH(22b, "[[:xdigit:]]", "a");
TEST_MATCH(23, "[[:xdigit:]]", "A");
TEST_MATCH(24, "[[:xdigit:]]", "f");
TEST_NOT_MATCH(25, "[[:xdigit:]]", "g");
TEST_MATCH(26, "abcdef", "abcdef");
TEST_NOT_MATCH(27, "abcdef", "abcGef");
TEST_MATCH(28, "", "");
TEST_MATCH(29, "(?:a|b|c)", "a");
TEST_MATCH(30, "(?:a|b|c)", "b");
TEST_MATCH(31, "(?:a|b|c)", "c");
TEST_NOT_MATCH(32, "(?:a|b|c)", "d");
TEST_MATCH(33, "(?:xy)?", "xy");
TEST_MATCH(34, "(?:xy)?", "");
TEST_SEARCH(35, "(?:xy)?", "zxy");
TEST_SEARCH(36, "(?:xy)?$", "zxy");
TEST_NOT_MATCH(37, "~(?:xy)?$", "zxy");

TEST_MATCH(38, "^abc", "abc");
TEST_MATCH(39, "^def$", "def");
TEST_NOT_MATCH(40, "a^", "a");
TEST_NOT_MATCH(41, "$a", "a");

TEST_SEARCH(42, "a+?", "aaax");
TEST_SEARCH(43, "a+?", "ax");
TEST_NOT_MATCH(44, "a+?", "x");

TEST_SEARCH(45, "a++", "aaax");
TEST_SEARCH(46, "a++", "ax");
TEST_NOT_MATCH(47, "a++", "x");

TEST_MATCH(48, "a*?x", "aaax");
TEST_MATCH(49, "a*?x", "ax");
TEST_MATCH(50, "a*?x", "x");
TEST_NOT_MATCH(51, "a*?x", "y");

TEST_MATCH(52, "a*+x", "aaax");
TEST_MATCH(53, "a*+x", "ax");
TEST_MATCH(54, "a*+x", "x");
TEST_NOT_MATCH(55, "a*+x", "y");

TEST_NOT_MATCH(56, "a*+ab", "aaab");
TEST_NOT_MATCH(57, "a++ab", "aaab");
TEST_NOT_MATCH(58, "a*+ab", "ab");
TEST_NOT_MATCH(59, "a++ab", "aab");

TEST_MATCH(60, "a*+ba", "aaba");
TEST_MATCH(61, "a++ba", "aaba");
TEST_MATCH(62, "a*+ba", "ba");
TEST_MATCH(63, "a++ba", "aba");

TEST_MATCH(64, "a{3,}x", "aaax");
TEST_MATCH(65, "a{3,}x", "aaaax");

TEST_MATCH(66, "^a{5}", "aaaaa");
TEST_SEARCH(67, "^a{5}", "aaaaaa");
TEST_NOT_MATCH(68, "^a{5}$", "aaaaaa");

TEST_MATCH(69, "a*", "aaa");
TEST_MATCH(70, "a+", "aaa");
TEST_MATCH(71, "a*", "");
TEST_MATCH(72, "a+", "a");

TEST_MATCH(73, "a*$", "aaa");
TEST_MATCH(74, "a+$", "aaa");
TEST_MATCH(75, "a*$", "");
TEST_MATCH(76, "a+$", "a");

TEST_MATCH(77, "a*xb", "aaxb");
TEST_MATCH(78, "a+xb", "aaxb");
TEST_MATCH(79, "a*xb", "xb");
TEST_MATCH(80, "a+xb", "axb");

TEST_MATCH(81, "a*ab", "aaab");
TEST_MATCH(82, "a+ab", "aaab");
TEST_MATCH(83, "a*ab", "ab");
TEST_MATCH(84, "a+ab", "aab");

TEST_NOT_MATCH(85, "^a{2,5}ab", "aab");
TEST_MATCH(86, "^a{2,5}ab", "aaab");
TEST_MATCH(87, "^a{2,5}ab", "aaaab");
TEST_MATCH(88, "^a{2,5}ab", "aaaaab");
TEST_MATCH(89, "^a{2,5}ab", "aaaaaab");
TEST_NOT_MATCH(90, "^a{2,5}ab", "aaaaaaab");

TEST_MATCH(91, "(abc)", "abc");
TEST_MATCH(92, "(abc)+", "abc");
TEST_MATCH(93, "(abc)+", "abcabc");
TEST_MATCH(94, "(abc)+", "abcabcabc");

TEST_MATCH(95, "(?<name>abc)", "abc");
TEST_MATCH(96, "(?<name>abc)+", "abc");
TEST_MATCH(97, "(?<name>abc)+", "abcabc");
TEST_MATCH(98, "(?<name>abc)+", "abcabcabc");
TEST_NOT_MATCH(99, "(?<name>abc)+", "name");





TEST_MATCH(101, "^([0-9]+[a-z]+)+", "123abc456def");



TEST_MATCH(102, "^([0-9]++[a-z]++)+", "123abc456def");



TEST_SEARCH(103, "^([0-9]+?[a-z]+?)+", "123abc456def");



TEST_MATCH(104, "^([0-9]+?[a-z]++)+", "123abc456def");






TEST_MATCH(105, "^([a-z]+):\\g{1}$", "abc:abc"sv);

TEST_NOT_MATCH(106, "^([a-z]+):\\g{1}$", "abc:abce"sv);
TEST_MATCH(107, "^([a-z]+)\\g{1}$", "abcabc"sv);
TEST_NOT_MATCH(108, "^([a-z]+)\\g{1}$", "abcabcd"sv);

TEST_MATCH(109, "^([a-z]+)\\g{-1}$", "abcabc"sv);


TEST_MATCH(110, "^(?<text>[a-z]+):\\g{text}$", "abc:abc"sv);

TEST_MATCH(111, "^abc$", "abc");






TEST_MATCH(112, "\\[\\]", "[]");

TEST_MATCH(113, "a+(?!b)", "aaaaaa");
TEST_MATCH(114, "a+(?!b).", "aaaaaac");
TEST_MATCH(115, "a+(?=b).", "aaaaaab");

TEST_NOT_MATCH(116, "a++a", "aaa"sv);
TEST_MATCH(117, "a++a*+", "aaa"sv);

TEST_MATCH(118, "^(\\w+)", "WORD");


TEST_MATCH(203, "(?=foo)foo", "foo");
TEST_MATCH(204, "(?!notfoo)foo", "foo");

TEST_MATCH(205, "ab(?<=ab)foo", "abfoo");
TEST_MATCH(206, "ab(?<!foo)foo", "abfoo");

// modes
TEST_MATCH(207, "(?i)AB(?c)AB", "abAB");

// bug

TEST_MATCH(210, "x+?", "x");
TEST_MATCH(211, "x+", "x");
TEST_MATCH(212, "x+?", L"x");
TEST_MATCH(213, "x+", L"x");
TEST_MATCH(214, "x+?", u8"x");
TEST_MATCH(215, "x+", u8"x");
TEST_SEARCH(216, "(?<!a)b", "xb");
TEST_SEARCH(217, "(?<!a)b", L"xb");
TEST_SEARCH(218, "(?<!a)b", u8"xb");
// TODO need to check this out...
//TEST_NOT_SEARCH(219, u8"(?<!č)b", u8"čb");
//TEST_SEARCH(220, u8"(?<=č)b", u8"čb");
TEST_SEARCH(221, u8"(?<!ě)b", u8"čb");
TEST_NOT_SEARCH(222, u8"(?<=ě)b", u8"čb");

//atomic groups (yet unsupported)
TEST_MATCH(223, "(?>abc)?", "abc");
TEST_MATCH(224, "(?>abc)?", "");



