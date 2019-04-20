#include <iostream>

#include <ctfa/helper/interval.hpp>

void print_position(char32_t pos) {
	if (pos == std::numeric_limits<char32_t>::min()) std::cout << "MIN";
	else if (pos == std::numeric_limits<char32_t>::max()) std::cout << "MAX";
	else if (pos >= 32 && pos <= 126) std::cout <<  '\'' << static_cast<char>(pos) << '\'';
	else std::cout << pos;
}

int main() {
	ctfa::intervals<5, char32_t, int> i;
	i.insert_range(std::numeric_limits<char32_t>::min(), std::numeric_limits<char32_t>::max(), 2);
	i.insert_range('a','a',4);
	i.insert_range('b','b',5);
	// 1..5
	// 10..15
	
	for (auto i: i.buffer) {
		print_position(i.position);
		std::cout << " ";
		if (i.type == ctfa::point_type::start) std::cout << "+" << i.feature;
		else std::cout << "-" << i.feature;
		std::cout << " (" << i.counter << ")";
		std::cout << "\n";
	}
	
	std::cout << "\nmerge:\n";
	i.merge();
	
	for (auto i: i.buffer) {
		print_position(i.position);
		std::cout << " ";
		if (i.type == ctfa::point_type::start) std::cout << "+" << i.feature;
		else std::cout << "-" << i.feature;
		std::cout << "\n";
	}
	std::cout << "\nsplit:\n";
	
	i.split([](auto low, auto high, const auto & set){
		print_position(low);
		std::cout << " .. ";
		print_position(high);
		std::cout << "\n";
	});

	return 0;
}