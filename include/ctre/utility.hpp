#ifndef CTRE__UTILITY__HPP
#define CTRE__UTILITY__HPP

#ifdef _MSC_VER
#define CTRE_FORCE_INLINE __forceinline
#define CTRE_FLATTEN
#else
#define CTRE_FORCE_INLINE inline __attribute__((always_inline))
#define CTRE_FLATTEN __attribute__((flatten))
#endif

#endif
