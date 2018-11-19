#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"
#include "simple.hpp"
#include "evaluation.hpp"
#include "wrapper.hpp"
#include "id.hpp"


namespace ctre {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into ctll::basic_fixed_string
// in C++20 we have `class nontype template parameters`

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... input> static inline constexpr auto _fixed_string_reference = ctll::ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});
#endif	

namespace literals {
	
#if false

#ifndef _MSC_VER
	
// add this when we will have concepts
// requires ctll::parser<ctre::pcre, _fixed_string_reference<CharT, charpack...>, ctre::pcre_actions>::template correct_with<pcre_context<>>

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
#endif
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}



// this will need to be fixed with C++20
#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_id() noexcept {
	return id<charpack...>();
}
#endif

#endif

}

namespace test_literals {
	
#ifndef _MSC_VER

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr inline auto operator""_ctre_test() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr inline auto operator""_ctre_test() noexcept {
#endif
	return ctll::parser<ctre::pcre, input>::correct;
}

#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr inline auto operator""_ctre_gen() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr inline auto operator""_ctre_gen() noexcept {
#endif
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type::stack_type();
}


#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_syntax() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_syntax() noexcept {
#endif
	return ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template correct_with<pcre_context<>>;
}


#if !__cpp_nontype_template_parameter_class
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr inline auto operator""_simple_test() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr inline auto operator""_simple_test() noexcept {
#endif
	return ctll::parser<ctre::simple, input>::correct;
}

#endif

#endif


} // literals

} // ctre

#endif
