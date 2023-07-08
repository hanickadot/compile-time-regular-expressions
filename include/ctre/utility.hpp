#ifndef CTRE__UTILITY__HPP
#define CTRE__UTILITY__HPP

#include "../ctll/utilities.hpp"

#define CTRE_CNTTP_COMPILER_CHECK CTLL_CNTTP_COMPILER_CHECK

#if __GNUC__ > 9
#if __has_cpp_attribute(likely)
#define CTRE_LIKELY [[likely]]
#else
#define CTRE_LIKELY
#endif

#if __has_cpp_attribute(unlikely)
#define CTRE_UNLIKELY [[unlikely]]
#else
#define CTRE_UNLIKELY
#endif
#else
#define CTRE_LIKELY
#define CTRE_UNLIKELY
#endif

#ifdef _MSC_VER
#define CTRE_FORCE_INLINE __forceinline
#if _MSC_VER >= 1930
#define CTRE_FLATTEN [[msvc::flatten]]
#else
#define CTRE_FLATTEN
#endif
#else
#define CTRE_FORCE_INLINE inline __attribute__((always_inline))
#define CTRE_FLATTEN __attribute__((flatten))
#endif

#endif
