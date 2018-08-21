#include <ctre.hpp>

using namespace ctre::test_literals;

// basics
static_assert(""_pcre_test);
static_assert("."_pcre_test);
static_assert("a"_pcre_test);
static_assert("ab"_pcre_test);
static_assert("a|b"_pcre_test);
static_assert("n"_pcre_test);

// repetation
static_assert("x?"_pcre_test); // optional
static_assert("x?+"_pcre_test); // optional possesive
static_assert("x??"_pcre_test); // optional lazy
static_assert("x*"_pcre_test); 
static_assert("x*+"_pcre_test);
static_assert("x*?"_pcre_test);
static_assert("x+"_pcre_test);
static_assert("x++"_pcre_test);
static_assert("x+?"_pcre_test);
static_assert("x{1}"_pcre_test);
static_assert("x{1,4}"_pcre_test);
static_assert("x{1,4}+"_pcre_test);
static_assert("x{1,4}?"_pcre_test);
static_assert("x{1,}"_pcre_test);
static_assert("x{1,}+"_pcre_test);
static_assert("x{1,}?"_pcre_test);

// character sets
static_assert("[abc]"_pcre_test); // [abc]
static_assert("[a-c]"_pcre_test); // [abc]
static_assert("[a-ce-g]"_pcre_test); // [abcefg]
static_assert("[xa-c]"_pcre_test); // [xabc]
static_assert("[a-cy]"_pcre_test); // [abcy]
static_assert("\\d"_pcre_test); // digit
static_assert("\\D"_pcre_test); // non-digit
static_assert("\\s"_pcre_test); // whitespace
static_assert("\\S"_pcre_test); // non-whitespace
static_assert("\\w"_pcre_test); // word character
static_assert("\\W"_pcre_test); // non-word character

// assertions
static_assert("^"_pcre_test);
static_assert("$"_pcre_test);

// special characters
static_assert("\\n"_pcre_test); // new line
static_assert("\\r"_pcre_test); // return carriage
static_assert("\\t"_pcre_test); // tab
static_assert("\\0"_pcre_test); // NULL
static_assert("\\xAB"_pcre_test);
static_assert("\\x{A}"_pcre_test);
static_assert("\\x{ABCDEF}"_pcre_test);
static_assert(not "\\x{ABCDEFG}"_pcre_test);

// named sets
static_assert("[[:alpha:][:digit:]]"_pcre_test);
static_assert("[^[:alpha:][:digit:]]"_pcre_test);
static_assert("[[:^alpha:][:^digit:]]"_pcre_test);
static_assert("[[:alpha:][:digit:]]+"_pcre_test);

// LIBC++ tests
static_assert(""_pcre_test);
static_assert("(.*).*"_pcre_test);
static_assert("(?=^)b"_pcre_test);
static_assert("(a*)*"_pcre_test);
static_assert("(ab)*c"_pcre_test);
static_assert("(tour|to|t)+"_pcre_test);
static_assert("(tour|to|tournament)+"_pcre_test);
static_assert("(tour|to|tournament)+\ntourna"_pcre_test);
static_assert("-.*,.*-"_pcre_test);

static_assert("-\\(.*\\),\\1-"_pcre_test);
static_assert("A[[:lower:]]B"_pcre_test);
static_assert("A[^[:lower:]0-9]B"_pcre_test);
static_assert("A[^[:lower:]]B"_pcre_test);
static_assert("Jeff(?!s\\b)"_pcre_test);
static_assert("Jeff(?=s\\b)"_pcre_test);
static_assert("[\\n]+"_pcre_test);
static_assert("[^\\0]*"_pcre_test);
static_assert("[^\\W]"_pcre_test);
//static_assert("[a[.ch.]z]"_pcre_test);
//static_assert("[a[.hyphen.]z]"_pcre_test);
//static_assert("[a[=M=]z]"_pcre_test);
static_assert("[abfor]*"_pcre_test);
static_assert("[ace1-9]*"_pcre_test);
static_assert("[ace1-9]+"_pcre_test);
static_assert("[ace1-9]\\{1,\\}"_pcre_test);
static_assert("[ace1-9]{1,}"_pcre_test);
static_assert("\\(.*\\).*"_pcre_test);
static_assert("\\(a*\\)*"_pcre_test);
static_assert("\\(ab\\)*c"_pcre_test);
static_assert("\\d[\\W]k"_pcre_test);
static_assert("^[a-f]$"_pcre_test);
static_assert("^[a]$"_pcre_test);
static_assert("^[ab]$"_pcre_test);
static_assert("^abc"_pcre_test);
static_assert("a"_pcre_test);
static_assert("a(?=^)b"_pcre_test);
static_assert("a*"_pcre_test);
static_assert("a*?"_pcre_test);
static_assert("a.\\{3,5\\}c"_pcre_test);
static_assert("a.c"_pcre_test);
static_assert("a.{3,5}c"_pcre_test);
static_assert("a\\{100000000000000000\\}"_pcre_test);
static_assert("ab"_pcre_test);
static_assert("ab*c"_pcre_test);
static_assert("ab\\{3,5\\}c"_pcre_test);
static_assert("abc$"_pcre_test);
static_assert("ab{3,5}c"_pcre_test);
static_assert("a{100000000000000000}"_pcre_test);
static_assert("b*"_pcre_test);
static_assert("ba"_pcre_test);
static_assert("bc"_pcre_test);
static_assert("cd((e)fg)hi"_pcre_test);
static_assert("cd((e)fg)hi|(z)"_pcre_test);
static_assert("cd\\(\\(e\\)fg\\)hi"_pcre_test);
static_assert("cdefghi"_pcre_test);
static_assert("q[^u]"_pcre_test);
static_assert("tour\n\ntournament"_pcre_test);
static_assert("tour\nto\ntournament"_pcre_test);
static_assert("tour|to|tournament"_pcre_test);
static_assert("z"_pcre_test);

