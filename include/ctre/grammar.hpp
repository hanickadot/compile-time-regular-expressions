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
	
	struct expr_info {
		unsigned variables{0};
		unsigned constants{0};
	};
	
	SUBJECT_TYPE(expr_info);
	
	struct add_variable {
		constexpr auto operator()(expr_info v) noexcept {
			v.variables++;
			return v;
		}
	};
	
	struct add_constant {
		constexpr auto operator()(expr_info v) noexcept {
			v.constants++;
			return v;
		}
	};
	
	RULE(E, term<'('>) -> push<T, E2>;
	RULE(E, range<'a','z'>) -> push<T, E2>;
	RULE(E, range<'0','9'>) -> push<T, E2>;
	
	
	RULE(E2, term<')'>) -> epsilon;
	RULE(E2, term<'+'>) -> push<term<'+'>, T, E2>;
	RULE(E2, term<'-'>) -> push<term<'-'>, T, E2>;
	RULE(E2, epsilon) -> epsilon;
	
	RULE(F, term<'('>) -> push<term<'('>, E, term<')'>>;
	RULE(F, range<'a','z'>) -> push<range<'a','z'>, add_variable>;
	RULE(F, range<'0','9'>) -> push<range<'0','9'>, add_constant>;
	
	
	RULE(T, term<'('>) -> push<F, T2>;
	RULE(T, range<'a','z'>) -> push<F, T2>;
	RULE(T, range<'0','9'>) -> push<F, T2>;
	
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
