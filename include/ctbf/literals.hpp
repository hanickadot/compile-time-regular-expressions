#ifndef CTRE_V2__BRAINFUCK__LITERALS__HPP
#define CTRE_V2__BRAINFUCK__LITERALS__HPP

#include "../ctll.hpp"
#include "actions.hpp"
#include "utility.hpp"
#include "wrapper.hpp"
//#include "evaluation.hpp"
//#include "wrapper.hpp"
//#include "id.hpp"

namespace ctbf {

// in C++17 (clang & gcc with gnu extension) we need translate character pack into basic_fixed_string
// in C++20 we have `class nontype template parameters`

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... input> static inline constexpr auto _fixed_string_reference = ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});
#endif	

namespace literals {


#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr CTBF_FORCE_INLINE auto operator""_ctbf() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTBF_FORCE_INLINE auto operator""_ctbf() noexcept {
#endif
	using tmp = typename ctll::parser<ctbf::brainfuck, input, ctbf::brainfuck_actions>::template output<ctll::list<>>;
	static_assert(tmp(), "Brainfuck code contains syntax error.");
	using c = decltype(front(typename tmp::output_type()));
	return ctbf::code<c>(c());
}

}

namespace test_literals {


#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... charpack> __attribute__((flatten)) constexpr CTBF_FORCE_INLINE auto operator""_ctbf_syntax() noexcept {
	constexpr auto & input = _fixed_string_reference<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTBF_FORCE_INLINE auto operator""_ctbf_syntax() noexcept {
#endif
	return ctll::parser<ctbf::brainfuck, input, ctbf::brainfuck_actions>::template correct_with<ctll::list<>>;
}


} // literals

} // ctre

#endif
