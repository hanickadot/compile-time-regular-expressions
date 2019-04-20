#include <ctfa/fa.hpp>
#include <ctfa/reachability.hpp>
#include <ctfa/minimize.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

template <typename T> struct is_minimize_tmp: std::false_type { };
template <typename T> struct is_minimize_tmp<ctfa::filtered_out<T>>: std::true_type { };

template <auto & fa> void debug_minimize() {
	//if constexpr (is_minimize_tmp<decltype(fa)>()) {
	//	debug_minimize_tmp(fa);
	//} else {
	//my_debug<fa>();
	//}
	debug_minimize_tmp(fa);
}

int main() {
	static constexpr auto test1 = ctfa::alternative<ctfa::block::string<'a','a','a'>, ctfa::block::string<'b','b','b'>>;
	my_debug<test1>();
	
	//static constexpr auto b = ctfa::state_helper<a>::known_states<ctfa::reachability>;
	static constexpr auto test1min = ctfa::minimize<test1>;
	my_debug<test1min>();
	//identify_value<c> x;
	//my_debug<b>();
}