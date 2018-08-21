#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "fixed_string.hpp"
#include "parser.hpp"
#include "grammar.hpp"
#include "pcre.hpp"

namespace ctre {
	
template <typename CharT, CharT... input> static inline constexpr auto string = basic_fixed_string<CharT, sizeof...(input)>({input...});

namespace literals {
	
//	in C++20 :)
//	template <basic_fixed_string input> constexpr auto operator""_ctre() noexcept { 
//		return tape;
//	}
	
// we need object with static linkage, 
// until we got C++20's P0732 "Class Types in Non-Type Template Parameters"

template <typename CharT, CharT... input> __attribute__((always_inline)) constexpr auto operator""_expr() noexcept {
	return ctre::parser<math_grammar_quick, string<CharT, input...>>::decide();
	//return ctre::parser<math_grammar_quick, string<CharT, input...>>::decide(math_grammar_quick::subject_type());
}

template <typename CharT, CharT... input> __attribute__((always_inline)) constexpr auto operator""_ctre() noexcept {
	using Out = decltype(ctre::parser<pcre, string<CharT, input...>>::decide(ctre::empty_subject()));
	static_assert(Out(), "Regular expression is not correct.");
	return Out();
	//return ctre::parser<math_grammar_quick, string<CharT, input...>>::decide(math_grammar_quick::subject_type());
}

}

namespace test_literals {

template <typename CharT, CharT... input> __attribute__((always_inline)) constexpr auto operator""_pcre_test() noexcept {
	return bool(ctre::parser<pcre, string<CharT, input...>>::decide(ctre::empty_subject()));
	//return ctre::parser<math_grammar_quick, string<CharT, input...>>::decide(math_grammar_quick::subject_type());
}

} // literals

} // ctre

#endif
