#ifndef CTFA__TRANSFORMATION__REMOVE_UNNEEDED__HPP
#define CTFA__TRANSFORMATION__REMOVE_UNNEEDED__HPP

#include "../helper/info.hpp"
#include "shrink.hpp"

namespace ctfa {
	
namespace impl {
	
struct reachable_state {
	state s;
	bool reachable_from_start = false;
	bool reachable_from_final = false;
	constexpr bool operator<(const reachable_state & rhs) const noexcept {
		return s < rhs.s;
	}
	constexpr bool operator==(const reachable_state & rhs) const noexcept {
		return s == rhs.s;
	}
	constexpr bool operator<(state rhs) const noexcept {
		return s < rhs;
	}
	constexpr bool operator==(state rhs) const noexcept {
		return s == rhs;
	}
	constexpr bool mark_reachable_from_start() noexcept {
		bool out = reachable_from_start;
		reachable_from_start = true;
		return !out;
	}
	constexpr bool mark_reachable_from_final() noexcept {
		bool out = reachable_from_final;
		reachable_from_final = true;
		return !out;
	}
	constexpr bool is_needed() const noexcept {
		return reachable_from_start && reachable_from_final;
	}
};

}

template <const auto & Fa> struct remove_unneeded_one {
	static constexpr auto build() {
		ctfa::set<impl::reachable_state, ctfa::info<Fa>::states> known_states;
		
		ctfa::info<Fa>::iterate_over_states([&](const auto & s){
			known_states.push_back(impl::reachable_state{
				state(s), (state(s) == start_state), (s.is_final)
			});
		});
		
		bool changed = true;
		while (changed) {
			changed = false;
			for (const auto & t: Fa.transitions) {
				auto source = known_states.find(t.source);
				auto target = known_states.find(t.target);
				if (source->reachable_from_start) {
					if (target->mark_reachable_from_start()) changed = true;
				}
				if (target->reachable_from_final) {
					if (source->mark_reachable_from_final()) changed = true;
				}
			}
		}
		
		auto out = Fa.create_blank();
		
		for (const auto & t: Fa.transitions) {
			auto source = known_states.find(t.source);
			auto target = known_states.find(t.target);
			if (source->is_needed() && target->is_needed()) {
				out.transitions.push_back(t);  // I know it's sorted
			}
		}
		
		for (state f: Fa.final_states) {
			auto final = known_states.find(f);
			if (final->is_needed()) {
				out.final_states.push_back(f); // I know it's sorted
			}
		}
		
		return out;
	}
	
	static constexpr auto result_unshrinked = build();
	static constexpr auto result = shrink_one<result_unshrinked>::result;
};

}

#endif
