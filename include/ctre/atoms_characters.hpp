#ifndef CTRE__ATOMS_CHARACTERS__HPP
#define CTRE__ATOMS_CHARACTERS__HPP

#include "utility.hpp"
#include "ordering.hpp"
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

	template <typename CharT> CTRE_FORCE_INLINE static constexpr equal_less_greater compare_char(CharT value) noexcept {
		if (value == V) return {1,1,1};
		if (value < V) return {0,1,0};
		return {0,0,1};
	}
};

struct any {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT) noexcept { return true; }
	template <typename CharT> CTRE_FORCE_INLINE static constexpr equal_less_greater compare_char(CharT value) noexcept {
		return {1,0,0};
	}
};

template <typename... Content> struct negative_set {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}
	template <typename CharT> inline static constexpr equal_less_greater compare_char(CharT value) noexcept {
		return {!(Content::match_char(value) || ... || false), 0, 0};
	}
};

template <typename... Content> struct set {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return (Content::match_char(value) || ... || false);
	}

	template <typename CharT> inline static constexpr equal_less_greater compare_char(CharT value) noexcept {
		bool equal = (Content::compare_char(value).equal || ... || false);
		bool less = (Content::compare_char(value).less && ... && true);
		bool greater = (Content::compare_char(value).greater && ... && true);
		return {equal, less, greater};
	}
};

template <auto... Cs> struct enumeration : set<character<Cs>...> { };

template <typename... Content> struct negate {
	template <typename CharT> inline static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}

	template <typename CharT> inline static constexpr equal_less_greater compare_char(CharT value) noexcept {
		return {!(Content::match_char(value) || ... || false), 0, 0};
	}
};

template <auto A, auto B> struct char_range {
	template <typename CharT> CTRE_FORCE_INLINE static constexpr bool match_char(CharT value) noexcept {
		return (value >= A) && (value <= B);
	}

	template <typename CharT> CTRE_FORCE_INLINE static constexpr equal_less_greater compare_char(CharT value) noexcept {
		equal_less_greater ret;
		if (value >= A && value <= B) ret.equal = true;
		if (value <= A) ret.less = true;
		if (value >= B) ret.greater = true;
		return ret;
	}
};

struct word_chars : set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'>, character<'_'> > { };

struct space_chars : enumeration<' ', '\t', '\n', '\v', '\f', '\r'> {};

struct alphanum_chars : set<char_range<'A','Z'>, char_range<'a','z'>, char_range<'0','9'> > { };

struct alpha_chars : set<char_range<'A','Z'>, char_range<'a','z'> > { };

struct xdigit_chars : set<char_range<'A','F'>, char_range<'a','f'>, char_range<'0','9'> > { };

struct punct_chars
    : enumeration<'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', ',', '-',
		  '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']',
		  '^', '_', '`', '{', '|', '}', '~'> {};

struct digit_chars : char_range<'0','9'> { };

struct ascii_chars : char_range<'\x00','\x7F'> { };

}

#endif
