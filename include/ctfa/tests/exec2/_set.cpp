#include <ctfa/fa.hpp>
#include <ctfa/reachability.hpp>
#include <ctfa/minimize.hpp>
#include <ctfa/determinize.hpp>
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
	//debug_minimize_tmp(fa);
}

int main() {
	static constexpr auto a = ctfa::finite_automaton<4,2>{
		{
			ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{1}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{2}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
		}, {
			ctfa::state{1},
			ctfa::state{2}
		}
	};
	my_debug<a>();
	
	std::cout << (ctfa::is_deterministic<a> ? "dfa\n" : "nfa\n");
	
	
	static constexpr auto b = ctfa::finite_automaton<5,3>{
		{
			ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
			ctfa::transition{ctfa::state{2}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{3}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
		}, {
			ctfa::state{1},
			ctfa::state{2},
			ctfa::state{3}
		}
	};
	my_debug<b>();
	
	std::cout << (ctfa::is_deterministic<b> ? "dfa\n" : "nfa\n");
}