#include <ctre.hpp>
#include <iostream>

static constexpr auto pattern = ctll::fixed_string("(?<first>[0-9])[0-9]++");

int main() {
	using namespace std::string_view_literals;
	auto input = "123,456,768"sv;
	
	for (auto match: ctre::search_all<pattern>(input)) {
		
		if (match == "456") std::cout << "bingo: ";
		if (match != "768") std::cout << "bad: ";
		
		if ("456" == match) std::cout << "bingo: ";
		if ("768" != match) std::cout << "bad: ";
		
		auto capture = match.get<1>();
		
		if (capture == "456") std::cout << "bingo: ";
		if (capture != "768") std::cout << "bad: ";
		
		if ("456" == capture) std::cout << "bingo: ";
		if ("768" != capture) std::cout << "bad: ";
		
		[[maybe_unused]] const char * ptr = std::data(match);
		[[maybe_unused]] size_t length = std::size(match);
		
		[[maybe_unused]] const char * ptr2 = std::data(capture);
		[[maybe_unused]] size_t length2 = std::size(capture);
		
		//auto [ptr, err] = std::from_chars(std::data(match), std::data(match)+std::size(match), value);
		//if (err == std::errc{}) {
		//	std::cout << value << "\n";
		//} else {
		//	std::cout << "error\n";
		//}
	}
}
