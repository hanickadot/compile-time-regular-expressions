#ifndef CTRE__ATOMS__HPP
#define CTRE__ATOMS__HPP

#include <cstdint>

namespace ctre {
	
struct accept { };

 template <auto... Str> struct string { };
 template <typename... Opts> struct select { };
 template <typename... Content> struct optional { };
template <typename... Content> struct lazy_optional { };
template <typename... Content> struct possessive_optional { };
 template <typename... Content> struct sequence { };
 struct empty { };

template <typename... Content> struct plus { };
template <typename... Content> struct star { };
template <uint64_t a, uint64_t b, typename... Content> struct repeat { };

template <typename... Content> struct lazy_plus { };
template <typename... Content> struct lazy_star { };
template <uint64_t a, uint64_t b, typename... Content> struct lazy_repeat { };

template <typename... Content> struct possessive_plus { };
template <typename... Content> struct possessive_star { };
template <uint64_t a, uint64_t b, typename... Content> struct possessive_repeat { };


template <uint64_t Index, typename... Content> struct capture { };

template <uint64_t Index, typename Name, typename... Content> struct capture_with_name { };


 struct assert_begin { };
 struct assert_end { };

}

#endif
