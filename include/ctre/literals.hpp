#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"



namespace ctre {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into basic_fixed_string
// in C++20 we have `class nontype template parameters`

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... input> static inline constexpr auto _fixed_string_reference = ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});
#endif	

namespace literals {
	
#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre() noexcept {
#endif
	using result = typename ctll::parser<ctre::pcre, input>::template output<ctll::list<>>;
	static_assert(result(), "Regular Expression contains syntax error.");
	return result::output_type();
}

}

namespace test_literals {

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre_test() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre_test() noexcept {
#endif
	return ctll::parser<ctre::pcre, input>::correct;
}

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre_gen() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre_gen() noexcept {
#endif
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<ctll::list<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type();
}


} // literals

} // ctre

#endif
