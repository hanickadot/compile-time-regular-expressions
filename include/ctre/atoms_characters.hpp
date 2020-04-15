#ifndef CTRE__ATOMS_CHARACTERS__HPP
#define CTRE__ATOMS_CHARACTERS__HPP

#include "utility.hpp"
#include <cstdint>

namespace ctre {
	
// sfinae check for types here

template <typename T> class MatchesCharacter {
	template <typename Y, typename CharT> static auto test(CharT c) -> decltype(Y::match_char(c), std::true_type());
	template <typename> static auto test(...) -> std::false_type;
public:
	template <typename CharT> static inline constexpr bool value = decltype(test<T>(std::declval<CharT>()))();
};



template <auto V> struct character {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT value) noexcept {
		return value == V;
	}
};

struct any {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT) noexcept { return true; }
};

template <typename... Content> struct negative_set {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}
};

template <typename... Content> struct set {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return (Content::match_char(value) || ... || false);
	}
};

template <auto... Cs> struct enumeration : set<character<Cs>...> { };

template <typename... Content> struct negate {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}
};

template <auto A, auto B> struct char_range {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT value) noexcept {
		return (value >= A) && (value <= B);
	}
};

using word_chars = set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'>, character<'_'> >;

using space_chars = enumeration<' ', '\t', '\n', '\v', '\f', '\r'>;

using alphanum_chars = set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'> >;

using alpha_chars = set<char_range<'A','Z'>, char_range<'a','z'> >;

using xdigit_chars = set<char_range<'A','F'>, char_range<'a','f'>, char_range<'0','9'> >;

using punct_chars
    = enumeration<'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', ',', '-',
		  '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']',
		  '^', '_', '`', '{', '|', '}', '~'>;

using digit_chars = char_range<'0','9'>;

using ascii_chars = char_range<'\x00','\x7F'>;


}

#endif
