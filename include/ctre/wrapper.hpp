#ifndef CTRE__CTRE_WRAPPER__HPP
#define CTRE__CTRE_WRAPPER__HPP

#include "evaluation.hpp"
#include "utility.hpp"
#include <string_view>

namespace ctre {

template <typename RE> struct regular_expression {
	constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept { };
	
	constexpr CTRE_FORCE_INLINE static bool match(std::string_view sv) noexcept {
		return evaluate(std::begin(sv), std::end(sv), RE());
	}
	template <typename Iterator> constexpr CTRE_FORCE_INLINE static bool match(Iterator begin, Iterator end) noexcept {
		return match(begin, end);
	}
	constexpr CTRE_FORCE_INLINE bool operator()(std::string_view sv) noexcept {
		return match(sv);
	}
	template <typename Iterator> constexpr CTRE_FORCE_INLINE bool operator()(Iterator begin, Iterator end) noexcept {
		return match(begin, end);
	}
};

template <typename RE> regular_expression(RE) -> regular_expression<RE>;


template <typename RE> constexpr inline bool operator==(regular_expression<RE> re, std::string_view sv) noexcept {
	return re.match(sv);
};

template <typename RE> constexpr inline bool operator==(std::string_view sv, regular_expression<RE> re) noexcept {
	return re.match(sv);
};

template <typename RE> constexpr inline bool operator!=(regular_expression<RE> re, std::string_view sv) noexcept {
	return !re.match(sv);
};

template <typename RE> constexpr inline bool operator!=(std::string_view sv, regular_expression<RE> re) noexcept {
	return !re.match(sv);
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