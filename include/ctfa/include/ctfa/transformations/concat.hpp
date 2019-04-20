#ifndef CTFA__TRANSFORMATION__CONCAT__HPP
#define CTFA__TRANSFORMATION__CONCAT__HPP

#include "../basic/fa.hpp"
#include "epsilon.hpp"

namespace ctfa {

template <const auto & Lhs, const auto & Rhs> struct concat_two {
	static constexpr auto build() {
		constexpr std::pair info = epsilon_join_count(Rhs, start_state);
		constexpr size_t transitions = Lhs.transitions.size() + Rhs.transitions.size() + Lhs.final_states.size() * info.first;
		constexpr size_t final_states = Rhs.final_states.size() + info.second + Lhs.final_states.size();
		
		finite_automaton<transitions, final_states> output;
		
		// copy Lhs transitions
		for (const auto & t: Lhs.transitions) {
			output.transitions.push_back(t); // quicker
		}
		
		// prefix for all Rhs states in output
		constexpr state base = Lhs.next_free_state();
		
		static_assert(base != invalid_state);
		static_assert(base != start_state);
		
		for (state s: Lhs.final_states) {
			epsilon_for(Rhs, start_state, [&](transition t) {
				t.source = s;
				t.target = t.target.prefix(base);
				output.add(t);
			}, [&](state) {
				output.mark_final(s);
			});
		}

		// copy Rhs transitions + with base prefix
		for (const auto & t: Rhs.transitions) {
			output.add(t.prefix(base));
		}
		
		for (state s: Rhs.final_states) {
			output.mark_final(s.prefix(base));
		}
		
		return output;
	}
	
	static constexpr auto result = build();
};

}

#endif
