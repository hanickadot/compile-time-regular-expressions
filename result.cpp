#include <ctre.hpp>
#include <cstdio>
#include <string_view>
#include <iostream>

int main() {
	using namespace ctre::literals;
	
	std::string_view sv = "test";
	if (auto [a] = ctre::regex_results<std::string_view::iterator>(sv.begin(), sv.end(), true); a) {
		std::cout << std::string_view{a} << "\n";
		//auto b = ctre::regex_results<std::string_view::iterator>(sv.begin(), sv.end(), true);
		//std::cout << b.get<decltype("testx"_ctre_id)>() << "\n";
	}
}