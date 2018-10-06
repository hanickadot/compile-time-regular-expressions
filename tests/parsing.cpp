#include <ctre.hpp>

using namespace ctre::test_literals;

// basics
static_assert(""_ctre_test);
static_assert("."_ctre_test);
static_assert("a"_ctre_test);
static_assert("ab"_ctre_test);
static_assert("a|b"_ctre_test);
static_assert("n"_ctre_test);

// repetation
static_assert("x?"_ctre_test); // optional
static_assert("x??"_ctre_test); // optional lazy
static_assert("x*"_ctre_test); 
static_assert("x*+"_ctre_test);
static_assert("x*?"_ctre_test);
static_assert("x+"_ctre_test);
static_assert("x++"_ctre_test);
static_assert("x+?"_ctre_test);
static_assert("x{1}"_ctre_test);
static_assert("x{1,4}"_ctre_test);
static_assert("x{1,4}+"_ctre_test);
static_assert("x{1,4}?"_ctre_test);
static_assert("x{1,}"_ctre_test);
static_assert("x{1,}+"_ctre_test);
static_assert("x{1,}?"_ctre_test);

// character sets
static_assert("[abc]"_ctre_test); // [abc]
static_assert("[a-c]"_ctre_test); // [abc]
static_assert("[a-ce-g]"_ctre_test); // [abcefg]
static_assert("[xa-c]"_ctre_test); // [xabc]
static_assert("[a-cy]"_ctre_test); // [abcy]
static_assert("\\d"_ctre_test); // digit
static_assert("\\D"_ctre_test); // non-digit
static_assert("\\s"_ctre_test); // whitespace
static_assert("\\S"_ctre_test); // non-whitespace
static_assert("\\w"_ctre_test); // word character
static_assert("\\W"_ctre_test); // non-word character

// assertions
static_assert("^"_ctre_test);
static_assert("$"_ctre_test);

// special characters
static_assert("\\n"_ctre_test); // new line
static_assert("\\r"_ctre_test); // return carriage
static_assert("\\t"_ctre_test); // tab
static_assert("\\0"_ctre_test); // NULL
static_assert("\\xAB"_ctre_test);
static_assert("\\x{A}"_ctre_test);
static_assert("\\x{ABCDEF}"_ctre_test);
static_assert(not "\\x{ABCDEFG}"_ctre_test);

// named sets
static_assert("[[:alpha:][:digit:]]"_ctre_test);
static_assert("[^[:alpha:][:digit:]]"_ctre_test);
static_assert("[[:^alpha:][:^digit:]]"_ctre_test);
static_assert("[[:alpha:][:digit:]]+"_ctre_test);

// captures
static_assert("<not capture>"_ctre_test);
static_assert("(nonname)"_ctre_test);
static_assert("(non<name>)"_ctre_test);
static_assert("(?:noncapture)"_ctre_test);
static_assert("(?:non<cap>ture)"_ctre_test);
static_assert("(?<hello>:named)"_ctre_test);

// LIBC++ tests
static_assert(""_ctre_test);
static_assert("(.*).*"_ctre_test);
//static_assert("(?=^)b"_ctre_test); // positive lookahead
static_assert("(a*)*"_ctre_test);
static_assert("(ab)*c"_ctre_test);
static_assert("(tour|to|t)+"_ctre_test);
static_assert("(tour|to|tournament)+"_ctre_test);
static_assert("(tour|to|tournament)+\ntourna"_ctre_test);
static_assert("-.*,.*-"_ctre_test);

static_assert("-\\(.*\\),\\1-"_ctre_test);
static_assert("A[[:lower:]]B"_ctre_test);
static_assert("A[^[:lower:]0-9]B"_ctre_test);
static_assert("A[^[:lower:]]B"_ctre_test);
//static_assert("Jeff(?!s\\b)"_ctre_test); // negative lookahead
//static_assert("Jeff(?=s\\b)"_ctre_test); // positive lookahead
static_assert("[\\n]+"_ctre_test);
static_assert("[^\\0]*"_ctre_test);
static_assert("[^\\W]"_ctre_test);
//static_assert("[a[.ch.]z]"_ctre_test);
//static_assert("[a[.hyphen.]z]"_ctre_test);
//static_assert("[a[=M=]z]"_ctre_test);
static_assert("[abfor]*"_ctre_test);
static_assert("[ace1-9]*"_ctre_test);
static_assert("[ace1-9]+"_ctre_test);
static_assert("[ace1-9]\\{1,\\}"_ctre_test);
static_assert("[ace1-9]{1,}"_ctre_test);
static_assert("\\(.*\\).*"_ctre_test);
static_assert("\\(a*\\)*"_ctre_test);
static_assert("\\(ab\\)*c"_ctre_test);
static_assert("\\d[\\W]k"_ctre_test);
static_assert("^[a-f]$"_ctre_test);
static_assert("^[a]$"_ctre_test);
static_assert("^[ab]$"_ctre_test);
static_assert("^abc"_ctre_test);
static_assert("a"_ctre_test);
//static_assert("a(?=^)b"_ctre_test); // positive lookahead
static_assert("a*"_ctre_test);
static_assert("a*?"_ctre_test);
static_assert("a.\\{3,5\\}c"_ctre_test);
static_assert("a.c"_ctre_test);
static_assert("a.{3,5}c"_ctre_test);
static_assert("a\\{100000000000000000\\}"_ctre_test);
static_assert("ab"_ctre_test);
static_assert("ab*c"_ctre_test);
static_assert("ab\\{3,5\\}c"_ctre_test);
static_assert("abc$"_ctre_test);
static_assert("ab{3,5}c"_ctre_test);
static_assert("a{100000000000000000}"_ctre_test);
static_assert("b*"_ctre_test);
static_assert("ba"_ctre_test);
static_assert("bc"_ctre_test);
static_assert("cd((e)fg)hi"_ctre_test);
static_assert("cd((e)fg)hi|(z)"_ctre_test);
static_assert("cd\\(\\(e\\)fg\\)hi"_ctre_test);
static_assert("cdefghi"_ctre_test);
static_assert("q[^u]"_ctre_test);
static_assert("tour\n\ntournament"_ctre_test);
static_assert("tour\nto\ntournament"_ctre_test);
static_assert("tour|to|tournament"_ctre_test);
static_assert("z"_ctre_test);

