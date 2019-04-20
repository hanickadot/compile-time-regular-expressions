#include <iostream>
//#include <ctfa/transformations/determinize2.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/transformations/determinize.hpp>
#include <ctfa/block.hpp>
#include <ctfa/basic/debug.hpp>


#include "debug.hpp"

int main() {
	static constexpr auto any = ctfa::star<ctfa::block::anything>;
	std::cout << "anything:\n";
	my_debug<any>();
	static constexpr auto hana = ctfa::block::string<'h','a','n','a'>;
	std::cout << "hana:\n";
	my_debug<hana>();
	static constexpr auto combined = ctfa::concat<any, hana, any>;
	std::cout << "combined:\n";
	my_debug<combined>();
	static constexpr auto determinized = ctfa::determinize<combined>;
	std::cout << "determinized:\n";
	my_debug<determinized>();
	static constexpr auto minimize = ctfa::minimize<determinized>;
	std::cout << "minimized:\n";
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

