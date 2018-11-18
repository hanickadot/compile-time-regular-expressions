#ifndef CTRE_V2__CTRE__FUNCTIONS__HPP
#define CTRE_V2__CTRE__FUNCTIONS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"
#include "simple.hpp"
#include "evaluation.hpp"
#include "wrapper.hpp"
#include "id.hpp"

namespace ctre {

// avoiding CTAD limitation in C++17
template <typename CharT, size_t N> class pattern: public ctll::basic_fixed_string<CharT, N> {
	using parent = ctll::basic_fixed_string<CharT, N>;
public:
	constexpr pattern(const CharT (&input)[N]) noexcept: parent(input) { }
};

#if __cpp_deduction_guides
template <typename CharT, size_t N> pattern(const CharT (&)[N]) -> pattern<CharT, N>;
#endif

// for better examples
template <typename CharT, size_t N> class fixed_string: public ctll::basic_fixed_string<CharT, N> {
	using parent = ctll::basic_fixed_string<CharT, N>;
public:
	constexpr fixed_string(const CharT (&input)[N]) noexcept: parent(input) { }
};

#if __cpp_deduction_guides
template <typename CharT, size_t N> fixed_string(const CharT (&)[N]) -> fixed_string<CharT, N>;
#endif

#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {
#else
template <auto & input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {	
	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}
#endif

// in moment when we get C++20 support this will start to work :)

#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(std::string_view sv) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.match(sv);
}
#else
template <auto & input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(std::string_view sv) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.match(sv);
}
#endif


#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input, typename ForwardIt> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(ForwardIt first, ForwardIt last) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re()).match(first, last);
}
#else
template <auto & input, typename ForwardIt> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(ForwardIt first, ForwardIt last) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.match(first,last);
}
#endif


#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(std::string_view sv) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(sv);
}
#else
template <auto & input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(std::string_view sv) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(sv);
}
#endif


#if __cpp_nontype_template_parameter_class
template <basic_fixed_string input, typename ForwardIt> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(ForwardIt first, ForwardIt last) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(first, last);
}
#else
template <auto & input, typename ForwardIt> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(ForwardIt first, ForwardIt last) noexcept {

	using tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context_t<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(first, last);
}
#endif

}

#endif
