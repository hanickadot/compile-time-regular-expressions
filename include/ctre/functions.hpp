#ifndef CTRE_V2__CTRE__FUNCTIONS__HPP
#define CTRE_V2__CTRE__FUNCTIONS__HPP

#include "../ctll.hpp"
#include "pcre_actions.hpp"
#include "evaluation.hpp"
#include "wrapper.hpp"
#include "id.hpp"

namespace ctre {

#if !__cpp_nontype_template_parameter_class
// avoiding CTAD limitation in C++17
template <typename CharT, size_t N> class pattern: public ctll::basic_fixed_string<CharT, N> {
	using parent = ctll::basic_fixed_string<CharT, N>;
public:
	constexpr pattern(const CharT (&input)[N]) noexcept: parent(input) { }
};

template <typename CharT, size_t N> pattern(const CharT (&)[N]) -> pattern<CharT, N>;

// for better examples
template <typename CharT, size_t N> class fixed_string: public ctll::basic_fixed_string<CharT, N> {
	using parent = ctll::basic_fixed_string<CharT, N>;
public:
	constexpr fixed_string(const CharT (&input)[N]) noexcept: parent(input) { }
};

template <typename CharT, size_t N> fixed_string(const CharT (&)[N]) -> fixed_string<CharT, N>;
#endif

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {
constexpr auto _input = input; // workaround for GCC 9 bug 88092
#else
template <auto & input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {	
constexpr auto & _input = input; 
#endif
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}


// in moment when we get C++20 support this will start to work :)

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(Args && ... args) noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression<re>(re());
	return re_obj.match(std::forward<Args>(args)...);
}
#else
template <auto & input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto match(Args && ... args) noexcept {
	constexpr auto & _input = input; 
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression<re>(re());
	return re_obj.match(std::forward<Args>(args)...);
}
#endif

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(Args && ... args) noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(std::forward<Args>(args)...);
}
#else
template <auto & input, typename... Args> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto search(Args && ... args) noexcept {
	constexpr auto & _input = input; 
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(ctll::front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return re_obj.search(std::forward<Args>(args)...);
}
#endif

}

#endif
