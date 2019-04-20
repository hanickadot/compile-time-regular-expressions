#ifndef CTFA__MATCHER__MATCH__HPP
#define CTFA__MATCHER__MATCH__HPP

#include "../utility.hpp"
#include <array>

namespace ctfa {
	
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
		
		template <int transition_index, int current_state, typename Iterator, typename EndIterator> static CTFA_FORCE_INLINE constexpr bool choose_transition(Iterator it, const EndIterator end) noexcept {
			if constexpr (transition_index != Dfa.transitions.size() && Dfa.transitions[transition_index].source.id == current_state) {
				constexpr auto transition = Dfa.transitions[transition_index];
				if (transition.cond.match(*it) && end != it) {
					return match_state<Iterator, EndIterator, transition.target.id>(it+1, end);
				} else {
					return choose_transition<transition_index+1, current_state>(it, end);
				}
			} else {
				return false;
			}
		}
	
		template <typename Iterator, typename EndIterator, int State> static CTFA_FLATTEN constexpr bool match_state(Iterator it, EndIterator end) noexcept {
			constexpr state current = state{State};
			constexpr auto transition_index = Dfa.transitions.lower_bound(current) - Dfa.transitions.begin();
			
			if constexpr (Dfa.is_final(current)) {
				if (end == it) return true;
			} else {
				if (end == it) return false;
			}
			
			return choose_transition<transition_index, current.id>(it, end);
		} 

		template <typename Iterator, typename EndIterator> static CTFA_FORCE_INLINE constexpr bool run(Iterator it, const EndIterator end) noexcept {
			return match_state<Iterator, EndIterator, 0>(it, end);		
		}
	};

	template <const auto & DFa> static constexpr auto match_wrap = ctfa::minimize<ctfa::determinize<DFa>>;
	
	template <const auto & DFa, typename Range> constexpr bool table_match(Range && range) noexcept {
		return dispatcher<match_wrap<DFa>>::run(range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool table_match_ptr(const Ptr * ptr) noexcept {
		return dispatcher<match_wrap<DFa>>::run(ptr, ctfa::zero_terminated_string_end_iterator());
	}

	static constexpr auto any_star = ctfa::star<ctfa::block::anything>;

	template <const auto & DFa> static constexpr auto search_wrap = ctfa::minimize<ctfa::determinize<any_star,DFa,any_star>>;
	
	template <const auto & DFa, typename Range> constexpr bool table_search(Range && range) noexcept {
		return dispatcher<search_wrap<DFa>>::run(range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool table_search_ptr(const Ptr * ptr) noexcept {
		return dispatcher<search_wrap<DFa>>::run(ptr, ctfa::zero_terminated_string_end_iterator());
	}


}

#endif
