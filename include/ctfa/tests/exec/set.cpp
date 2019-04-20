#include <iostream>
#include <ctfa/helper/interval.hpp>

//#include "debug.hpp"

int main() {
	//ctfa::intervals<5, char32_t, int> i;
	//i.insert_range(0, 0, 1);
	//i.insert_range(4294967295ull, 4294967295ull, 5);
	//i.insert_range(0, 'C', 2);
	//i.insert_range('E', 4294967295ull, 4);
	//i.insert_range('D', 'D', 3);
	//
	//i.insert_range(0, 10, 1);
	//i.insert_range(5, 15, 2);
	
	ctfa::intervals<3, int, int> i;
	//i.insert_range(0, 10, 1);
	//i.insert_range(5, 15, 2);
	
	i.insert_range(0, 30, 1);
	i.insert_range(10, 20, 2);
	i.insert_range(25, 30, 1);
	
	auto ret = i.split([](char32_t low, char32_t high, const auto & f){
		std::cout << (uint64_t)low << " .. " << (uint64_t)high << " = ";
		for (const auto & x: f) {
			std::cout << int(x) << " ";
		}
		std::cout << "\n";
	});
	//std::cout << "\n";
	std::cout << "ret = " << ret << "\n";
	
	//static_assert(min = star);
	//my_debug<combined>();
	//my_debug<determinized>();
}