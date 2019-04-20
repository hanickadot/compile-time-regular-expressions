#ifndef CTFA__TRANSFORMATION__STAR__HPP
#define CTFA__TRANSFORMATION__STAR__HPP

#include "../basic/fa.hpp"
#include "epsilon.hpp"

namespace ctfa {

template <const auto & Fa> struct star_one {
	static constexpr auto build() {
		constexpr std::pair info = epsilon_join_count(Fa, start_state);
		constexpr size_t transitions = Fa.transitions.size() + info.first * Fa.final_states.size() + info.first;
		constexpr size_t final_states = Fa.final_states.size() + info.second + 1;
		
		finite_automaton<transitions, final_states> output;
		
		constexpr state base = start_state.next();
		
		// A + prefix => (B + prefix) (1 -> 2)
		for (const auto & t: Fa.transitions) {
			output.transitions.push_back(t.prefix(base));
		}
		
		// new_start => (old_start -> X) (0 -> 2)
		epsilon_for(Fa, start_state, [&](transition t) {
			t.source = start_state;
			t.target = t.target.prefix(base);
			output.add(t);
		}, [&](state f){
			output.mark_final(f);
		});
		
		// finals => (old_start -> X) (2 -> 2)
		for (state s: Fa.final_states) {
			epsilon_for(Fa, start_state, [&](transition t) {
				t.source = s;
				output.add(t.prefix(base));
			});
			output.mark_final(s.prefix(base));
		}
		
		output.mark_final(start_state);
		
		return output;
	}
	
	static constexpr auto result = build();
};

}

#endif
