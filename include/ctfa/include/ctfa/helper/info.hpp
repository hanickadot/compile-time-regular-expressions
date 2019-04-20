#ifndef CTFA__HELPER__INFO__HPP
#define CTFA__HELPER__INFO__HPP

#include "../container/set.hpp"
#include "../basic/fa.hpp"
#include <algorithm>

namespace ctfa {

struct state_info {
	state s;
	size_t count{0};
	bool is_final{false};
	constexpr operator state() const noexcept {
		return s;
	}
	constexpr bool operator==(const state_info & rhs) const noexcept {
		return s == rhs.s;
	}
	constexpr bool operator<(const state_info & rhs) const noexcept {
		return s < rhs.s;
	}
};

template <const auto & Fa> struct info {
	template <typename CB> static constexpr size_t iterate_over_states(CB && cb) noexcept {
		ctfa::set<state_info, Fa.transitions.size() * 2 + Fa.final_states.size()> known_state;
		for (const auto & t: Fa.transitions) {
			auto pair = known_state.insert(state_info{t.source});
			if (pair.first != known_state.end()) {
				pair.first->count++;
			}
			
			known_state.insert(state_info{t.target});
		}
		for (state s: Fa.final_states) {
			auto pair = known_state.insert(state_info{s});
			if (pair.first != known_state.end()) {
				pair.first->is_final = true;
			}
		}
		for (const state_info & s: known_state) {
			cb(s);
		}
		return known_state.size();
	}
	
	static constexpr size_t max_transitions = []{
		size_t max{0};
		iterate_over_states([&](const auto & state_info){
			max = std::max(state_info.count, max);
		});
		return max;
	}();
	
	static constexpr size_t states = iterate_over_states([](const auto &){});
	
	static constexpr size_t final_states = Fa.final_states.size();
};

}

#endif
