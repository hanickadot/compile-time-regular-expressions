#if false
#include <ctfa/fa.hpp>
#include <ctfa/reachability.hpp>
#include <ctfa/minimize.hpp>
#include <ctfa/determinize.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

template <typename T> struct is_minimize_tmp: std::false_type { };
template <typename T> struct is_minimize_tmp<ctfa::filtered_out<T>>: std::true_type { };

template <auto & fa> void debug_minimize() {
	//if constexpr (is_minimize_tmp<decltype(fa)>()) {
	//	debug_minimize_tmp(fa);
	//} else {
	//my_debug<fa>();
	//}
	//debug_minimize_tmp(fa);
}

template <const auto & a> void determinize_debug() {
	std::cout << (ctfa::is_deterministic<a> ? "dfa\n" : "nfa\n");
	
	std::cout << "determinized: \n";
	auto ret = ctfa::determinize_impl<a>::iterate_over_transitions([](const auto & source, auto range, const auto & states, bool secondary, ctfa::state s) {
		bool start = false;
		if (ctfa::is_final(source)) {
			if (source.size() == 1 && source[0].s.is_start()) std::cout << "<-> ";
			else std::cout << " <- ";
		} else {
			if (source.size() == 1 && source[0].s.is_start()) std::cout << "--> ";
			else std::cout << "    ";
		}
		bool first = true;
		for (auto t: source) {
			if (first) first = false;
			else std::cout << ",";
			std::cout << t.s.id;
		}
		std::cout << " x ";
		range.debug([](auto && ... c){
			(std::cout << ... << c);
		});
		std::cout << " -> ";
		first = true;
		for (auto t: states) {
			if (first) first = false;
			else std::cout << ",";
			std::cout << t.s.id;
		}
		if (secondary) std::cout << " (secondary: " << s.id << ")";
		std::cout << "\n";
	}, [](const auto & final_states){
		std::cout << " <- ";
		bool first = true;
		for (auto t: final_states) {
			if (first) first = false;
			else std::cout << ",";
			std::cout << t.s.id;
		}
		std::cout << "\n";
	});
	std::cout << "transitions = " << ret.transitions << "\n";
	std::cout << "final states = " << ret.final_states << "\n";
	std::cout << "nfa = " << ret.nfa << "\n";
	std::cout << "nfa max length = " << ret.nfa_max_length << "\n";

	std::cout << "\n\n";
}

template <size_t A, size_t B> void debug(const ctfa::states_to_state<A,B> & value) {
	for (const auto & map: value.data) {
		for (ctfa::state s: map.source_states) {
			std::cout << s.id << " ";
		}
		std::cout << " => " << map.s.id << "\n";
	}
}

int main() {
	static constexpr auto a = ctfa::finite_automaton<3,1>{
		{
			ctfa::transition{ctfa::state{0}, ctfa::state{0}, ctfa::matcher::range<'a','z'>},
			ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'d'>},
			ctfa::transition{ctfa::state{1}, ctfa::state{1}, ctfa::matcher::range<'a','z'>}
		}, {
			ctfa::state{1}
		}
	};
	my_debug<a>();
	
	
	determinize_debug<a>();
	static constexpr auto out1 = ctfa::determinize<a>;
	static constexpr auto out1_names = ctfa::mapping<a>;
	my_debug<out1>();
	debug(out1_names);
	
	std::cout << "---+++---------\n";
	
	static constexpr auto out2 = ctfa::determinize<out1>;
	static constexpr auto out2_names = ctfa::mapping<a>;

	my_debug<out2>();
	debug(out2_names);
	
	//static constexpr auto min = ctfa::minimize<out>;
	//
	//my_debug<min>();
}
#endif

int main() {
	
}