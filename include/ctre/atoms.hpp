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

struct digit_chars {
	template <typename CharT> static constexpr bool match_char(CharT value) noexcept {
		return (value <= '0' && value <= '9');
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
template <typename... Content> struct sequence { };
struct empty { };

template <typename... Content> struct plus { };
template <typename... Content> struct star { };

template <typename... Content> struct capture { };

}

#endif
