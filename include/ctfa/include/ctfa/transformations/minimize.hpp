#ifndef CTFA__TRANSFORMATION__MINIMIZE__HPP
#define CTFA__TRANSFORMATION__MINIMIZE__HPP

#include "../helper/info.hpp"
#include "shrink.hpp"
#include "remove_unneeded.hpp"
#include "../helper/interval.hpp"

// TODO better minimizing, based on merge_and_split to groups, not based on different signatures of conditions

namespace ctfa {
	
namespace impl {
	
struct extended_transition {
	transition t;
	int target_index;
	constexpr bool operator<(const extended_transition & rhs) const noexcept {
		return t < rhs.t;
	}
	constexpr bool operator==(const extended_transition & rhs) const noexcept {
		return t == rhs.t;
	}
};
	
template <size_t N> struct transitions_from_state {
	// these are provided 

	state s;
	bool is_final{false};
	int index{0};
	
	// these will be set later
	state group{start_state};
	state next_group{invalid_state};
	bool is_unique{false};
	ctfa::set<extended_transition, N> transitions{};
	ctfa::set<transition, N*2> merged_transitions{};
	
	template <size_t States> constexpr void merge_based_on_group(const ctfa::set<transitions_from_state<N>, States> & table) noexcept {
		intervals<N, char32_t, state> i;
		
		for (const auto & t: transitions) {
			i.insert_range(t.t.cond.r.low, t.t.cond.r.high, table[t.target_index].group);
		}
		
		i.merge_and_split([&](char32_t low, char32_t high, const auto & target_set){
			for (state t: target_set) {
				merged_transitions.insert(transition{s, t, condition(impl::range{low, high})});
			}
		});
	}
	
	constexpr bool operator<(const transitions_from_state & rhs) const noexcept {
		return s < rhs.s;
	}
	constexpr bool operator==(const transitions_from_state & rhs) const noexcept {
		return s == rhs.s;
	}
	constexpr bool operator<(state rhs) const noexcept {
		return s < rhs;
	}
	constexpr bool operator==(state rhs) const noexcept {
		return s == rhs;
	}
};

template <size_t MaxTransitions> constexpr bool is_similar_state(const transitions_from_state<MaxTransitions> & lhs, const transitions_from_state<MaxTransitions> & rhs) {
	// should be in same group
	if (lhs.group != rhs.group) {
		return false;
	}
	if (lhs.is_final != rhs.is_final) {
		return false;
	}
	
	// should have same size
	if (lhs.merged_transitions.size() != rhs.merged_transitions.size()) {
		return false;
	}
	
	for (size_t i = 0; i != lhs.merged_transitions.size(); ++i) {
		// all transitions should have same condition
		if (lhs.merged_transitions[i].cond != rhs.merged_transitions[i].cond) {
			return false;
		}
		// and point to same group
		if (lhs.merged_transitions[i].target != rhs.merged_transitions[i].target) {
			return false;
		}
	}
	
	return true;
}

template <size_t States, size_t MaxTransitions> 
constexpr void mark_group(const ctfa::set<impl::transitions_from_state<MaxTransitions>, States> & table, transitions_from_state<MaxTransitions> & subject, state & available_group) noexcept {
	// find same group and use it's identification or generate new one
	subject.is_unique = true;
	state found_group = available_group;
	
	for (const auto & row: table) {
		if (row.next_group == invalid_state) {
			break;
		}
		
		if (is_similar_state(subject, row)) {
			subject.is_unique = false;
			found_group = row.next_group;
			break;
		}
	}
	
	subject.next_group = found_group;
	// if it's unique, increment state counter
	if (found_group == available_group) available_group = available_group.next();
}

template <size_t States, size_t MaxTransitions> constexpr bool build_groups(ctfa::set<impl::transitions_from_state<MaxTransitions>, States> & table) noexcept {
	// reset next group to default
	for (auto & row: table) {
		row.next_group = invalid_state;
		row.merge_based_on_group(table);
	}
	
	// first group is 0 (start)
	state group_counter = start_state;
	
	// for reach row find similar
	for (auto & row: table) {
		mark_group(table, row, group_counter);
	}
	
	// store next group in group
	bool changed = false;
	
	for (auto & row: table) {
		if (row.group != row.next_group) changed = true;
		row.group = row.next_group;
	}

	return changed;
}

}

template <const auto & Arg> struct minimize_one {
	// before we start minimizing, we need to remove anything unneeded
	static constexpr auto fa = remove_unneeded_one<Arg>::result_unshrinked;
	
	using state_and_transitions = impl::transitions_from_state<fa.transitions.size()>;

	static constexpr auto build() {
		constexpr auto known_states = []{
			ctfa::set<state_and_transitions, ctfa::info<fa>::states> known_states;
		
			// build state table
		
			ctfa::info<fa>::iterate_over_states([&, index = 0] (const auto & s) mutable {
				known_states.push_back({
					state(s), s.is_final, index++
				});
			});
		
			// insert transition with target index in the extended_transition
			for (const auto & t: fa.transitions) {
				auto source = known_states.find(t.source);
				int index = known_states.find(t.target) - known_states.begin();
				source->transitions.push_back(impl::extended_transition{t, index});
			}
		
			// divide into groups until there is no change
			while (build_groups(known_states));
			
			return known_states;
		}();
		
		constexpr auto transitions = [&]{
			size_t count = 0;
			for (const auto & s: known_states) {
				if (s.is_unique) {
					intervals<fa.transitions.size(), char32_t, state> i;
				
					auto source = s.group;
				
					for (const auto & et: s.transitions) {
						i.insert_range(et.t.cond.r.low, et.t.cond.r.high, known_states[et.target_index].group);
					}
		
					i.merge_and_split([&](char32_t, char32_t, const auto & target_set){
						for (state t: target_set) { // it will happen only once, or it will be nondeterministic
							count++;
						}
					});
				}
			}
			return count;
		}();
		
		auto out = finite_automaton<transitions, fa.final_states.size()>();
		
		for (const auto & s: known_states) {
			if (s.is_unique) {
				if (s.is_final) {
					out.final_states.push_back(s.group); // I know it's sorted
				}
				
				intervals<fa.transitions.size(), char32_t, state> i;
				
				auto source = s.group;
				
				for (const auto & et: s.transitions) {
					i.insert_range(et.t.cond.r.low, et.t.cond.r.high, known_states[et.target_index].group);
				}
		
				i.merge_and_split([&](char32_t low, char32_t high, const auto & target_set){
					for (state t: target_set) { // it will happen only once, or it will be nondeterministic
						out.transitions.push_back(transition{source, t, condition(impl::range{low, high})});
					}
				});
			} 
		}
		
		return out;
	}
	
	static constexpr auto result_unshrinked = build();
	static constexpr auto result = shrink_one<result_unshrinked>::result;
};

}

#endif
