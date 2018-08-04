#include <ctre.hpp>
#include <iostream>
#include "ctre/type-stack.hpp"

int main() {
	using namespace ctre::literals;
	
	auto x = "((x+y)*z+x+(x+y)*z+x+(x+y)*z+x)*((x+y)*z+x+(x+y)*z+x+(x+y)*z+x)"_ctre;
	printf("%s\n", x.value?"accept":"reject");
	printf("steps = %zu\n", x.steps);
	printf("variables = %u\n", x.subject.count);
}