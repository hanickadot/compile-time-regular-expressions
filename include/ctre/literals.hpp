#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"
#include "simple.hpp"
#include "evaluation.hpp"
#include "wrapper.hpp"
#include "id.hpp"


namespace ctre {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into basic_fixed_string
// in C++20 we have `class nontype template parameters`

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... input> static inline constexpr auto _fixed_string_reference = ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});
#endif	

namespace literals {
	
#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_fixed_pcre() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_fixed_pcre() noexcept {
#endif
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}

// add this when we will have concepts
// requires ctll::parser<ctre::pcre, _fixed_string_reference<CharT, charpack...>, ctre::pcre_actions>::template correct_with<pcre_context<>>

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_pcre() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_pcre() noexcept {
#endif
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}


// duplicate for _ctre

// add this when we will have concepts
// requires ctll::parser<ctre::pcre, _fixed_string_reference<CharT, charpack...>, ctre::pcre_actions>::template correct_with<pcre_context<>>

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
#endif
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}



// this will need to be fixed with C++20
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_ctre_id() noexcept {
	return id<charpack...>();
}

}

namespace test_literals {

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre_test() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre_test() noexcept {
#endif
	return ctll::parser<ctre::pcre, input>::correct;
}

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_pcre_gen() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_pcre_gen() noexcept {
#endif
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type::stack_type();
}


#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_pcre_syntax() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto operator""_pcre_syntax() noexcept {
#endif
	return ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template correct_with<pcre_context<>>;
}


#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr inline auto operator""_simple_test() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr inline auto operator""_simple_test() noexcept {
#endif
	return ctll::parser<ctre::simple, input>::correct;
}


} // literals

} // ctre

#endif
