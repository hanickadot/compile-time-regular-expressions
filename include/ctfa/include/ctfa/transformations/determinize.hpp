#ifndef CTFA__TRANSFORMATION__DETERMINIZE2__HPP
#define CTFA__TRANSFORMATION__DETERMINIZE2__HPP

#include "../helper/info.hpp"
#include "../helper/interval.hpp"
#include "remove_unneeded.hpp"
#include <algorithm>

namespace ctfa {

struct resize_info {
	size_t length;
	size_t states;
};

template <size_t States, size_t Length> struct line {
	ctfa::set<state, Length> states;
	bool final{false};
	bool visited{false};
	
	constexpr line() { }
	constexpr line(const ctfa::set<state, Length> & states, bool final): states{states}, final{final} { }
	constexpr line(const line & other) = default;
	constexpr line & operator=(const line &) = default;
	template <size_t RhsStates, size_t RhsLength> constexpr line(const line<RhsStates, RhsLength> & other): states{other.states}, final{other.final}, visited{other.visited} { }
	constexpr bool operator<(const line & rhs) const {
		return states < rhs.states;
	}
	constexpr bool operator==(const line & rhs) const {
		return states == rhs.states;
	}
	template <size_t RhsLength> constexpr bool operator<(const ctfa::set<state, RhsLength> & rhs) const {
		return states < rhs;
	}
	template <size_t RhsLength> constexpr bool operator==(const ctfa::set<state, RhsLength> & rhs) const {
		return states == rhs;
	}
};

template <size_t States, size_t Length> struct list_of_states {
	static constexpr size_t max_length = Length;
	
	using line = ctfa::line<States, Length>;
	
	ctfa::set<line, States> _data;
	
	constexpr list_of_states() {
		// do nothing
	}
	template <size_t RhsStates, size_t RhsLength> constexpr list_of_states(const list_of_states<RhsStates, RhsLength> & older) {
		for (const auto & l: older) {
			_data.push_back(line(l));
		}
	}
	constexpr void mark_everything_visited() {
		for (auto & l: _data) {
			l.visited = true;
		}
	}
	template <size_t RhsLength> constexpr bool exists(const ctfa::set<state, RhsLength> & rhs) const {
		return _data.find(rhs) != _data.end();
	}
	constexpr auto begin() const {
		return _data.begin();
	}
	constexpr auto end() const {
		return _data.end();
	}
	template <size_t RhsLength> constexpr auto find(const ctfa::set<state, RhsLength> & rhs) const {
		return _data.find(rhs);
	}
	template <size_t RhsLength> constexpr auto one_state(const ctfa::set<state, RhsLength> & rhs) const {
		return state{int(_data.find(rhs) - _data.begin())};
	}
	constexpr size_t size() const {
		return _data.size();
	}
	constexpr void insert(state s, bool is_final = false) {
		auto pair = _data.insert(line(ctfa::set<state, Length>({s}), is_final));
		if (!pair.second) {
			pair.first->final |= is_final;
		}
	}
	constexpr void insert(ctfa::set<state, Length> s, bool is_final = false) {
		auto pair = _data.insert(line{s, is_final});
		if (!pair.second) {
			pair.first->final |= is_final;
		}
	}
	constexpr bool contains_new_states() const {
		for (const auto & l: _data) {
			if (l.visited == false) return true;
		}
		return false;
	}
};

template <const auto & Fa> struct determinize_one {

	// iterate over all transitions from multi-state (no determinization done, they are from original Fa)
	template <size_t N, typename CB> static constexpr auto iterate_over_multi_state(const ctfa::set<state, N> & states, CB && callback) {
		for (state s: states) {
			auto it = Fa.transitions.lower_bound(s);
			while (it != Fa.transitions.end() && it->source == s) {
				callback(*it);
				it++;
			}
		}
	}

	template <size_t N> static constexpr bool is_multi_state_final(const ctfa::set<state, N> & states) {
		for (state s: states) {
			if (Fa.is_final(s)) return true;
		}
		return false;
	}

	static constexpr auto max_state_count = Fa.transitions.size() * 2 + Fa.final_states.size();
	static constexpr auto max_transition_count = Fa.transitions.size();

	static constexpr auto build_seed() noexcept {
		list_of_states<max_state_count, 1> seed;
	
		for (const auto & t: Fa.transitions) {
			seed.insert(t.source);
			seed.insert(t.target);
		}
	
		for (state s: Fa.final_states) {
			seed.insert(s, true);
		}
	
		return seed;
	}

	static constexpr auto seed = build_seed();

	template <const auto & states> static constexpr auto process() noexcept {
		// calculate how much of new states we need
		constexpr resize_info info = [&]{
			size_t max_length = states.max_length;
			size_t potential_states = states.size();
			for (const auto & st: states) {
				if (st.visited) continue; // ignore visited
			
				intervals<max_transition_count, char32_t, state> i;
				// merge all transitions from state string length
				iterate_over_multi_state(st.states, [&](const transition & t){
					i.insert_range(t.cond.r.low, t.cond.r.high, t.target);
				});
				// determinize state
				i.merge_and_split([&](char32_t, char32_t, const auto & target_set){
					// if new state will be created, resize output, and find maximum of state string length
					if (!states.exists(target_set)) {
						if (target_set.size() > 1) {
							// this is merger
							max_length = std::max(max_length, target_set.size());
							potential_states++;
						}
					}
				});
			}
			return resize_info{max_length, potential_states};
		}();
	
		// fill output
		list_of_states<info.states, info.length> out{states}; // it also set visited in copy constructor
		out.mark_everything_visited();
	
		// add new states
		for (const auto & st: states) {
			if (st.visited) continue; // ignore visited
		
			intervals<max_transition_count, char32_t, state> i;
			// merge all transitions from state string length
			iterate_over_multi_state(st.states, [&](const transition & t){
				i.insert_range(t.cond.r.low, t.cond.r.high, t.target);
			});
			// determinize state
			i.merge_and_split([&](char32_t, char32_t, const auto & target_set){
				// if new state will be created, resize output, and find maximum of state string length
				if (!states.exists(target_set)) {
					if (target_set.size() > 1) {
						// this is merger
						bool is_final = is_multi_state_final(target_set);
						out.insert(target_set, is_final);
					}
				}
			});
		}
	
		return out;
	}
	
	template <const auto & previous> struct recursion_helper {
		static constexpr auto output = process<previous>();
	};
	
	template <const auto & previous = seed> constexpr static auto cycle_build_states() {
		if constexpr (recursion_helper<previous>::output.contains_new_states()) {
			return cycle_build_states<recursion_helper<previous>::output>();
		} else {
			return recursion_helper<previous>::output;
		}
	}
	
	static constexpr auto all_dfa_states = cycle_build_states();
	
	constexpr static auto build() {
		constexpr resize_info info = []{
			size_t final_states{0};
			size_t transitions{0};
			for (const auto & st: all_dfa_states) {
				// calculate final_states
				if (st.final) {
					final_states++;
				}
				
				intervals<max_transition_count, char32_t, state> i;
				// merge all transitions from state string length
				iterate_over_multi_state(st.states, [&](const transition & t){
					i.insert_range(t.cond.r.low, t.cond.r.high, t.target);
				});
				// determinize state
				i.merge_and_split([&](char32_t, char32_t, const auto &){
					transitions++;
				});
			}
			return resize_info{transitions, final_states};
		}(); 
		
		finite_automaton<info.length, info.states> out;
		
		for (const auto & st: all_dfa_states) {
			auto source_state = all_dfa_states.one_state(st.states);
			
			if (st.final) {
				out.mark_final(source_state);
			}
			
			intervals<max_transition_count, char32_t, state> i;
			// merge all transitions from state string length
			iterate_over_multi_state(st.states, [&](const transition & t){
				i.insert_range(t.cond.r.low, t.cond.r.high, t.target);
			});
			// determinize state
			i.merge_and_split([&](char32_t low, char32_t high, const auto & target_set){
				auto target_state = all_dfa_states.one_state(target_set);
				out.add(transition{source_state, target_state, condition(impl::range{low, high})});
			});
		}
		
		return out;
	}
	
	static constexpr auto result = build();
};
	
}

#endif
