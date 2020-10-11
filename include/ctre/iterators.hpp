#ifndef CTRE_V2__CTRE__ITERATOR__HPP
#define CTRE_V2__CTRE__ITERATOR__HPP

#include "literals.hpp"
#include "functions.hpp"

namespace ctre {

struct regex_end_iterator {
	constexpr regex_end_iterator() noexcept { }
};

template <typename BeginIterator, typename EndIterator, typename RE> struct regex_iterator {
	BeginIterator current;
	const EndIterator end;
	decltype(RE::search_2(std::declval<BeginIterator>(), std::declval<EndIterator>())) current_match;

	constexpr regex_iterator(BeginIterator begin, EndIterator end) noexcept: current{begin}, end{end}, current_match{RE::search_2(current, end)} {
		if (current_match) {
			current = current_match.template get<0>().end();
		}
	}
	constexpr const auto & operator*() const noexcept {
		return current_match;
	}
	constexpr regex_iterator & operator++() noexcept {
		current_match = RE::search_2(current, end);
		if (current_match) {
			current = current_match.template get<0>().end();
		}
		return *this;
	}
	constexpr regex_iterator operator++(int) noexcept {
		auto previous = *this;
		current_match = RE::search_2(current, end);
		if (current_match) {
			current = current_match.template get<0>().end();
		}
		return previous;
	}
};

template <typename BeginIterator, typename EndIterator, typename RE> constexpr bool operator!=(const regex_iterator<BeginIterator, EndIterator, RE> & left, regex_end_iterator) {
	return bool(left.current_match);
}

template <typename BeginIterator, typename EndIterator, typename RE> constexpr bool operator!=(regex_end_iterator, const regex_iterator<BeginIterator, EndIterator, RE> & right) {
	return bool(right.current_match);
}

template <typename BeginIterator, typename EndIterator, typename RE> constexpr auto iterator(BeginIterator begin, EndIterator end, RE) noexcept {
	return regex_iterator<BeginIterator, EndIterator, RE>(begin, end);
}

constexpr auto iterator() noexcept {
	return regex_end_iterator{};
}

template <typename Subject, typename RE> constexpr auto iterator(const Subject & subject, RE re) noexcept {
	return iterator(subject.begin(), subject.end(), re);
}



#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <ctll::fixed_string input, typename BeginIterator, typename EndIterator> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto iterator(BeginIterator begin, EndIterator end) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return iterator(begin, end, re());
}
#endif


#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
template <ctll::fixed_string input, typename Subject> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto iterator(const Subject & subject) noexcept {
	constexpr auto _input = input;
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return iterator(subject.begin(), subject.end(), re());
}
#endif





} // ctre

#endif
