#ifndef CTLL__FIXED_STRING__GPP
#define CTLL__FIXED_STRING__GPP

#include <utility>
#include <cstddef>
#include <string_view>
#include <cstdint>

namespace ctll {

struct length_value_t {
	uint32_t value;
	uint8_t length;
};

constexpr length_value_t length_and_value_of_utf8_code_point(uint8_t first_unit) noexcept {
	if ((first_unit & 0b1000'0000) == 0b0000'0000) return {static_cast<uint32_t>(first_unit), 1};
	else if ((first_unit & 0b1110'0000) == 0b1100'0000) return {static_cast<uint32_t>(first_unit & 0b0001'1111), 2};
	else if ((first_unit & 0b1111'0000) == 0b1110'0000) return {static_cast<uint32_t>(first_unit & 0b0000'1111), 3};
	else if ((first_unit & 0b1111'1000) == 0b1111'0000) return {static_cast<uint32_t>(first_unit & 0b0000'0111), 4};
	else if ((first_unit & 0b1111'1100) == 0b1111'1000) return {static_cast<uint32_t>(first_unit & 0b0000'0011), 5};
	else if ((first_unit & 0b1111'1100) == 0b1111'1100) return {static_cast<uint32_t>(first_unit & 0b0000'0001), 6};
	else return {0, 0};
}

constexpr char32_t value_of_trailing_utf8_code_point(uint8_t unit, bool & correct) noexcept {
	if ((unit & 0b1100'0000) == 0b1000'0000) return unit & 0b0011'1111;
	else {
		correct = false;
		return 0;
	}
}

constexpr length_value_t length_and_value_of_utf16_code_point(uint16_t first_unit) noexcept {
	if ((first_unit & 0b1111110000000000) == 0b1101'1000'0000'0000) return {static_cast<uint32_t>(first_unit & 0b0000001111111111), 2};
	else return {first_unit, 1};
}

template <size_t N> struct fixed_string {
	char32_t content[N] = {};
	size_t real_size{0};
	bool correct_flag{true};
	template <typename T> constexpr fixed_string(const T (&input)[N+1]) noexcept {
		if constexpr (std::is_same_v<T, char>) {
			#if CTRE_STRING_IS_UTF8
				size_t out{0};
				for (size_t i{0}; i < N; ++i) {
					if ((i == (N-1)) && (input[i] == 0)) break;
					length_value_t info = length_and_value_of_utf8_code_point(input[i]);
					switch (info.length) {
						case 6:
							if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
							[[fallthrough]];
						case 5:
							if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
							[[fallthrough]];
						case 4:
							if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
							[[fallthrough]];
						case 3:
							if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
							[[fallthrough]];
						case 2:
							if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
							[[fallthrough]];
						case 1:
							content[out++] = static_cast<char32_t>(info.value);
							real_size++;
							break;
						default:
							correct_flag = false;
							return;
					}
				}
			#else
				for (size_t i{0}; i < N; ++i) {
					content[i] = static_cast<uint8_t>(input[i]);
					if ((i == (N-1)) && (input[i] == 0)) break;
					real_size++;
				}
			#endif
		#if __cpp_char8_t
		} else if constexpr (std::is_same_v<T, char8_t>) {
			size_t out{0};
			for (size_t i{0}; i < N; ++i) {
				if ((i == (N-1)) && (input[i] == 0)) break;
				length_value_t info = length_and_value_of_utf8_code_point(input[i]);
				switch (info.length) {
					case 6:
						if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
						[[fallthrough]];
					case 5:
						if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
						[[fallthrough]];
					case 4:
						if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
						[[fallthrough]];
					case 3:
						if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
						[[fallthrough]];
					case 2:
						if (++i < N) info.value = (info.value << 6) | value_of_trailing_utf8_code_point(input[i], correct_flag);
						[[fallthrough]];
					case 1:
						content[out++] = static_cast<char32_t>(info.value);
						real_size++;
						break;
					default:
						correct_flag = false;
						return;
				}
			}
		#endif
		} else if constexpr (std::is_same_v<T, char16_t>) {
			size_t out{0};
			for (size_t i{0}; i < N; ++i) {
				length_value_t info = length_and_value_of_utf16_code_point(input[i]);
				if (info.length == 2) {
					if (++i < N) {
						if ((input[i] & 0b1111'1100'0000'0000) == 0b1101'1100'0000'0000) {
							content[out++] = (info.value << 10) | (input[i] & 0b0000'0011'1111'1111);
						} else {
							correct_flag = false;
							break;
						}
					}
				} else {
					if ((i == (N-1)) && (input[i] == 0)) break;
					content[out++] = info.value;
				}
			}
			real_size = out;
		} else if constexpr (std::is_same_v<T, wchar_t> || std::is_same_v<T, char32_t>) {
			for (size_t i{0}; i < N; ++i) {
				content[i] = input[i];
				if ((i == (N-1)) && (input[i] == 0)) break;
				real_size++;
			}
		}
	}
	constexpr fixed_string(const fixed_string & other) noexcept {
		for (size_t i{0}; i < N; ++i) {
			content[i] = other.content[i];
		}
		real_size = other.real_size;
		correct_flag = other.correct_flag;
	}
	constexpr bool correct() const noexcept {
		return correct_flag;
	}
	constexpr size_t size() const noexcept {
		return real_size;
	}
	constexpr const char32_t * begin() const noexcept {
		return content;
	}
	constexpr const char32_t * end() const noexcept {
		return content + size();
	}
	constexpr char32_t operator[](size_t i) const noexcept {
		return content[i];
	}
	template <size_t M> constexpr bool is_same_as(const fixed_string<M> & rhs) const noexcept {
		if (real_size != rhs.size()) return false;
		for (size_t i{0}; i != real_size; ++i) {
			if (content[i] != rhs[i]) return false;
		}
		return true;
	}
	constexpr operator std::basic_string_view<char32_t>() const noexcept {
		return std::basic_string_view<char32_t>{content, size()};
	}
};

template <> class fixed_string<0> {
	static constexpr char32_t empty[1] = {0};
public:
	template <typename T> constexpr fixed_string(const T *) noexcept {
		
	}
	constexpr fixed_string(std::initializer_list<char32_t>) noexcept {
		
	}
	constexpr fixed_string(const fixed_string &) noexcept {
			
	}
	constexpr bool correct() const noexcept {
		return true;
	}
	constexpr size_t size() const noexcept {
		return 0;
	}
	constexpr const char32_t * begin() const noexcept {
		return empty;
	}
	constexpr const char32_t * end() const noexcept {
		return empty + size();
	}
	constexpr char32_t operator[](size_t) const noexcept {
		return 0;
	}
	constexpr operator std::basic_string_view<char32_t>() const noexcept {
		return std::basic_string_view<char32_t>{empty, 0};
	}
};

template <typename CharT, size_t N> fixed_string(const CharT (&)[N]) -> fixed_string<N-1>;
template <size_t N> fixed_string(fixed_string<N>) -> fixed_string<N>;

}

#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
	#define CTLL_FIXED_STRING ctll::fixed_string
#else
	#define CTLL_FIXED_STRING const auto &
#endif

#endif
