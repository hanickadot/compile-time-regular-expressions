#include <ctre.hpp>
#include <iostream>
#include "ctre/type-stack.hpp"
#include "ctre/pcre.hpp"

template <typename Expr> void accept(Expr) {
	static_assert(std::declval<Expr>().value);
}

template <typename Expr> void reject(Expr) {
	static_assert(!std::declval<Expr>().value);
}

template <typename T> void foo();

template <typename Expr> void info(Expr x) {
	printf("%s\n", x?"accept":"reject");
	//printf("steps = %zu\n", x.steps);
	//printf("variables = %zu\n", x.subject.variables);
	//printf("constants = %u\n", x.subject.constants);
	//x.subject.print([](auto t){
	//	printf("<%c> ",t.value);
	//});
	printf("\n\n");
}

int main() {
	using namespace ctre::test_literals;
	
	static_assert(""_pcre_test);
	static_assert("(.*).*"_pcre_test);
	static_assert("(?=^)b"_pcre_test);
	static_assert("(a*)*"_pcre_test);
	static_assert("(ab)*c"_pcre_test);
	static_assert("(tour|to|t)+"_pcre_test);
	static_assert("(tour|to|tournament)+"_pcre_test);
	static_assert("(tour|to|tournament)+\ntourna"_pcre_test);
	static_assert("-(.*),\1-"_pcre_test);
	static_assert("-.*,.*-"_pcre_test);
	
	//static_assert("-\\(.*\\),\\1-"_pcre_test);
	//static_assert("A[[:lower:]]B"_pcre_test);
	//static_assert("A[^[:lower:]0-9]B"_pcre_test);
	//static_assert("A[^[:lower:]]B"_pcre_test);
	//static_assert("Jeff(?!s\\b)"_pcre_test);
	//static_assert("Jeff(?=s\\b)"_pcre_test);
	//static_assert("[\\n]+"_pcre_test);
	//static_assert("[^\\0]*"_pcre_test);
	//static_assert("[^\\W]"_pcre_test);
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
	static_assert("\\a"_pcre_test);
	static_assert("\\b"_pcre_test);
	//static_assert("\\d[\\W]k"_pcre_test);
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
	
	
	
}