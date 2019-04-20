#ifndef CTFA__TRANSFORMATION__OPTIONAL__HPP
#define CTFA__TRANSFORMATION__OPTIONAL__HPP

#include "../basic/fa.hpp"
#include "epsilon.hpp"

namespace ctfa {

template <const auto & Fa> struct optional_one {
	static constexpr auto build() {
		constexpr std::pair info = epsilon_join_count(Fa, start_state);
		constexpr size_t transitions = Fa.transitions.size() + info.first;
		constexpr size_t final_states = Fa.final_states.size() + 1;
		
		finite_automaton<transitions, final_states> output;
		
		constexpr state base = start_state.next();
		
		for (const auto & t: Fa.transitions) {
			output.transitions.push_back(t.prefix(base)); // quicker
		}
		
		epsilon_for(Fa, start_state, [&](transition t) {
			t.source = start_state;
			t.target = t.target.prefix(base);
			output.add(t);
		});
		
		output.mark_final(start_state);
		
		for (state s: Fa.final_states) {
			output.mark_final(s.prefix(base));
		}
		
		return output;
	}
	
	static constexpr auto result = build();
};

}

#endif
