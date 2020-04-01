#ifndef CTRE_V2__CTRE__RANGE__HPP
#define CTRE_V2__CTRE__RANGE__HPP

#include "iterators.hpp"

namespace ctre {

template <typename BeginIterator, typename EndIterator, typename RE> struct regex_range {
	BeginIterator _begin;
	const EndIterator _end;
	constexpr regex_range(BeginIterator begin, EndIterator end) noexcept: _begin{begin}, _end{end} { }
	
	constexpr auto begin() const noexcept {
		return regex_iterator<BeginIterator, EndIterator, RE>(_begin, _end);
	}
	constexpr auto end() const noexcept {
		return regex_end_iterator{};
	}
};


template <typename BeginIterator, typename EndIterator, typename RE> constexpr auto range(BeginIterator begin, EndIterator end, RE) noexcept {
	return regex_range<BeginIterator, EndIterator, RE>(begin, end);
}

#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <ctll::fixed_string input, typename BeginIterator, typename EndIterator> constexpr auto range(BeginIterator begin, EndIterator end) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return range(begin, end, re_obj);
}
#endif

template <typename Subject, typename RE> constexpr auto range(const Subject & subject, RE re) noexcept {
	return range(subject.begin(), subject.end(), re);
}

template <typename RE> constexpr auto range(const char * subject, RE re) noexcept {
	return range(subject, zero_terminated_string_end_iterator(), re);
}

#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <ctll::fixed_string input, typename Subject> constexpr auto range(const Subject & subject) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return range(subject.begin(), subject.end(), re_obj);
}
#else
template <auto & input, typename Subject> constexpr auto range(const Subject & subject) noexcept {
	constexpr auto & _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	auto re_obj = ctre::regular_expression(re());
	return range(subject.begin(), subject.end(), re_obj);
}
#endif





}

#endif
