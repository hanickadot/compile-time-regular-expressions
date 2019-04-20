#ifndef CTFA__MATCHER__MATCH__HPP
#define CTFA__MATCHER__MATCH__HPP

#include "../utility.hpp"
#include <array>

namespace ctfa {
	
	template <const auto & Dfa> struct table_dispatcher {
		static constexpr auto states() {
			constexpr size_t size = ctfa::info<Dfa>::states;
	
			ctfa::set<state, size> list;
	
			ctfa::info<Dfa>::iterate_over_states([&](state s){
				list.push_back(s);
			});
	
			return list;
		}
		
		static constexpr auto list = states();
		
		template <int transition_index, int current_state, typename Iterator, typename EndIterator> static CTFA_FORCE_INLINE constexpr bool choose_transition(Iterator it, const EndIterator end) noexcept {
			if constexpr (transition_index != Dfa.transitions.size() && Dfa.transitions[transition_index].source.id == current_state) {
				constexpr auto transition = Dfa.transitions[transition_index];
				if (transition.cond.match(*it) && end != it) {
					constexpr size_t index = list.find(transition.target) - list.begin();
					return run(index, it+1, end);
				} else {
					return choose_transition<transition_index+1, current_state>(it, end);
				}
			} else {
				return false;
			}
		}
	
		template <typename Iterator, typename EndIterator, size_t State> static CTFA_FLATTEN constexpr bool match_state(Iterator it, EndIterator end) noexcept {
			constexpr state current = list[State];
			constexpr auto transition_index = Dfa.transitions.lower_bound(current) - Dfa.transitions.begin();
			
			if constexpr (Dfa.is_final(current)) {
				if (end == it) return true;
			} else {
				if (end == it) return false;
			}
			
			return choose_transition<transition_index, current.id>(it, end);
		} 

		template <typename Iterator, typename EndIterator, size_t... Idx> static constexpr auto build_internal(std::index_sequence<Idx...>) noexcept {
			using function_ptr = bool(*)(Iterator, const EndIterator) noexcept;
			std::array<function_ptr, sizeof...(Idx)> out{{match_state<Iterator, EndIterator, Idx>...}};

			return out;
		}

		template <typename Iterator, typename EndIterator> static constexpr auto CTFA_FORCE_INLINE build() noexcept {
			return build_internal<Iterator, EndIterator>(std::make_index_sequence<list.size()>());
		}

		template <typename Iterator, typename EndIterator> static constexpr auto table = build<Iterator, EndIterator>();

		template <typename Iterator, typename EndIterator> static CTFA_FORCE_INLINE constexpr bool run(size_t s, Iterator it, const EndIterator end) noexcept {
			return table<Iterator, EndIterator>[s](it, end);			
		}
	};

	template <const auto & Dfa> struct dispatcher {
		static constexpr auto states() {
			constexpr size_t size = ctfa::info<Dfa>::states;
		
			ctfa::set<state, size> list;
		
			ctfa::info<Dfa>::iterate_over_states([&](state s){
				list.push_back(s);
			});
		
			return list;
		}
	
		static constexpr auto list = states();
	
		template <int transition_index, int current_state, typename Iterator, typename EndIterator, typename CB> static CTFA_FORCE_INLINE constexpr bool choose_transition(Iterator it, const EndIterator end, CB && cb) noexcept {
			if constexpr (transition_index != Dfa.transitions.size() && Dfa.transitions[transition_index].source.id == current_state) {
				constexpr auto transition = Dfa.transitions[transition_index];
				if constexpr (transition.cond == ctfa::matcher::anything && transition.source == transition.target && Dfa.is_final(state{current_state})) {
					return true;
				} else if (transition.cond.match(*it) && end != it) {
					return cb(transition.target, it, end);
				} else {
					return choose_transition<transition_index+1, current_state>(it, end, std::forward<CB>(cb));
				}
			} else {
				if constexpr (Dfa.is_final(state{current_state})) {
					if (end == it) return true;
				} else {
					if (end == it) return false;
				}
				return false; // if I'm out of options
			}
		}
		
	
		template <int state_index, typename Iterator, typename EndIterator, typename CB> static constexpr bool CTFA_FORCE_INLINE choose_state(state s, Iterator it, const EndIterator end, CB && cb) noexcept {
			if constexpr (state_index != list.size()) {
				constexpr state current = list[state_index];
				if (s == current) {
					constexpr auto transition_index = Dfa.transitions.lower_bound(current) - Dfa.transitions.begin();
					
					return choose_transition<transition_index, current.id>(it, end, std::forward<CB>(cb));
				} else {
					return choose_state<state_index+1>(s, it, end, std::forward<CB>(cb));
				}
			} else {
				// ub
				[[maybe_unused]] auto never_happen = list[list.capacity];
				return false;
			}
		}
	
		template <typename Iterator, typename EndIterator> static CTFA_FORCE_INLINE constexpr bool run(state s, Iterator it, const EndIterator end) noexcept {
			return choose_state<0>(s, it, end, [](state s, Iterator it, const EndIterator end){
				return run(s, it+1, end);
			});
		}
		
	};

	template <const auto & DFa> static constexpr auto match_wrap = ctfa::minimize<ctfa::determinize<DFa>>;

	template <const auto & DFa, typename Range> constexpr bool match(Range && range) noexcept {
		return dispatcher<match_wrap<DFa>>::run(start_state, range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool match_ptr(const Ptr * ptr) noexcept {
		return dispatcher<match_wrap<DFa>>::run(start_state, ptr, ctfa::zero_terminated_string_end_iterator());
	}
	
	template <const auto & DFa, typename Range> constexpr bool table_match(Range && range) noexcept {
		return table_dispatcher<match_wrap<DFa>>::run(0, range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool table_match_ptr(const Ptr * ptr) noexcept {
		return table_dispatcher<match_wrap<DFa>>::run(0, ptr, ctfa::zero_terminated_string_end_iterator());
	}

	static constexpr auto any_star = ctfa::star<ctfa::block::anything>;

	template <const auto & DFa> static constexpr auto search_wrap = ctfa::minimize<ctfa::determinize<any_star,DFa,any_star>>;

	template <const auto & DFa, typename Range> constexpr bool search(Range && range) noexcept {
		return dispatcher<search_wrap<DFa>>::run(start_state, range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool search_ptr(const Ptr * ptr) noexcept {
		return dispatcher<search_wrap<DFa>>::run(start_state, ptr, ctfa::zero_terminated_string_end_iterator());
	}
	
	template <const auto & DFa, typename Range> constexpr bool table_search(Range && range) noexcept {
		return table_dispatcher<search_wrap<DFa>>::run(0, range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool table_search_ptr(const Ptr * ptr) noexcept {
		return table_dispatcher<search_wrap<DFa>>::run(0, ptr, ctfa::zero_terminated_string_end_iterator());
	}


}

#endif
