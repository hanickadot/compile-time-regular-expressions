#include <ctre.hpp>
#include <iostream>

int main() {
	using namespace std::string_view_literals;
	auto input = "123,456,768"sv;

	#if __cpp_nontype_template_parameter_class
	    for (auto match: ctre::range<"[0-9]++">(input)) {
	#else
		using namespace ctre::literals;
		for (auto match: ctre::range(input,"[0-9]++"_ctre)) {
	#endif
		std::cout << std::string_view(match) << "\n";
	}
}
