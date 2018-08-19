#include <ctre.hpp>
#include <iostream>
#include "ctre/type-stack.hpp"

template <typename T> void foo();

template <typename Expr> void info(Expr x) {
	printf("%s\n", x.value?"accept":"reject");
	printf("steps = %zu\n", x.steps);
	printf("variables = %zu\n", x.subject.variables);
	printf("constants = %u\n", x.subject.constants);
	x.subject.print([](auto t){
		printf("<%c> ",t.value);
	});
	printf("\n\n");
}

int main() {
	using namespace ctre::literals;
	
	using out = decltype(  
		ctre::augment_grammar<ctre::math_grammar>::rule(ctre::math_grammar::E(), ctre::term<'c'>())
	);

	info("(x+y+z)*(x+y+a)"_ctre);
	info("(x+y+z)*(x+y+a)+(x+y+z)*(x+y)"_ctre);
	info("(x+y+z)*(x+y+a)+(x+y+z)*(x+y)+(x+y+z)*(x+y)"_ctre);
	info("(x+y+z)*(x+y+a)+(x+y+z)*(x+y)+(x+y+z)*(x+y)+(x+y+z)*(x+y)"_ctre);
	
	
}