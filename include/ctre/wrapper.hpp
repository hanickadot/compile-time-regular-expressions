#ifndef CTRE__WRAPPER__HPP
#define CTRE__WRAPPER__HPP

#include "evaluation.hpp"
#include "utility.hpp"
#include "utf8.hpp"
#include <string_view>
#include <string>

namespace ctre {
	
struct zero_terminated_string_end_iterator {
	constexpr inline zero_terminated_string_end_iterator() = default;
	constexpr CTRE_FORCE_INLINE bool operator==(const char * ptr) const noexcept {
		return *ptr == '\0';
	} 
	constexpr CTRE_FORCE_INLINE bool operator==(const wchar_t * ptr) const noexcept {
		return *ptr == 0;
	} 
	constexpr CTRE_FORCE_INLINE bool operator!=(const char * ptr) const noexcept {
		return *ptr != '\0';
	} 
	constexpr CTRE_FORCE_INLINE bool operator!=(const wchar_t * ptr) const noexcept {
		return *ptr != 0;
	} 
};

template <typename T> class RangeLikeType {
	template <typename Y> static auto test(Y *) -> decltype(std::declval<const Y &>().begin(), std::declval<const Y &>().end(), std::true_type());
	template <typename> static auto test(...) -> std::false_type;
public:
	static inline constexpr bool value = decltype(test<std::remove_reference_t<std::remove_const_t<T>>>( nullptr ))::value;
};

template <typename RE> struct regular_expression {
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto match_2(IteratorBegin begin, IteratorEnd end) noexcept {
		return match_re(begin, end, RE());
	}
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto search_2(IteratorBegin begin, IteratorEnd end) noexcept {
		return search_re(begin, end, RE());
	}
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto starts_with_2(IteratorBegin begin, IteratorEnd end) noexcept {
		return starts_with(begin, end, RE());
	}
	constexpr CTRE_FORCE_INLINE regular_expression() noexcept { }
	constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept { }
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto match(IteratorBegin begin, IteratorEnd end) noexcept {
		return match_re(begin, end, RE());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const char * s) noexcept {
		return match_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const wchar_t * s) noexcept {
		return match_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const std::string & s) noexcept {
		return match_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(const std::wstring & s) noexcept {
		return match_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::wstring_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
#if __cpp_char8_t >= 201811
	static constexpr CTRE_FORCE_INLINE auto match(std::u8string_view sv) noexcept {
		return match_re<utf8_iterator, utf8_iterator::sentinel, RE, const char8_t *>(utf8_range(sv).begin(), utf8_range(sv).end(), {});
	}
#endif
	static constexpr CTRE_FORCE_INLINE auto match(std::u16string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto match(std::u32string_view sv) noexcept {
		return match(sv.begin(), sv.end());
	}
	template <typename Range, typename = typename std::enable_if<RangeLikeType<Range>::value>::type> static constexpr CTRE_FORCE_INLINE auto match(Range && range) noexcept {
		return match(std::begin(range), std::end(range));
	}
	
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto search(IteratorBegin begin, IteratorEnd end) noexcept {
		return search_re(begin, end, RE());
	}
	constexpr CTRE_FORCE_INLINE static auto search(const char * s) noexcept {
		return search_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(const wchar_t * s) noexcept {
		return search_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(const std::string & s) noexcept {
		return search_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(const std::wstring & s) noexcept {
		return search_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto search(std::string_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto search(std::wstring_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
#if __cpp_char8_t >= 201811
	static constexpr CTRE_FORCE_INLINE auto search(std::u8string_view sv) noexcept {
		return search_re<utf8_iterator, utf8_iterator::sentinel, RE, const char8_t *>(utf8_range(sv).begin(), utf8_range(sv).end(), {});
	}
#endif
	static constexpr CTRE_FORCE_INLINE auto search(std::u16string_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto search(std::u32string_view sv) noexcept {
		return search(sv.begin(), sv.end());
	}
	template <typename Range> static constexpr CTRE_FORCE_INLINE auto search(Range && range) noexcept {
		return search(std::begin(range), std::end(range));
	}
	
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto starts_with(IteratorBegin begin, IteratorEnd end) noexcept {
		return starts_with_re(begin, end, RE());
	}
	constexpr CTRE_FORCE_INLINE static auto starts_with(const char * s) noexcept {
		return starts_with_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto starts_with(const wchar_t * s) noexcept {
		return starts_with_2(s, zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto starts_with(const std::string & s) noexcept {
		return starts_with_2(s.c_str(), zero_terminated_string_end_iterator());
	}
	static constexpr CTRE_FORCE_INLINE auto starts_with(const std::wstring & s) noexcept {
		return starts_with_2(s.c_str(), zero_terminated_string_end_iterator());
	}
#if __cpp_char8_t >= 201811
	static constexpr CTRE_FORCE_INLINE auto starts_with(std::u8string_view sv) noexcept {
		return starts_with<utf8_iterator, utf8_iterator::sentinel, RE, const char8_t *>(utf8_range(sv).begin(), utf8_range(sv).end(), {});
	}
#endif
	static constexpr CTRE_FORCE_INLINE auto starts_with(std::string_view sv) noexcept {
		return starts_with(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto starts_with(std::wstring_view sv) noexcept {
		return starts_with(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto starts_with(std::u16string_view sv) noexcept {
		return starts_with(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto starts_with(std::u32string_view sv) noexcept {
		return starts_with(sv.begin(), sv.end());
	}
	template <typename Range> static constexpr CTRE_FORCE_INLINE auto starts_with(Range && range) noexcept {
		return starts_with(std::begin(range), std::end(range));
	}
};

template <typename RE> regular_expression(RE) -> regular_expression<RE>;


}

#endif
