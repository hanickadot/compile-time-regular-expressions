#include <iostream>
#include <ctfa/transformation.hpp>
#include <ctfa/transformations/determinize.hpp>
#include <ctfa/block.hpp>
#include <ctfa/basic/debug.hpp>

#include "debug.hpp"



int main() {
	static constexpr auto xyz = ctfa::star<ctfa::block::anything>;
	std::cout << "anything:\n";
	my_debug<xyz>();
	static constexpr auto abc = ctfa::block::string<'a','b','c'>;
	std::cout << "DD:\n";
	my_debug<abc>();
	static constexpr auto combined = ctfa::concat<xyz, abc, xyz>;
	//
	std::cout << "combined:\n";
	my_debug<combined>();
	//ctfa::determinize_one<combined>::build();
	


	static constexpr auto determinized = ctfa::determinize_one<combined>::result;
	
	//static_assert(min = star);
	//my_debug<combined>();
	std::cout << "determinized:\n";
	my_debug<determinized>();
	
	static constexpr auto unneeded = ctfa::remove_unneeded<determinized>;
	
	
	std::cout << "remove_unneeded:\n";
	my_debug<unneeded>();
	
	static constexpr auto minimize = ctfa::minimize_one<determinized>::result;
	
	std::cout << "minimized:\n";
	my_debug<minimize>();


}