#include <ctre.hpp>
#include <iostream>

static constexpr auto pattern = ctll::fixed_string("([a-z]++),([a-z]++),([a-z]++),([a-z]++),([a-z]++)");

size_t total_size(std::string_view subject) {
	auto result = ctre::match<pattern>(subject);
	
	if (not result) return 0;
	
	size_t size = 0;
	size += std::string_view(result.get<1>()).size();
	size += std::string_view(result.get<2>()).size();
	size += std::string_view(result.get<3>()).size();
	size += std::string_view(result.get<4>()).size();
	size += std::string_view(result.get<5>()).size();

	return size;
}
