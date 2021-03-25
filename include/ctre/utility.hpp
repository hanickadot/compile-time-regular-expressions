#ifndef CTRE__UTILITY__HPP
#define CTRE__UTILITY__HPP

#define CTRE_CNTTP_COMPILER_CHECK (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L) || (__cpp_nontype_template_args >= 201411L && __clang_major__ >= 12))

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
#define CTRE_FLATTEN
#else
#define CTRE_FORCE_INLINE inline __attribute__((always_inline))
#define CTRE_FLATTEN __attribute__((flatten))
#endif

#endif
