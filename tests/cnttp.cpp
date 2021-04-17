#include <iostream>
#include <ctre.hpp>
#include <string_view>

struct name { bool has; std::string_view first, family; };

name extract(std::string_view sv) noexcept {
	#if CTRE_CNTTP_COMPILER_CHECK
	if (auto [re,f,l] = ctre::match<"([A-Za-z]+?),([A-Za-z]+?)">(sv); re) {
	#else
	using namespace ctre::literals;
	if (auto [re,f,l] = "([A-Za-z]+?),([A-Za-z]+?)"_ctre.match(sv); re) {
	#endif
		return name{true, f,l};
	} else {
		return name{false, std::string_view(), std::string_view()};
	}
}

int main() {
	
}