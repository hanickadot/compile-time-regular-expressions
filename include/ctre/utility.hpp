#ifndef CTRE__UTILITY__HPP
#define CTRE__UTILITY__HPP

#include "../ctll/utilities.hpp"

#define CTRE_CNTTP_COMPILER_CHECK CTLL_CNTTP_COMPILER_CHECK

#ifdef CTRE_IN_A_MODULE
#define CTRE_EXPORT export
#else
#define CTRE_EXPORT 
#endif

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
#if __has_cpp_attribute(msvc::flatten)
#define CTRE_FLATTEN [[msvc::flatten]]
#elif _MSC_VER >= 1930 && !defined(__clang__)
#define CTRE_FLATTEN [[msvc::flatten]]
#else
#define CTRE_FLATTEN
#endif
#else
#define CTRE_FORCE_INLINE inline __attribute__((always_inline))
#define CTRE_FLATTEN __attribute__((flatten))
#endif

#endif
