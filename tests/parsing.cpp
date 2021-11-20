#include <ctre.hpp>

void empty_symbol() { }

using namespace ctre::test_literals;

#if !CTRE_CNTTP_COMPILER_CHECK
#define CTRE_TEST(pattern) (pattern ## _ctre_test)
#else

template <ctll::fixed_string input> constexpr bool test() {
	constexpr auto _input = input;
	return ctll::parser<ctre::pcre, _input>::template correct_with<>;
}

#define CTRE_TEST(pattern) test<pattern>()
#endif


// basics
static_assert(CTRE_TEST(""));
static_assert(CTRE_TEST("."));
static_assert(CTRE_TEST("a"));
static_assert(CTRE_TEST("ab"));
static_assert(CTRE_TEST("a|b"));
static_assert(CTRE_TEST("n"));

// repetation
static_assert(CTRE_TEST("x?")); // optional
static_assert(CTRE_TEST("x??")); // optional lazy
static_assert(CTRE_TEST("x?+")); // optional possessive
static_assert(CTRE_TEST("x*")); 
static_assert(CTRE_TEST("x*+"));
static_assert(CTRE_TEST("x*?"));
static_assert(CTRE_TEST("x+"));
static_assert(CTRE_TEST("x++"));
static_assert(CTRE_TEST("x+?"));
static_assert(CTRE_TEST("x{1}"));
static_assert(CTRE_TEST("x{1,4}"));
static_assert(CTRE_TEST("x{1,4}+"));
static_assert(CTRE_TEST("x{1,4}?"));
static_assert(CTRE_TEST("x{1,}"));
static_assert(CTRE_TEST("x{1,}+"));
static_assert(CTRE_TEST("x{1,}?"));

// character sets
static_assert(CTRE_TEST("[abc]")); // [abc]
static_assert(CTRE_TEST("[a-c]")); // [abc]
static_assert(CTRE_TEST("[a-ce-g]")); // [abcefg]
static_assert(CTRE_TEST("[xa-c]")); // [xabc]
static_assert(CTRE_TEST("[a-cy]")); // [abcy]
static_assert(CTRE_TEST("\\d")); // digit
static_assert(CTRE_TEST("\\D")); // non-digit
static_assert(CTRE_TEST("\\s")); // whitespace
static_assert(CTRE_TEST("\\S")); // non-whitespace
static_assert(CTRE_TEST("\\w")); // word character
static_assert(CTRE_TEST("\\W")); // non-word character

// assertions
static_assert(CTRE_TEST("^"));
static_assert(CTRE_TEST("$"));

// special characters
static_assert(CTRE_TEST("\\n")); // new line
static_assert(CTRE_TEST("\\r")); // return carriage
static_assert(CTRE_TEST("\\t")); // tab
static_assert(CTRE_TEST("\\0")); // NULL
static_assert(CTRE_TEST("\\xAB"));
static_assert(CTRE_TEST("\\x{A}"));
static_assert(CTRE_TEST("\\x{ABCDEF}"));
static_assert(not CTRE_TEST("\\x{ABCDEFG}"));

// named sets
static_assert(CTRE_TEST("[[:alpha:][:digit:]]"));
static_assert(CTRE_TEST("[^[:alpha:][:digit:]]"));
static_assert(CTRE_TEST("[[:^alpha:][:^digit:]]"));
static_assert(CTRE_TEST("[[:alpha:][:digit:]]+"));

// captures
static_assert(CTRE_TEST("<not capture>"));
static_assert(CTRE_TEST("(nonname)"));
static_assert(CTRE_TEST("(non<name>)"));
static_assert(CTRE_TEST("(?:noncapture)"));
static_assert(CTRE_TEST("(?:non<cap>ture)"));
static_assert(CTRE_TEST("(?<hello>:named)"));

// LIBC++ tests
static_assert(CTRE_TEST(""));
static_assert(CTRE_TEST("(.*).*"));
static_assert(CTRE_TEST("(?=^)b")); // positive lookahead
static_assert(CTRE_TEST("(a*)*"));
static_assert(CTRE_TEST("(ab)*c"));
static_assert(CTRE_TEST("(tour|to|t)+"));
static_assert(CTRE_TEST("(tour|to|tournament)+"));
static_assert(CTRE_TEST("(tour|to|tournament)+\ntourna"));
static_assert(CTRE_TEST("-.*,.*-"));

static_assert(CTRE_TEST("-\\(.*\\),\\1-"));
static_assert(CTRE_TEST("A[[:lower:]]B"));
static_assert(CTRE_TEST("A[^[:lower:]0-9]B"));
static_assert(CTRE_TEST("A[^[:lower:]]B"));
//static_assert(CTRE_TEST("Jeff(?!s\\b)")); // negative lookahead
//static_assert(CTRE_TEST("Jeff(?=s\\b)")); // positive lookahead
static_assert(CTRE_TEST("[\\n]+"));
static_assert(CTRE_TEST("[^\\0]*"));
static_assert(CTRE_TEST("[^\\W]"));
//static_assert(CTRE_TEST("[a[.ch.]z]"));
//static_assert(CTRE_TEST("[a[.hyphen.]z]"));
//static_assert(CTRE_TEST("[a[=M=]z]"));
static_assert(CTRE_TEST("[abfor]*"));
static_assert(CTRE_TEST("[ace1-9]*"));
static_assert(CTRE_TEST("[ace1-9]+"));
static_assert(CTRE_TEST("[ace1-9]\\{1,\\}"));
static_assert(CTRE_TEST("[ace1-9]{1,}"));
static_assert(CTRE_TEST("\\(.*\\).*"));
static_assert(CTRE_TEST("\\(a*\\)*"));
static_assert(CTRE_TEST("\\(ab\\)*c"));
static_assert(CTRE_TEST("\\d[\\W]k"));
static_assert(CTRE_TEST("^[a-f]$"));
static_assert(CTRE_TEST("^[a]$"));
static_assert(CTRE_TEST("^[ab]$"));
static_assert(CTRE_TEST("/"));
static_assert(CTRE_TEST("[/]"));
static_assert(CTRE_TEST("[\\[]"));
static_assert(CTRE_TEST("[\\[-a]"));
static_assert(CTRE_TEST("[\\[-\\[]"));
static_assert(CTRE_TEST("[/-/]"));
static_assert(CTRE_TEST("[!-/]"));
static_assert(CTRE_TEST("[|]"));
static_assert(CTRE_TEST("^abc"));
static_assert(CTRE_TEST("a"));
static_assert(CTRE_TEST("a(?=^)b")); // positive lookahead
static_assert(CTRE_TEST("a*"));
static_assert(CTRE_TEST("a*?"));
static_assert(CTRE_TEST("a.\\{3,5\\}c"));
static_assert(CTRE_TEST("a.c"));
static_assert(CTRE_TEST("a.{3,5}c"));
static_assert(CTRE_TEST("a\\{100000000000000000\\}"));
static_assert(CTRE_TEST("ab"));
static_assert(CTRE_TEST("ab*c"));
static_assert(CTRE_TEST("ab\\{3,5\\}c"));
static_assert(CTRE_TEST("abc$"));
static_assert(CTRE_TEST("ab{3,5}c"));
static_assert(CTRE_TEST("a{100000000000000000}"));
static_assert(CTRE_TEST("b*"));
static_assert(CTRE_TEST("ba"));
static_assert(CTRE_TEST("bc"));
static_assert(CTRE_TEST("cd((e)fg)hi"));
static_assert(CTRE_TEST("cd((e)fg)hi|(z)"));
static_assert(CTRE_TEST("cd\\(\\(e\\)fg\\)hi"));
static_assert(CTRE_TEST("cdefghi"));
static_assert(CTRE_TEST("q[^u]"));
static_assert(CTRE_TEST("tour\n\ntournament"));
static_assert(CTRE_TEST("tour\nto\ntournament"));
static_assert(CTRE_TEST("tour|to|tournament"));
static_assert(CTRE_TEST("z"));
static_assert(CTRE_TEST("=!"));
static_assert(CTRE_TEST("[=!]"));
static_assert(CTRE_TEST("(=!)"));
static_assert(CTRE_TEST("(?!test)"));
static_assert(CTRE_TEST("(?=test)"));
static_assert(CTRE_TEST("^(?=.*(.)\\g{1}+.*)[a-z]+"));
static_assert(CTRE_TEST("^(?=.*(a)\\g{1}.*)$"));

static_assert(CTRE_TEST("\\b"));
static_assert(CTRE_TEST("\\A"));
static_assert(!CTRE_TEST("[\\A]"));
static_assert(!CTRE_TEST("\\i")); // this is not existing backslash
static_assert(CTRE_TEST("(.*)\\1"));


