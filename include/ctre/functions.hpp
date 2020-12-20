#ifndef CTRE_V2__CTRE__FUNCTIONS__HPP
#define CTRE_V2__CTRE__FUNCTIONS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"
#include "evaluation.hpp"
#include "wrapper.hpp"
#include "id.hpp"

namespace ctre {

#if !(__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
// avoiding CTAD limitation in C++17
template <typename CharT, size_t N> class pattern: public ctll::fixed_string<N> {
	using parent = ctll::fixed_string<N>;
public:
	constexpr pattern(const CharT (&input)[N]) noexcept: parent(input) { }
};

template <typename CharT, size_t N> pattern(const CharT (&)[N]) -> pattern<CharT, N>;

// for better examples
template <typename CharT, size_t N> class fixed_string: public ctll::fixed_string<N> {
	using parent = ctll::fixed_string<N>;
public:
	constexpr fixed_string(const CharT (&input)[N]) noexcept: parent(input) { }
};

template <typename CharT, size_t N> fixed_string(const CharT (&)[N]) -> fixed_string<CharT, N>;
#endif

#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <ctll::fixed_string input, typename Modifier = void> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {
constexpr auto _input = input; // workaround for GCC 9 bug 88092
#else
template <auto & input, typename Modifier = void> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {	
constexpr auto & _input = input; 
#endif
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using regex = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression<regex, Modifier, singleline>();
}

}

#endif
