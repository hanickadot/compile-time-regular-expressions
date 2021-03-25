#ifndef CTRE_V2__CTRE__RANGE__HPP
#define CTRE_V2__CTRE__RANGE__HPP

#include "iterators.hpp"

namespace ctre {

template <typename> constexpr bool is_range = false;

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

template <typename... Ts> constexpr bool is_range<regex_range<Ts...>> = true;

template <typename BeginIterator, typename EndIterator, typename RE, typename ResultIterator = BeginIterator> struct regex_split_range {
	BeginIterator _begin;
	const EndIterator _end;
	
	constexpr CTRE_FORCE_INLINE regex_split_range(BeginIterator begin, EndIterator end) noexcept: _begin{begin}, _end{end} { }
	
	constexpr CTRE_FORCE_INLINE auto begin() const noexcept {
		return regex_split_iterator<BeginIterator, EndIterator, RE, ResultIterator>(_begin, _end);
	}
	constexpr CTRE_FORCE_INLINE auto end() const noexcept {
		return regex_end_iterator{};
	}
};

template <typename... Ts> constexpr bool is_range<regex_split_range<Ts...>> = true;

}

#if __cpp_lib_ranges >= 201911
namespace std::ranges {

	template <typename... Ts> inline constexpr bool enable_borrowed_range<::ctre::regex_range<Ts...>> = true;
	template <typename... Ts> inline constexpr bool enable_borrowed_range<::ctre::regex_split_range<Ts...>> = true;

}
#endif 

#endif
