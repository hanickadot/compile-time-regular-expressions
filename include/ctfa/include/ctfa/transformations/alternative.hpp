#ifndef CTFA__TRANSFORMATION__ALTERNATIVE__HPP
#define CTFA__TRANSFORMATION__ALTERNATIVE__HPP

#include "../basic/fa.hpp"
#include "epsilon.hpp"

namespace ctfa {

template <const auto & Lhs, const auto & Rhs> struct alternative_two {
	static constexpr auto build() {
		constexpr std::pair lhs_info = epsilon_join_count(Lhs, start_state);
		constexpr std::pair rhs_info = epsilon_join_count(Rhs, start_state);
		constexpr size_t transitions = Lhs.transitions.size() + Rhs.transitions.size() + Lhs.final_states.size() * lhs_info.first + rhs_info.first;
		constexpr size_t final_states = Lhs.final_states.size() + Rhs.final_states.size() + std::max(lhs_info.second, rhs_info.second);
		
		finite_automaton<transitions, final_states> output;
		
		constexpr state lhs_base = start_state.next();
		constexpr state rhs_base = Lhs.next_free_state().next();
		
		// I can do this two push_backs because I know it's already sorted
		for (const auto & t: Lhs.transitions) {
			output.transitions.push_back(t.prefix(lhs_base)); // quicker than .add
		}
		
		for (const auto & t: Rhs.transitions) {
			output.transitions.push_back(t.prefix(rhs_base)); // quicker than .add
		}
		
		epsilon_for(Lhs, start_state, [&](transition t) {
			t.source = start_state;
			t.target = t.target.prefix_base(lhs_base);
			output.add(t);
		}, [&](state) {
			output.mark_final(start_state);
		});
		
		epsilon_for(Rhs, start_state, [&](transition t) {
			t.source = start_state;
			t.target = t.target.prefix_base(rhs_base);
			output.add(t);
		}, [&](state) {
			output.mark_final(start_state);
		});
		
		
		for (state s: Lhs.final_states) {
			output.mark_final(s.prefix_base(lhs_base));
		}
		
		for (state s: Rhs.final_states) {
			output.mark_final(s.prefix_base(rhs_base));
		}
		
		return output;
	}
	
	static constexpr auto result = build();
};

}

#endif
