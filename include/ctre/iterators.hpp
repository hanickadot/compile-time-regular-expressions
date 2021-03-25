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

template <typename BeginIterator, typename EndIterator, typename RE, typename ResultIterator = BeginIterator> struct regex_split_iterator {
	BeginIterator orig_begin;
	BeginIterator current;
	const EndIterator end;
	decltype(RE::template exec_with_result_iterator<ResultIterator>(current, end)) current_match;
	bool last_match{false};

	constexpr void modify_match() {
		auto tmp = current_match.template get<0>().end();
		current_match.set_end_mark(current_match.template get<0>().begin());
		current_match.set_start_mark(current);
		current = tmp;
	}
	
	constexpr void match_rest() {
		// the end is there set by search_method
		current_match.set_start_mark(current);
		current_match.matched();
		current = current_match.template get<0>().end();
		last_match = true;
	}

	constexpr CTRE_FORCE_INLINE regex_split_iterator(BeginIterator begin, EndIterator end) noexcept: orig_begin{begin}, current{begin}, end{end}, current_match{RE::template exec_with_result_iterator<ResultIterator>(current, end)} {
		if (current_match) {
			modify_match();
		} else {
			match_rest();
		}
	}
	constexpr CTRE_FORCE_INLINE const auto & operator*() const noexcept {
		return current_match;
	}
	constexpr CTRE_FORCE_INLINE regex_split_iterator & operator++() noexcept {
		if (current == end && last_match) {
			current_match = decltype(current_match){};
			return *this;
		}
	
		current_match = RE::template exec_with_result_iterator<ResultIterator>(orig_begin, current, end);
	
		if (current_match) {
			modify_match();
		} else {
			match_rest();
		}
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_split_iterator operator++(int) noexcept {
		auto previous = *this;
		this->operator++();
		return previous;
	}
	friend constexpr CTRE_FORCE_INLINE bool operator!=(const regex_split_iterator<BeginIterator, EndIterator, RE, ResultIterator> & left, regex_end_iterator) {
		return bool(left.current_match);
	}
	friend constexpr CTRE_FORCE_INLINE bool operator!=(regex_end_iterator, const regex_split_iterator<BeginIterator, EndIterator, RE, ResultIterator> & right) {
		return bool(right.current_match);
	}
};

} // ctre

#endif
