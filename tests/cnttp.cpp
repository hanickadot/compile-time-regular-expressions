#include <iostream>
#include <ctre.hpp>
#include <string_view>
#include <optional>


struct name { std::string_view first, family; };

std::optional<name> extract(std::string_view sv) noexcept {
	#if __cpp_nontype_template_parameter_class
	if (auto [re,f,l] = ctre::match<"([A-Za-z]+?),([A-Za-z]+?)">(sv); re) {
	#else
	using namespace ctre::literals;
	if (auto [re,f,l] = "([A-Za-z]+?),([A-Za-z]+?)"_ctre.match(sv); re) {
	#endif
		return name{f,l};
	} else {
		return std::nullopt;
	}
}

int main() {
	
}