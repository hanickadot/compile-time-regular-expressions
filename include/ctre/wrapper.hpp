#ifndef CTRE__WRAPPER__HPP
#define CTRE__WRAPPER__HPP

#include "evaluation.hpp"
#include "utility.hpp"
#include <string_view>

namespace ctre {

template <typename RE> struct regular_expression {
	constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept { };
	
	constexpr CTRE_FORCE_INLINE static auto match(std::string_view sv) noexcept {
		return match_re(std::begin(sv), std::end(sv), RE());
	}
	template <typename Iterator> constexpr CTRE_FORCE_INLINE static auto match(Iterator begin, Iterator end) noexcept {
		return match(begin, end);
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::string_view sv) noexcept {
		return match(sv);
	}
	template <typename Iterator> constexpr CTRE_FORCE_INLINE auto operator()(Iterator begin, Iterator end) noexcept {
		return match(begin, end);
	}
};

template <typename RE> regular_expression(RE) -> regular_expression<RE>;


template <typename RE> constexpr inline bool operator==(regular_expression<RE> re, std::string_view sv) noexcept {
	return bool(re.match(sv));
};

template <typename RE> constexpr inline bool operator==(std::string_view sv, regular_expression<RE> re) noexcept {
	return bool(re.match(sv));
};

template <typename RE> constexpr inline bool operator!=(regular_expression<RE> re, std::string_view sv) noexcept {
	return !bool(re.match(sv));
};

template <typename RE> constexpr inline bool operator!=(std::string_view sv, regular_expression<RE> re) noexcept {
	return !bool(re.match(sv));
};


template <typename RE> constexpr inline bool operator==(regular_expression<RE>, regular_expression<RE>) noexcept {
	return true;
};

template <typename RE1, typename RE2> constexpr inline bool operator==(regular_expression<RE1>, regular_expression<RE2>) noexcept {
	return false;
};

template <typename RE> constexpr inline bool operator!=(regular_expression<RE>, regular_expression<RE>) noexcept {
	return false;
};

template <typename RE1, typename RE2> constexpr inline bool operator!=(regular_expression<RE1>, regular_expression<RE2>) noexcept {
	return true;
};


}

#endif