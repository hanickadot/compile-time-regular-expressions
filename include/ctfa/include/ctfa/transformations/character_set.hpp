#ifndef CTFA__TRANSFORMATION__CHARACTER_SET__HPP
#define CTFA__TRANSFORMATION__CHARACTER_SET__HPP

#include "../basic/fa.hpp"
#include "epsilon.hpp"

namespace ctfa {

template <const auto & ... Fa> struct character_set_n {
	static constexpr auto build() {
		constexpr size_t transitions = (Fa.transitions.size() + ... + 0);

		finite_automaton<transitions, 1> output;
		
		// I can do this two push_backs because I know it's already sorted
		auto add = [&](const auto & current){
			for (const auto & t: current.transitions) {
				output.transitions.insert(transition{start_state, state{1}, t.cond}); 
			}
		};
		
		(add(Fa),...);
		
		output.mark_final(state{1});
		
		return output;
	}
	
	static constexpr auto result = build();
};

}

#endif
