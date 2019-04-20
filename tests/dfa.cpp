#include <iostream>
#include "ctre.hpp"
#include "../include/ctre/translate_dfa.hpp"
#include "../include/ctfa/include/ctfa/basic/debug.hpp"

template <typename... T> struct id_type;

using namespace ctre::test_literals;

#if !__cpp_nontype_template_parameter_class
#define CTRE_GEN(pattern) (pattern ## _ctre_gen)
#else


template <ctll::fixed_string input> constexpr auto gen() {
	constexpr auto _input = input;
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<ctre::pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type::stack_type();
}


#define CTRE_GEN(pattern) gen<pattern>()

#endif

template <typename A, typename B> constexpr auto same(A,B) -> std::false_type { return {}; }

template <typename A> constexpr auto same(A,A) -> std::true_type { return {}; }

template <typename A, typename B> constexpr bool same_f(A a,B b) {
	return same(ctll::front(a), b);
}

template <typename Fa, typename TransitionCB, typename FinalCB> void debug_fa(const Fa & fa, TransitionCB && tran, FinalCB && final) {
	for (const auto & t: fa.transitions) {
		tran(t.source, t.target, t.cond, fa.is_final(t.source));
	}
	for (const auto & f: fa.final_states) {
		if (fa.transitions.find(f) == fa.transitions.end()) final(f);
	}
}

template <typename Fa> void my_debug(const Fa & fa) {
	debug_fa(fa, [](ctfa::state s, ctfa::state t, ctfa::condition cond, bool final){
		if (final) {
			if (s.is_start()) std::cout << "<-> ";
			else std::cout << " <- ";
		} else {
			if (s.is_start()) std::cout << "--> ";
			else std::cout << "    ";
		}
		std::cout << s.id << " x ";
		
		cond.debug([](auto && ... c){
			(std::cout << ... << c);
		});
		
		std::cout <<" => " << t.id << "\n";
	},[](ctfa::state f){
		std::cout << " <- " << f.id << "\n";
	});
	std::cout << "------------\n";
}



[[maybe_unused]] static constexpr auto value = ctre::translate_dfa(CTRE_GEN("(?:a|b)?x+.*+(www)[a-z]"));
[[maybe_unused]] static constexpr auto value2 = ctre::translate_dfa(CTRE_GEN("[Hh]ello [A-Za-z]+"));

int main() {
	my_debug(value2);
}
