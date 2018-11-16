#include <ctre.hpp>
#include <cstdio>
#include <string_view>
#include <iostream>


static constexpr ctre::pattern pattern = "(?:hello|cau|guten tag|ola)|(?:hello|cau|guten tag|ola)|(?:hello|cau|guten tag|ola)|(?:hello|cau|guten tag|ola)|(?:hello|cau|guten tag|ola)|(?:hello|cau|guten tag|ola)|(?:hello|cau|guten tag|ola)";

bool fnc(std::string_view view) {
	return ctre::match<pattern>(view);
}

int main() {
	std::cout << fnc("hello") << "\n";
}