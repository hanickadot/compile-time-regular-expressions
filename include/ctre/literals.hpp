#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "fixed_string.hpp"
#include "parser.hpp"
#include "grammar.hpp"

namespace ctre {

namespace literals {
	
//	in C++20 :)
//	template <basic_fixed_string input> constexpr auto operator""_ctre() noexcept { 
//		return tape;
//	}
	
// we need object with static linkage, 
// until we got C++20's P0732 "Class Types in Non-Type Template Parameters"

template <typename CharT, CharT... input> static inline constexpr auto string = basic_fixed_string<CharT, sizeof...(input)>({input...}, std::make_index_sequence<sizeof...(input)>());

template <typename CharT, CharT... input> constexpr auto operator""_ctre() noexcept {
	return ctre::parser<math_grammar, string<CharT, input...>>::decide(math_grammar::subject_type());
}

} // literals

} // ctre

#endif
