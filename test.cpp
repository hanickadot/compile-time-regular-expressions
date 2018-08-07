#include <ctre.hpp>
#include <iostream>
#include "ctre/type-stack.hpp"

template <typename T> void foo();

int main() {
	using namespace ctre::literals;
	
	using out = decltype(  
		ctre::augment_grammar<ctre::math_grammar>::rule(ctre::math_grammar::E(), ctre::term<'c'>())
	);

	
	auto x = "(x+y)*z+(4+(x+y)*z+4+(x+y)*z+4+(x+y))*z+4+(x+y)*z+4"_ctre;
	static_assert("(x+y)*z+(4+(x+y)*z+4+(x+y)*z+4+(x+y))*z+4+(x+y)*z+4"_ctre.value);
	printf("%s\n", x.value?"accept":"reject");
	printf("steps = %zu\n", x.steps);
	printf("variables = %u\n", x.subject.variables);
	printf("constants = %u\n", x.subject.constants);
	
	
}