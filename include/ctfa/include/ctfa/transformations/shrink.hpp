#ifndef CTFA__TRANSFORMATION__SHRINK__HPP
#define CTFA__TRANSFORMATION__SHRINK__HPP

#include "../basic/fa.hpp"

namespace ctfa {

template <const auto & Fa> struct shrink_one {
	static constexpr auto build() {
		constexpr size_t transitions = Fa.transitions.size();
		constexpr size_t final_states = Fa.final_states.size();
		
		ctfa::finite_automaton<transitions, final_states> out;
		
		for (const auto & t: Fa.transitions) {
			out.transitions.push_back(t); // I'm sure it's sorted
		}
		
		for (state f: Fa.final_states) {
			out.final_states.push_back(f); // I'm sure it's sorted
		}
		
		return out;
	}
	
	static constexpr auto result = build();
};

}

#endif
