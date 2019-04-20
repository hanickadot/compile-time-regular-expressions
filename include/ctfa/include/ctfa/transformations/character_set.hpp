#ifndef CTFA__TRANSFORMATION__CHARACTER_SET__HPP
#define CTFA__TRANSFORMATION__CHARACTER_SET__HPP

#include "../basic/fa.hpp"
#include "../helper/interval.hpp"
#include <limits>

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

template <const auto & Fa> struct negative_set_one {
	static constexpr auto build() {
		constexpr auto inter = []{
			intervals<Fa.transitions.size()+1, char32_t, state> i;
		
			for (const auto & t: Fa.transitions) {
				i.insert_range(t.cond.r.low, t.cond.r.high, state{1});
			}
		
			i.insert_range(std::numeric_limits<char32_t>::min(), std::numeric_limits<char32_t>::max(), state{0});
		
			i.merge();
			return i;
		}();
		
		// calculate needed size
		constexpr size_t count = [&]{
			size_t c = 0;
			inter.split([&](char32_t, char32_t, const auto & target_set){
				if (target_set.size() == 1 && target_set[0] == state{0}) {
					c++;
				}
			});
			return c;
		}();
		
		finite_automaton<count, 1> output;
		
		// create transition from everything else
		inter.split([&](char32_t low, char32_t high, const auto & target_set){
			if (target_set.size() == 1 && target_set[0] == state{0}) {
				output.add(transition{start_state, state{1}, condition(impl::range{low, high})});
			}
		});
		
		// state 1 is final
		output.mark_final(state{1});
		
		return output;
	}
	
	static constexpr auto result = build();
};

}

#endif
