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
	printf("%s\n", x.value?"accept":"reject");
	printf("steps = %zu\n", x.steps);
	//printf("variables = %zu\n", x.subject.variables);
	//printf("constants = %u\n", x.subject.constants);
	//x.subject.print([](auto t){
	//	printf("<%c> ",t.value);
	//});
	printf("\n\n");
}

int main() {
	using namespace ctre::literals;
	
	static_assert( ""_pcre_test);
	static_assert( "^"_pcre_test);
	static_assert( "$"_pcre_test);
	static_assert( "^$"_pcre_test);
	static_assert( "a{1}"_pcre_test);
	static_assert( "a{1,}"_pcre_test);
	static_assert( "a{1,2}"_pcre_test);
	"a{,2}"_ctre;
	static_assert( "a{10}"_pcre_test);
	static_assert( "a{10,}"_pcre_test);
	static_assert( "(a{10,20})+"_pcre_test);
	static_assert( "[abcd]+"_pcre_test);
	static_assert( "(X|[abc]*)+"_pcre_test);
	static_assert( "a+"_pcre_test);
	
	
}