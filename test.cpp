#include <ctre.hpp>
#include <iostream>
#include "ctre/type-stack.hpp"

int main() {
	using namespace ctre::literals;
	
	auto x = "((x+y)*z+x+(x+y)*z+x+(x+y)*z+x)*((x+y)*z+x+(x+y)*z+x+(x+y)*z+x)"_ctre;
	std::cout << (x.value?"accept":"reject") << std::endl;
	std::cout << "steps = " << x.steps << std::endl;
	std::cout << "variables = " << x.subject.count << std::endl;
}