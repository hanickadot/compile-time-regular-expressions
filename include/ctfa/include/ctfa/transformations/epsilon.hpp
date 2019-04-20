#ifndef CTFA__TRANSFORMATION__EPSILON__HPP
#define CTFA__TRANSFORMATION__EPSILON__HPP

#include "../basic/fa.hpp"

namespace ctfa {

template <typename Fa, typename TransitionCB, typename FinalState> constexpr std::pair<size_t, bool> epsilon_for(const Fa & fa, state source, TransitionCB && transition_cb, FinalState && final_cb) {
	size_t transition_count = 0;
	bool is_final = false;
	auto it = (source == start_state) ? fa.transitions.begin() : fa.transitions.find(source);
	while (it != fa.transitions.end() && *it == source) {
		transition_cb(*it++);
		transition_count++;
	}
	auto fit = (source == start_state) ? fa.final_states.begin() : fa.final_states.find(source);
	if (fit != fa.final_states.end() && *fit == source) {
		final_cb(source);
		is_final = true;
	}
	return std::pair<size_t, bool>{transition_count, is_final};
}

template <typename Fa, typename TransitionCB> constexpr std::pair<size_t, bool> epsilon_for(const Fa & fa, state source, TransitionCB && transition_cb) {
	return epsilon_for(fa, source, std::forward<TransitionCB>(transition_cb), [](auto && ...){});
}

template <typename Fa> constexpr std::pair<size_t, bool> epsilon_join_count(const Fa & fa, state source) {
	return epsilon_for(fa, source, [](auto && ...){}, [](auto && ...){});
}

}

#endif
