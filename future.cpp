#include <ctre.hpp>
#include <optional>
#include <iostream>

std::string match(std::string_view sv) {
	if (auto match = ctre::match<"[a-z]+">(sv); match) {
		return match.to_string();
	} else {
		return "not_match";
	}
} 

int main() {
	std::cout << match("hello") << "\n";
}