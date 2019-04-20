#ifndef CTFA__HELPER__TRANSLATOR__HPP
#define CTFA__HELPER__TRANSLATOR__HPP

#include "../basic/state.hpp"
#include "../basic/transition.hpp"
#include "../container/set.hpp"

namespace ctfa {

struct merged_transition {
	bool is_final;
	bool is_new;
	state target;
};

enum class translation_key {
	single,
	multi
};

template <translation_key Key, size_t S> struct state_and_states {
	ctfa::set<state, S> multi{};
	state single{};
	bool is_final{false};
	constexpr state_and_states() noexcept { }
	template <size_t S2> constexpr state_and_states(const ctfa::set<state, S2> & set, state single, bool is_final) noexcept: multi{set}, single{single}, is_final{is_final} {
		
	}
	template <translation_key Key2, size_t S2> constexpr state_and_states(const state_and_states<Key2, S2> & rhs) noexcept {
		for (auto s: rhs.multi) {
			multi.push_back(s);
		}
		single = rhs.single;
		is_final = rhs.is_final;
	}
	constexpr bool operator==(const state_and_states & rhs) const noexcept {
		if constexpr (Key == translation_key::single) {
			return single == rhs.single;
		} else {
			return multi == rhs.multi;
		}
	}
	constexpr bool operator<(const state_and_states & rhs) const noexcept {
		if constexpr (Key == translation_key::single) {
			return single < rhs.single;
		} else {
			return multi < rhs.multi;
		}
	}
	constexpr bool operator==(state rhs) const noexcept {
		return single == rhs;
	}
	constexpr bool operator<(state rhs) const noexcept {
		return single < rhs;
	}
	template <size_t S2> constexpr bool operator==(const ctfa::set<state, S2> & rhs) const noexcept {
		return multi == rhs;
	}
	template <size_t S2> constexpr bool operator<(const ctfa::set<state, S2> & rhs) const noexcept {
		return multi < rhs;
	}
};

template <size_t S> using state_to_states = state_and_states<translation_key::single, S>;
template <size_t S> using states_to_state = state_and_states<translation_key::multi, S>;


template <size_t N, size_t S> struct translator {
	static constexpr size_t capacity = N;
	static constexpr size_t max_length = S;
	
	state available_state{invalid_state};

	using states_to_state = ctfa::states_to_state<S>;
	using state_to_states = ctfa::state_to_states<S>;

	ctfa::set<states_to_state, N> multi_to_single{};
	ctfa::set<state_to_states, N> single_to_multi{};
	
	template <size_t N2, size_t S2> constexpr translator & operator=(const translator<N2, S2> & rhs) noexcept {
		available_state = rhs.available_state;
		for (const auto & s: rhs.multi_to_single) {
			single_to_multi.push_back(state_to_states(s));
			multi_to_single.push_back(states_to_state(s));
		}
		return *this;
	}
	
	constexpr bool is_empty() const noexcept {
		return N == 0 && S == 0;
	}
	
	constexpr void add_basic(state t, bool is_final = false) noexcept {
		ctfa::set<state, S> multi;
		multi.push_back(t);
		multi_to_single.insert(states_to_state(multi, t, is_final));
		single_to_multi.insert(state_to_states(multi, t, is_final));
		available_state = t.next();
	}
	template <typename CB> constexpr void find_state(state target, CB && cb) const noexcept {
		auto it = single_to_multi.find(target);
		if (it != single_to_multi.end()) {
			for (auto s: it->multi) {	
				cb(s);
			}
		}
	}
	template <size_t S2, typename CB> constexpr merged_transition find_or_callback(const ctfa::set<state,S2> & states, CB && create_merged) const {
		auto existing = multi_to_single.find(states);
		
		if (existing != multi_to_single.end()) {
			return merged_transition{existing->is_final, false, existing->single};
		} else {
			return create_merged();
		}
	}
	template <size_t S2> constexpr merged_transition merge(const ctfa::set<state,S2> & states) {
		return find_or_callback(states, [&](){
			state new_state = available_state;
			available_state = available_state.next();
			
			bool is_final = false;
			
			for (state s: states) {
				auto it = single_to_multi.find(s);
				if (it != single_to_multi.end()) {
					is_final |= it->is_final;
				}
			}
			
			multi_to_single.insert(states_to_state(states, new_state, is_final));
			single_to_multi.insert(state_to_states(states, new_state, is_final));
			
			return merged_transition{is_final, true, new_state};
		});
	}
	template <size_t S2> constexpr merged_transition precalculate_merge(const ctfa::set<state,S2> & states) const {
		return find_or_callback(states, [&](){
			bool is_final = false;
	
			for (state s: states) {
				auto it = single_to_multi.find(s);
				if (it != single_to_multi.end()) {
					is_final |= it->is_final;
				}
			}
			return merged_transition{is_final, true, invalid_state};
		});
	}
};

static constexpr auto empty_translator = translator<0,0>();

}

#endif
