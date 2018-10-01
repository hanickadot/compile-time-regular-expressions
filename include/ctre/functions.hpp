#ifndef CTRE_V2__CTRE__FUNCTIONS__HPP
#define CTRE_V2__CTRE__FUNCTIONS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"
#include "simple.hpp"
#include "evaluation.hpp"
#include "wrapper.hpp"
#include "id.hpp"

namespace ctre {

// in moment when we get C++20 support this will start to work :)

#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto match(std::string_view sv) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::float_regular_expression(re()).match(sv);
}
#endif


#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input, typename ForwardIt> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto match(ForwardIt first, ForwardIt last) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::float_regular_expression(re()).match(first, last);
}
#endif


#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto search(std::string_view sv) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re()).match(sv);
}
#endif


#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input, typename ForwardIt> __attribute__((flatten)) constexpr CTRE_FORCE_INLINE auto search(ForwardIt first, ForwardIt last) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re()).match(first, last);
}
#endif

}

#endif
