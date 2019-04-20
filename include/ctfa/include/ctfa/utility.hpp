#ifndef CTFA__UTILITY__HPP
#define CTFA__UTILITY__HPP

#ifdef _MSC_VER
#define CTFA_FORCE_INLINE __forceinline
#define CTFA_FLATTEN
#else
#define CTFA_FORCE_INLINE inline __attribute__((always_inline))
#define CTFA_FLATTEN __attribute__((flatten))
#endif

namespace ctfa {
	struct zero_terminated_string_end_iterator {
		constexpr inline zero_terminated_string_end_iterator() = default;
		constexpr CTFA_FORCE_INLINE bool operator==(const char * ptr) const noexcept {
			return *ptr == '\0';
		} 
		constexpr CTFA_FORCE_INLINE bool operator==(const wchar_t * ptr) const noexcept {
			return *ptr == 0;
		} 
		constexpr CTFA_FORCE_INLINE bool operator!=(const char * ptr) const noexcept {
			return *ptr != '\0';
		} 
		constexpr CTFA_FORCE_INLINE bool operator!=(const wchar_t * ptr) const noexcept {
			return *ptr != 0;
		} 
	};
}


#endif