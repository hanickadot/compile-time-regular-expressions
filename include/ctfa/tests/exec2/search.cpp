#include <ctfa/fa.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/reachability.hpp>
#include <ctfa/minimize.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

int main() {
	std::cout << "x+\n\n";
	static constexpr auto test1 = ctfa::concat<ctfa::plus<ctfa::block::anything>>;
	my_debug<test1>();
	
	std::cout << "x*\n\n";
	static constexpr auto test2 = ctfa::concat<ctfa::star<ctfa::block::anything>>;
	my_debug<test2>();
	
	std::cout << "x+a\n\n";
	static constexpr auto test3 = ctfa::concat<ctfa::plus<ctfa::block::anything>, ctfa::block::string<'a'>>;
	my_debug<test3>();
	
	std::cout << "x*a\n\n";
	static constexpr auto test5 = ctfa::concat<ctfa::star<ctfa::block::anything>, ctfa::block::string<'a'>>;
	my_debug<test5>();
	
	std::cout << "min(x*a)\n\n";
	static constexpr auto test6 = ctfa::minimize<test5>;
	my_debug<test6>();
	
	std::cout << "min(x*ax*)\n\n";
	static constexpr auto test7 = ctfa::minimize<ctfa::concat<ctfa::star<ctfa::block::anything>,  ctfa::block::string<'a'>,ctfa::star<ctfa::block::anything>>>;
	my_debug<test7>();
}