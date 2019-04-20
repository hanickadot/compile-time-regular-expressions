#include <ctfa/fa.hpp>
#include <ctfa/reachability.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

int main() {
	static constexpr auto a = ctfa::finite_automaton<3,1>{
		{
			ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
			ctfa::transition{ctfa::state{0}, ctfa::state{3}, ctfa::matcher::unit<'x'>},
			ctfa::transition{ctfa::state{2}, ctfa::state{1}, ctfa::matcher::unit<'b'>}
		}, {
			ctfa::state{1}
		}
	};
	my_debug<a>();
	
	static constexpr auto b = ctfa::state_helper<a>::known_states<ctfa::reachability>;
	static constexpr auto c = ctfa::remove_unreachable<a>;
	my_debug<c>();
	//identify_value<b> x;
	//my_debug<b>();
}