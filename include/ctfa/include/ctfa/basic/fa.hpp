#ifndef CTFA__BASIC__FA__HPP
#define CTFA__BASIC__FA__HPP

#include <cstddef>
#include "../container/set.hpp"
#include "transition.hpp"

namespace ctfa {

struct transition_count_pair {
	size_t from_start{0};
	size_t other{0};
};

template <size_t Transitions, size_t FinalStates> struct finite_automaton {
	ctfa::set<transition, Transitions> transitions{};
	ctfa::set<state, FinalStates> final_states{};
	
	constexpr finite_automaton() = default;
	constexpr finite_automaton(const ctfa::set<transition, Transitions> & t, const ctfa::set<state, FinalStates> & f): transitions{t}, final_states{f} { }
	
	constexpr bool is_final(state s) const noexcept {
		if (s == invalid_state) return false;
		return final_states.find(s) != final_states.end();
	}

	constexpr auto create_blank() const noexcept {
		return finite_automaton<Transitions, FinalStates>{};
	}
	
	constexpr state next_free_state() const noexcept {
		state highest = invalid_state;
		// I need to iterate because there can be unaccessible state (it's sorted source,target )
		for (const auto & t: transitions) {
			highest = std::max(t.source, highest);
			highest = std::max(t.target, highest);
		}
		for (state s: final_states) {
			highest = std::max(s, highest);
		}
		return highest.next();
	}
	constexpr void add(const transition & t) {
		transitions.insert(t);
	}
	constexpr void mark_final(state s) {
		final_states.insert(s);
	}
	template <size_t T, size_t F> constexpr bool operator==(const finite_automaton<T,F> & rhs) const noexcept {
		return final_states == rhs.final_states && transitions == rhs.transitions;
	}
};

template <const auto & Fa, typename TransitionCB, typename FinalCB> void debug(TransitionCB && tran, FinalCB && final) {
	for (const auto & t: Fa.transitions) {
		tran(t.source, t.target, t.cond, Fa.is_final(t.source));
	}
	for (const auto & f: Fa.final_states) {
		if (Fa.transitions.find(f) == Fa.transitions.end()) final(f);
	}
}


template <typename F> struct filtered_out {
	F fa;
	size_t transitions;
	size_t final_states;
};

template <typename F> filtered_out(const F &, size_t, size_t) -> filtered_out<F>;



}

#endif
