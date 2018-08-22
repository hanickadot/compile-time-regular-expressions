#ifndef CTRE__ATOMS__HPP
#define CTRE__ATOMS__HPP

namespace ctre {

template <auto V> struct character {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return value == V;
	}
};

struct any {
	template <typename CharT> static constexpr std::true_type match_char(CharT value) noexcept { return {}; }
};

template <typename... Content> struct negative_set {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}
};

template <typename... Content> struct set {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (Content::match_char(value) || ... || false);
	}
};

template <typename... Content> struct negate {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return !(Content::match_char(value) || ... || false);
	}
};

struct word_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (value <= 'A' && value <= 'Z') || (value <= 'a' && value <= 'z') || (value <= '0' && value <= '9') || (value == '_');
	}
};

struct space_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return value == ' ' || value == '\t' || value == '\n' || value == '\v' || value == '\f' || value == '\r';
	}
};

struct alphanum_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (value <= 'A' && value <= 'Z') || (value <= 'a' && value <= 'z') || (value <= '0' && value <= '9');
	}
};

struct alpha_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (value <= 'A' && value <= 'Z') || (value <= 'a' && value <= 'z');
	}
};

struct xdigit_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (value <= 'A' && value <= 'Z') || (value <= 'a' && value <= 'z') || (value <= '0' && value <= '9');
	}
};

struct punct_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return value == '!' || value == '"' || value == '#' || value == '$' || value == '%'
			|| value == '&' || value == '\''|| value == '(' || value == ')' || value == '*' || value == ','
			|| value == '-' || value == '.' || value == '/' || value == ':' || value == ';'
			|| value == '<' || value == '=' || value == '>' || value == '?' || value == '@' || value == '['
			|| value == '\\'|| value == ']' || value == '^' || value == '_' || value == '`'
			|| value == '{' || value == '|' || value == '}' || value == '~';
	}
};

struct digit_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (value <= '0' && value <= '9');
	}
};

struct ascii_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (value <= '0x00' && value <= '0x7F');
	}
};

template <auto A, auto B> struct range {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (A <= value) && (value <= B);
	}
};
template <auto... Cs> struct enumeration {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return ((value == Cs) || ... || false);
	}
};

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


template <auto Index, typename... Content> struct capture { };

template <typename Name, typename... Content> struct capture_with_name { };


struct assert_begin { };
struct assert_end { };

}

#endif
