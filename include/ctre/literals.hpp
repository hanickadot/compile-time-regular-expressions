#ifndef CTRE_V2__CTRE__LITERALS__HPP
#define CTRE_V2__CTRE__LITERALS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"
#include "evaluation.hpp"
#include "wrapper.hpp"
#include "id.hpp"

#ifndef __EDG__

namespace ctre {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into ctll::fixed_string
// in C++20 we have `class nontype template parameters`

#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <typename CharT, CharT... input> static inline constexpr auto _fixed_string_reference = ctll::fixed_string< sizeof...(input)>({input...});
#endif	

namespace literals {
	
// clang and GCC <9 supports LITERALS with packs

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#define CTRE_ENABLE_LITERALS
#endif

#ifdef __INTEL_COMPILER
// not enable literals
#elif defined __GNUC__
#if __GNUC__ < 9
#define CTRE_ENABLE_LITERALS
#elif __GNUC__ >= 10
#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L)) 
// newer versions of GCC will give error when trying to use GNU extension
#else
#define CTRE_ENABLE_LITERALS
#endif	
#endif
#endif

#ifdef CTRE_ENABLE_LITERALS
	
// add this when we will have concepts
// requires ctll::parser<ctre::pcre, _fixed_string_reference<CharT, charpack...>, ctre::pcre_actions>::template correct_with<pcre_context<>>

#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	if constexpr (tmp()) {
		using re = decltype(front(typename tmp::output_type::stack_type()));
		return ctre::regular_expression(re());
	} else {
		return ctre::regular_expression(reject());
	}
}



// this will need to be fixed with C++20
#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_id() noexcept {
	return id<charpack...>();
}
#endif

#endif // CTRE_ENABLE_LITERALS

}

namespace test_literals {
	
#ifdef CTRE_ENABLE_LITERALS

#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr inline auto operator""_ctre_test() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::fixed_string input> CTRE_FLATTEN constexpr inline auto operator""_ctre_test() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	return ctll::parser<ctre::pcre, _input>::template correct_with<>;
}

#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr inline auto operator""_ctre_gen() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::fixed_string input> CTRE_FLATTEN constexpr inline auto operator""_ctre_gen() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type::stack_type();
}


#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <typename CharT, CharT... charpack> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_syntax() noexcept {
	constexpr auto & _input = _fixed_string_reference<CharT, charpack...>;
#else
template <ctll::fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto operator""_ctre_syntax() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
#endif
	return ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template correct_with<pcre_context<>>;
}


#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // literals

} // ctre

#endif

#endif
