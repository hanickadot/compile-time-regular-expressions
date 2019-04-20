#include <iostream>
//#include <ctfa/transformations/determinize2.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/transformations/determinize.hpp>
#include <ctfa/block.hpp>
#include <ctfa/basic/debug.hpp>


#include "debug.hpp"

int main() {
	//static constexpr auto abcd = ctfa::block::string<'a','b','c','d'>;
	//static constexpr auto defgh = ctfa::block::string<'d','e','f','g','h'>;
	//static constexpr auto efghi = ctfa::block::string<'e','f','g','h','i'>;
	//
	//static constexpr auto a4 = ctfa::concat<ctfa::block::string<'a','a','a'>, ctfa::plus<ctfa::block::unit<'a'>>>;
	static constexpr auto any = ctfa::star<ctfa::block::anything>;
	
	//static constexpr auto combined = ctfa::concat<any, ctfa::alternative<abcd, defgh, efghi, a4>, any>;
	//static constexpr auto combined2 = ctfa::concat<ctfa::alternative<abcd, defgh, efghi, a4>>;
	
	static constexpr auto number = ctfa::plus<ctfa::block::range<'0','9'>>;
	
	static constexpr auto identifier = ctfa::plus<ctfa::block::range<'a','z'>>;
	
	static constexpr auto keyword_for = ctfa::block::string<'f','o','r'>;
	
	
	static constexpr auto combined = ctfa::alternative<number, identifier, keyword_for>;
	//
	std::cout << "combined:\n";
	my_debug<combined>();
	//ctfa::determinize_one<combined>::build();
	
	static constexpr auto determinized = ctfa::determinize_one<ctfa::remove_unneeded<combined>>::result;
	my_debug<determinized>();
	
	//static constexpr auto determinized2 = ctfa::determinize_one<ctfa::remove_unneeded<combined>>::result;
	my_debug<determinized>();
	
	static constexpr auto minimize = ctfa::minimize_one<determinized>::result;
	my_debug<minimize>();
	
	//static constexpr auto states = ctfa::determinize_one<combined>::all_dfa_states;
	//for (const auto & s: states) {
	//	std::cout << "<" << (&s - states.begin()) << "> ";
	//	if (s.states[0] == ctfa::start_state) {
	//		if (s.final) {
	//			std::cout << "<-> ";
	//		} else {
	//			std::cout << "--> ";
	//		}
	//	} else {
	//		if (s.final) {
	//			std::cout << " <- ";
	//		} else {
	//			std::cout << "    ";
	//		}
	//	}
	//	bool first = true;
	//	for (ctfa::state sub: s.states) {
	//		if (first) first = false;
	//		else std::cout << "+";
	//		std::cout << sub.id;
	//	}
	//	std::cout << "\n";
	//}
	//if (states.contains_new_states()) {
	//	std::cout << "(added states)\n";
	//}
}

