#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "../ctll.hpp"
#include "pcre.hpp"



namespace ctre {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into basic_fixed_string

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... input> static inline constexpr auto string = ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});
#endif	

namespace literals {
	
#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((always_inline)) constexpr auto operator""_pcre() noexcept {
	constexpr auto & input = string<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((always_inline)) constexpr auto operator""_pcre() noexcept {
#endif
	constexpr auto out = ctll::parser<ctre::pcre, input>::decide();
	return bool(out);
}

}

namespace test_literals {

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((always_inline)) constexpr auto operator""_pcre_test() noexcept {
	constexpr auto & input = string<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((always_inline)) constexpr auto operator""_pcre_test() noexcept {
#endif
	constexpr auto out = ctll::parser<ctre::pcre, input>::decide();
	return bool(out);
}

} // literals

} // ctre

#endif
