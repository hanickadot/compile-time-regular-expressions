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
template <ctll::fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto re() noexcept {
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

template <typename RE> struct regex_match_t {
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto operator()(Args && ... args) const noexcept {
		auto re_obj = ctre::regular_expression<RE>();
		return re_obj.match(std::forward<Args>(args)...);
	}
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto try_extract(Args && ... args) const noexcept {
		return operator()(std::forward<Args>(args)...);
	}
};

template <typename RE> struct regex_search_t {
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto operator()(Args && ... args) const noexcept {
		auto re_obj = ctre::regular_expression<RE>();
		return re_obj.search(std::forward<Args>(args)...);
	}
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto try_extract(Args && ... args) const noexcept {
		return operator()(std::forward<Args>(args)...);
	}
};

#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))

template <auto input> struct regex_builder {
	static constexpr auto _input = input;
	using _tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(_tmp(), "Regular Expression contains syntax error.");
	using type = ctll::conditional<(bool)(_tmp()), decltype(ctll::front(typename _tmp::output_type::stack_type())), ctll::list<reject>>;
};

template <ctll::fixed_string input> static constexpr inline auto match = regex_match_t<typename regex_builder<input>::type>();

template <ctll::fixed_string input> static constexpr inline auto search = regex_search_t<typename regex_builder<input>::type>();

#else

template <auto & input> struct regex_builder {
	using _tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(_tmp(), "Regular Expression contains syntax error.");
	using type = ctll::conditional<(bool)(_tmp()), decltype(ctll::front(typename _tmp::output_type::stack_type())), ctll::list<reject>>;
};

template <auto & input> static constexpr inline auto match = regex_match_t<typename regex_builder<input>::type>();

template <auto & input> static constexpr inline auto search = regex_search_t<typename regex_builder<input>::type>();

#endif


}

#endif
