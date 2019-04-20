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
	//debug_minimize_tmp(fa);
}

int main() {
	static constexpr auto a = ctfa::finite_automaton<4,1>{
		{
			ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
			ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'c'>},
			ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'c'>},
		}, {
			ctfa::state{3}
		}
	};
	my_debug<a>();
	
	static constexpr auto c = ctfa::minimize<a>;
	my_debug<c>();

	static constexpr auto test2 = ctfa::finite_automaton<5, 3>{
		{
			ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
			ctfa::transition{ctfa::state{1}, ctfa::state{5}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
			ctfa::transition{ctfa::state{2}, ctfa::state{4}, ctfa::matcher::unit<'b'>}
			//ctfa::transition{ctfa::state{5}, ctfa::state{5}, ctfa::condition::unit<'a'>()},
		},
		{ctfa::state{3}, ctfa::state{4}, ctfa::state{5}}
	};
	
	my_debug<test2>();
	
	static constexpr auto test2min = ctfa::minimize<test2>;
	my_debug<test2min>();
	
	std::cout << "hello:\n";
	
	static constexpr auto fa1 = ctfa::finite_automaton<2,1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{0}, ctfa::matcher::unit<'b'>}
	},{
		ctfa::state{1}
	}};

	static constexpr auto fa2 = ctfa::finite_automaton<2,1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'c'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{0}, ctfa::matcher::unit<'d'>}
	},{
		ctfa::state{1}
	}};
	
	static constexpr auto x = ctfa::concat<fa1, fa2>;
	my_debug<x>();
	
	
	//static constexpr auto x2 = ctfa::star<x>;
	//my_debug<x2>();
	//
	my_debug<ctfa::minimize<x>>();
}