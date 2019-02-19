#ifndef CTFMT__UTILITY__HPP
#define CTFMT__UTILITY__HPP

#ifdef _MSC_VER
#define CTFMT_FORCE_INLINE __forceinline
#define CTFMT_FLATTEN
#else
#define CTFMT_FORCE_INLINE inline __attribute__((always_inline))
#define CTFMT_FLATTEN __attribute__((flatten))
#endif

#endif