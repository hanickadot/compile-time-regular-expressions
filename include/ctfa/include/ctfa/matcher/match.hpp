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
	
	template <const auto & DFa, typename Range> constexpr bool match(Range && range) noexcept {
		return dispatcher<match_wrap<DFa>>::run(range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool match_ptr(const Ptr * ptr) noexcept {
		return dispatcher<match_wrap<DFa>>::run(ptr, ctfa::zero_terminated_string_end_iterator());
	}

	static constexpr auto any_star = ctfa::star<ctfa::block::anything>;

	template <const auto & DFa> static constexpr auto search_wrap = ctfa::minimize<ctfa::determinize<any_star,DFa,any_star>>;
	
	template <const auto & DFa, typename Range> constexpr bool search(Range && range) noexcept {
		return dispatcher<search_wrap<DFa>>::run(range.begin(), range.end());
	}

	template <const auto & DFa, typename Ptr> constexpr bool search_ptr(const Ptr * ptr) noexcept {
		return dispatcher<search_wrap<DFa>>::run(ptr, ctfa::zero_terminated_string_end_iterator());
	}

	template <const auto & Dfa, typename CharT> struct table_dispatcher {
		static constexpr size_t size = Dfa.transitions.size() + Dfa.final_states.size() + 1;
		
		static constexpr auto index_example = []{
			if constexpr (size < (256 - 1)) {
				return uint8_t(0);
			} else if constexpr (size < (65536 - 1)) {
				return uint16_t(0);
			} else {
				return uint32_t(0);
			}
		}();
		
		using index_type = uint16_t;
		
		using char_type = char32_t;
	
		struct jump_extended {
			index_type source;
			index_type target;
			char_type low;
			char_type high;
			constexpr bool operator<(const jump_extended & rhs) const {
				if (source == rhs.source) {
					if (target == rhs.target) {
						if (low == rhs.low) {
							return high < rhs.high;
						} else {
							return low < rhs.low;
						}
					} else {
						return target < rhs.target;
					}
				} else {
					return source < rhs.source;
				}
			}
			constexpr bool operator==(const jump_extended & rhs) const {
				return source == rhs.source && target == rhs.target && low == rhs.low && high == rhs.high;
			}
			constexpr bool operator<(index_type idx) const {
				return source < idx;
			}
			constexpr bool operator==(index_type idx) const {
				return source == idx;
			}
		};

		static constexpr auto build_presort() {
			ctfa::set<jump_extended, size*2> presort;
			for (const auto & t: Dfa.transitions) {
				presort.insert(jump_extended{index_type(t.source.id), index_type(t.target.id), char_type(t.cond.r.low), char_type(t.cond.r.high)});
				if (!Dfa.is_final(t.source)) presort.insert(jump_extended{index_type(t.source.id), std::numeric_limits<index_type>::max(), 0, 0});
			}
			for (state f: Dfa.final_states) {
				presort.insert(jump_extended{index_type(f.id), std::numeric_limits<index_type>::max(), 1, 0});
			}
			presort.insert(jump_extended{std::numeric_limits<index_type>::max(), std::numeric_limits<index_type>::max(), 0, 0});
			return presort;
		}
		
		static constexpr auto presort_table = build_presort();
		
		static constexpr auto build_table() {
			ctfa::set<jump_extended, presort_table.size()> out;
			
			for (const auto & j: presort_table) {
				if (j.source != std::numeric_limits<index_type>::max()) {
					auto source = presort_table.find(j.source);
					if (j.target != std::numeric_limits<index_type>::max()) {
						auto target = presort_table.find(j.target);
						out.push_back(jump_extended{index_type(source - presort_table.begin()), index_type(target - presort_table.begin()), j.low, j.high});
					} else {
						out.push_back(jump_extended{index_type(source - presort_table.begin()), j.target, j.low, j.high});
					}
				} else {
					out.push_back(jump_extended{j.source, j.target, j.low, j.high});
				}
				
			}
			return out;
			
		}
	    
		static constexpr auto jump_table = build_table();
		
		template <typename Iterator, typename EndIterator> static constexpr bool run(Iterator it, const EndIterator end) noexcept {
			index_type state = 0; // start
			index_type current = state;
			
			while (true) {
				if (end == it) {
					//std::cout << "hit end == it => ";
					while (true) {
						const auto & jump = jump_table[state++];
						if (~jump.target) {
							//std::cout << (jump.low ? "ACCEPT\n" : "REJECT\n");
							return jump.low;
						}
						else continue;
					}
				}
				
				//std::cout << "current = " << current << ", state = " << state; 
				const auto & jump = jump_table[state++];
				//std::cout << ", target = " << jump.target << "\n";
				
				if (jump.source != current) {
					//std::cout << " end of state => REJECT";
					return false;
				}
				
				// the final mark can't be matched anyway
				//std::cout << jump.low << " <= it ( " << unsigned(*it) << ") <= " << jump.high << "\n";
				if ((jump.low <= *it) && (*it <= jump.high)) {
					//std::cout << " match char " << *it << "\n";
					current = state = jump.target;
					it++;
				} else {
					//std::cout << " ... \n";
				}
			}
		}
	};
}

#endif
