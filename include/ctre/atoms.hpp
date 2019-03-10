#ifndef CTRE__ATOMS__HPP
#define CTRE__ATOMS__HPP

#include <cstdint>

namespace ctre {
	
// special helpers for matching
struct accept { };
struct reject { };
struct start_mark { };
struct end_mark { };
struct end_cycle_mark { };
struct end_lookahead_mark { };
template <size_t Id> struct numeric_mark { };

// actual AST of regexp
template <auto... Str> struct string { };
template <typename... Opts> struct select { };
template <typename... Content> struct optional { };
template <typename... Content> struct lazy_optional { };
template <typename... Content> struct sequence { };
struct empty { };

template <typename... Content> struct plus { };
template <typename... Content> struct star { };
template <size_t a, size_t b, typename... Content> struct repeat { };

template <typename... Content> struct lazy_plus { };
template <typename... Content> struct lazy_star { };
template <size_t a, size_t b, typename... Content> struct lazy_repeat { };

template <typename... Content> struct possessive_plus { };
template <typename... Content> struct possessive_star { };
template <size_t a, size_t b, typename... Content> struct possessive_repeat { };


template <size_t Index, typename... Content> struct capture { };

template <size_t Index, typename Name, typename... Content> struct capture_with_name { };

template <size_t Index> struct back_reference { };
template <typename Name> struct back_reference_with_name { };

template <typename Type> struct look_start { };

template <typename... Content> struct lookahead_positive { };
template <typename... Content> struct lookahead_negative { };

struct assert_begin { };
struct assert_end { };


}

#endif
