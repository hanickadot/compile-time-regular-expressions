#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "../ctll.hpp"
#include "pcre-actions.hpp"



namespace ctre {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into basic_fixed_string

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... input> static inline constexpr auto _string_reference = ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});
#endif	

namespace literals {
	
#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre() noexcept {
	constexpr auto & input = _string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre() noexcept {
#endif
	constexpr auto out = ctll::parser<ctre::pcre, input>::template decide<ctll::list<>>();
	return bool(out);
}

}

namespace test_literals {

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre_test() noexcept {
	constexpr auto & input = _string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre_test() noexcept {
#endif
	return bool(ctll::parser<ctre::pcre, input>::decide());
}

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre_gen() noexcept {
	constexpr auto & input = _string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre_gen() noexcept {
#endif
	return ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template decide<ctll::list<>>();
}


} // literals

} // ctre

#endif
