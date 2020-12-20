#ifndef CTRE_V2__CTRE__RANGE__HPP
#define CTRE_V2__CTRE__RANGE__HPP

#include "iterators.hpp"

namespace ctre {

template <typename BeginIterator, typename EndIterator, typename RE, typename ResultIterator = BeginIterator> struct regex_range {
	BeginIterator _begin;
	const EndIterator _end;
	constexpr CTRE_FORCE_INLINE regex_range(BeginIterator begin, EndIterator end) noexcept: _begin{begin}, _end{end} { }
	
	constexpr CTRE_FORCE_INLINE auto begin() const noexcept {
		return regex_iterator<BeginIterator, EndIterator, RE, ResultIterator>(_begin, _end);
	}
	constexpr CTRE_FORCE_INLINE auto end() const noexcept {
		return regex_end_iterator{};
	}
};

}

#endif
