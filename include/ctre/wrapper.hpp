#ifndef CTRE__WRAPPER__HPP
#define CTRE__WRAPPER__HPP

#include "evaluation.hpp"
#include "utility.hpp"
#include <string_view>

namespace ctre {

template <typename RE> struct regular_expression {
	constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept { };
	
	template <typename Iterator> constexpr CTRE_FORCE_INLINE auto operator()(Iterator begin, Iterator end) noexcept {
		return match(begin, end);
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::string_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::wstring_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::u16string_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::u32string_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	template <typename Iterator> constexpr CTRE_FORCE_INLINE static auto match(Iterator begin, Iterator end) noexcept {
		return match_re(begin, end, RE());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::wstring_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::u16string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::u32string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
};

template <typename RE> regular_expression(RE) -> regular_expression<RE>;




template <typename RE> struct float_regular_expression {
	constexpr CTRE_FORCE_INLINE float_regular_expression(RE) noexcept { };
	
	template <typename Iterator> constexpr CTRE_FORCE_INLINE auto operator()(Iterator begin, Iterator end) noexcept {
		return match(begin, end);
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::string_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::wstring_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::u16string_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	constexpr CTRE_FORCE_INLINE auto operator()(std::u32string_view sv) const noexcept {
		return match(sv.begin(), sv.end());
	}
	template <typename Iterator> constexpr CTRE_FORCE_INLINE static auto match(Iterator begin, Iterator end) noexcept {
		return float_match_re(begin, end, RE());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::wstring_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::u16string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::u32string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	
};

template <typename RE> float_regular_expression(RE) -> float_regular_expression<RE>;


}

#endif