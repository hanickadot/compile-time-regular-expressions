#include <ctre.hpp>
#include <cstdio>
#include <string_view>
#include <iostream>

enum class type {
	nothing,
	matched,
	number,
	identifier
};

std::pair<type, std::string_view> match(std::string_view subject) {
	using namespace ctre::literals;
	
	if (auto [matched, identifier, number] = "(?:([a-z]++)|([0-9]++))"_ctre.match(subject); matched) {
		if (identifier) return {type::identifier, std::string_view{identifier}};
		else if (number) return {type::number, std::string_view{number}};
		else return {type::matched, std::string_view{matched}};
	}
	return {type::nothing, std::string_view{}};
}

int main(int argc, char ** argv) {
	if (argc >= 2) {
		using namespace std::string_view_literals;
		if (auto m = match(std::string_view(argv[1])); m.first == type::number) {
			std::cout << "number: " << m.second << "\n";
		} else if (m.first == type::identifier) {
			std::cout << "identifier: " << m.second << "\n";
		} else  if (m.first == type::matched) {
			std::cout << "matched: " << m.second << "\n";
		} else {
			std::cout << "not matched\n";
			return 1;
		}
		return 0;
	} else {
		std::cout << "missing input\n";
		return 2;
	}
	
}