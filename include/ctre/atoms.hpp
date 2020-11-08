#ifndef CTRE__ATOMS__HPP
#define CTRE__ATOMS__HPP

#include "atoms_characters.hpp"
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

struct any { };

// actual AST of regexp
template <auto... Str> struct string { };
template <typename... Opts> struct select { };
template <typename... Content> struct sequence { };
struct empty { };

template <size_t a, size_t b, typename... Content> struct repeat { };
template <typename... Content> using plus = repeat<1,0,Content...>;
template <typename... Content> using star = repeat<0,0,Content...>;

template <size_t a, size_t b, typename... Content> struct lazy_repeat { };
template <typename... Content> using lazy_plus = lazy_repeat<1,0,Content...>;
template <typename... Content> using lazy_star = lazy_repeat<0,0,Content...>;

template <size_t a, size_t b, typename... Content> struct possessive_repeat { };
template <typename... Content> using possessive_plus = possessive_repeat<1,0,Content...>;
template <typename... Content> using possessive_star = possessive_repeat<0,0,Content...>;

template <typename... Content> using optional = repeat<0,1,Content...>;
template <typename... Content> using lazy_optional = lazy_repeat<0,1,Content...>;

template <size_t Index, typename... Content> struct capture { };

template <size_t Index, typename Name, typename... Content> struct capture_with_name { };

template <size_t Index> struct back_reference { };
template <typename Name> struct back_reference_with_name { };

template <typename Type> struct look_start { };

template <typename... Content> struct lookahead_positive { };
template <typename... Content> struct lookahead_negative { };

struct atomic_start { };

template <typename... Content> struct atomic_group { };

template <typename... Content> struct boundary { };
template <typename... Content> struct not_boundary { };

using word_boundary = boundary<word_chars>;
using not_word_boundary = not_boundary<word_chars>;

struct assert_subject_begin { };
struct assert_subject_end { };
struct assert_subject_end_line{ };
struct assert_line_begin { };
struct assert_line_end { };

}

#endif
