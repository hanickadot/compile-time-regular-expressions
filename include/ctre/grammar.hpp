#ifndef CTRE_V2__CTRE__GRAMMAR__HPP
#define CTRE_V2__CTRE__GRAMMAR__HPP

#include "parser.hpp"

namespace ctre {

struct math_grammar {
	START_NONTERM(E);
	NONTERM(E2);
	NONTERM(T);
	NONTERM(T2);
	NONTERM(F);
	
	struct variable_counter {
		unsigned count{0};
	};
	
	struct add_variable {
		constexpr auto operator()(variable_counter v) noexcept {
			v.count++;
			return v;
		}
	};
	
	RULE(E, term<'('>) -> push<T, E2>;
	RULE(E, term<'x'>) -> push<T, E2>;
	RULE(E, term<'y'>) -> push<T, E2>;
	RULE(E, term<'z'>) -> push<T, E2>;
	
	RULE(E2, term<')'>) -> epsilon;
	RULE(E2, term<'+'>) -> push<term<'+'>, T, E2>;
	RULE(E2, term<'-'>) -> push<term<'-'>, T, E2>;
	RULE(E2, epsilon) -> epsilon;
	
	RULE(F, term<'('>) -> push<term<'('>, E, term<')'>>;
	RULE(F, term<'x'>) -> push<term<'x'>, add_variable>;
	RULE(F, term<'y'>) -> push<term<'y'>, add_variable>;
	RULE(F, term<'z'>) -> push<term<'z'>, add_variable>;
	
	RULE(T, term<'('>) -> push<F, T2>;
	RULE(T, term<'x'>) -> push<F, T2>;
	RULE(T, term<'y'>) -> push<F, T2>;
	RULE(T, term<'z'>) -> push<F, T2>;
	
	RULE(T2, term<')'>) -> epsilon;
	RULE(T2, term<'+'>) -> epsilon;
	RULE(T2, term<'-'>) -> epsilon;
	RULE(T2, term<'*'>) -> push<term<'*'>, F, T2>;
	RULE(T2, term<'/'>) -> push<term<'/'>, F, T2>;
	RULE(T2, epsilon) -> epsilon;
};

struct math_grammar_quick {
	struct E { };
	struct E2 { };
	struct T { };
	struct T2 { };
	struct F { };
	
	using start = E;
	
	RULE(E, term<'('>) -> push<term<'('>, E, term<')'>, T2, E2>;
	RULE(E, term<'x'>) -> push<term<'x'>, T2, E2>;
	RULE(E, term<'y'>) -> push<term<'y'>, T2, E2>;
	RULE(E, term<'z'>) -> push<term<'z'>, T2, E2>;
	
	RULE(E2, term<')'>) -> epsilon;
	RULE(E2, term<'+'>) -> push<term<'+'>, T, E2>;
	RULE(E2, term<'-'>) -> push<term<'-'>, T, E2>;
	RULE(E2, epsilon) -> epsilon;
	
	RULE(F, term<'('>) -> push<term<'('>, E, term<')'>>;
	RULE(F, term<'x'>) -> term<'x'>;
	RULE(F, term<'y'>) -> term<'y'>;
	RULE(F, term<'z'>) -> term<'z'>;
	
	RULE(T, term<'('>) -> push<term<'('>, E, term<')'>, T2>;
	RULE(T, term<'x'>) -> push<term<'x'>, T2>;
	RULE(T, term<'y'>) -> push<term<'y'>, T2>;
	RULE(T, term<'z'>) -> push<term<'z'>, T2>;
	
	RULE(T2, term<')'>) -> epsilon;
	RULE(T2, term<'+'>) -> epsilon;
	RULE(T2, term<'-'>) -> epsilon;
	RULE(T2, term<'*'>) -> push<term<'*'>, F, T2>;
	RULE(T2, term<'/'>) -> push<term<'/'>, F, T2>;
	RULE(T2, epsilon) -> epsilon;
};

}

#endif
