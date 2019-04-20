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
	static constexpr auto a = ctfa::concat<ctfa::plus<ctfa::block::unit<'a'>>, ctfa::block::unit<'b'>>;
	my_debug<a>();
	
	static constexpr auto b = ctfa::concat<ctfa::block::unit<'b'>, ctfa::block::unit<'a'>, ctfa::block::unit<'a'>, ctfa::block::unit<'b'>>;
	my_debug<b>();
	
	std::cout << "alternative:\n";
	static constexpr auto a_or_b = ctfa::alternative<a, b>;
	my_debug<a_or_b>();
	
	std::cout << "minimize:\n";
	
	static constexpr auto min = ctfa::minimize<a_or_b>;
	my_debug<min>();
	
}