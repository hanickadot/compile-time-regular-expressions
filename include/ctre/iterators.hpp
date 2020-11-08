#ifndef CTRE_V2__CTRE__ITERATOR__HPP
#define CTRE_V2__CTRE__ITERATOR__HPP

#include "literals.hpp"
#include "wrapper.hpp"

namespace ctre {

// TODO make proper iterator traits here

struct regex_end_iterator {
	constexpr regex_end_iterator() noexcept { }
};

template <typename BeginIterator, typename EndIterator, typename RE, typename ResultIterator = BeginIterator> struct regex_iterator {
	BeginIterator orig_begin;
	BeginIterator current;
	const EndIterator end;
	decltype(RE::template exec_with_result_iterator<ResultIterator>(current, end)) current_match;

	constexpr CTRE_FORCE_INLINE regex_iterator(BeginIterator begin, EndIterator end) noexcept: orig_begin{begin}, current{begin}, end{end}, current_match{RE::template exec_with_result_iterator<ResultIterator>(current, end)} {
		if (current_match) {
			current = current_match.template get<0>().end();
		}
	}
	constexpr CTRE_FORCE_INLINE const auto & operator*() const noexcept {
		return current_match;
	}
	constexpr CTRE_FORCE_INLINE regex_iterator & operator++() noexcept {
		if (current == end) {
			current_match = decltype(current_match){};
			return *this;
		}
		
		current_match = RE::template exec_with_result_iterator<ResultIterator>(orig_begin, current, end);
		
		if (current_match) {
			current = current_match.template get<0>().end();
		}
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_iterator operator++(int) noexcept {
		auto previous = *this;
		this->operator++();
		return previous;
	}
	friend constexpr CTRE_FORCE_INLINE bool operator!=(const regex_iterator<BeginIterator, EndIterator, RE, ResultIterator> & left, regex_end_iterator) {
		return bool(left.current_match);
	}
	friend constexpr CTRE_FORCE_INLINE bool operator!=(regex_end_iterator, const regex_iterator<BeginIterator, EndIterator, RE, ResultIterator> & right) {
		return bool(right.current_match);
	}
};

} // ctre

#endif
