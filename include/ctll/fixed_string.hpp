#ifndef CTLL__FIXED_STRING__GPP
#define CTLL__FIXED_STRING__GPP

#include <utility>
#include <cstddef>
#include <string_view>

namespace ctll {

struct length_value_t {
	uint32_t value;
	uint8_t length;
};

constexpr length_value_t length_and_value_of_utf8_code_point(uint8_t first_unit) noexcept {
	if ((first_unit & 0b1110'0000) == 0b1100'0000) return {static_cast<uint32_t>(first_unit & 0b0001'1111), 2};
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

template <size_t N> class fixed_string {
private:
	char32_t content[N] = {};
	size_t real_size{0};
	bool correct_flag{true};
public:
#if CTRE_STRING_IS_UTF8
	constexpr fixed_string(const char (&input)[N]) noexcept {
		size_t out{0};
		for (size_t i{0}; i < N; ++i) {
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
	}
#else
	constexpr fixed_string(const char (&input)[N]) noexcept {
		for (size_t i{0}; i < N; ++i) {
			content[i] = static_cast<uint8_t>(input[i]);
			if ((i == (N-1)) && (input[i] == 0)) break;
			real_size++;
		}
	}
#endif // CTRE_STRING_IS_UTF8
#if __cpp_char8_t
	constexpr fixed_string(const char8_t (&input)[N]) noexcept {
		size_t out{0};
		for (size_t i{0}; i < N; ++i) {
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
	}
#endif // char8_t feature test
	constexpr fixed_string(const char16_t (&input)[N]) noexcept {
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
	}
	constexpr fixed_string(const char32_t (&input)[N]) noexcept {
		for (size_t i{0}; i < N; ++i) {
			content[i] = input[i];
			if ((i == (N-1)) && (input[i] == 0)) break;
			real_size++;
		}
	}
	constexpr fixed_string(const wchar_t (&input)[N]) noexcept {
		for (size_t i{0}; i < N; ++i) {
			content[i] = input[i];
			if ((i == (N-1)) && (input[i] == 0)) break;
			real_size++;
		}
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
};

template <typename CharT, size_t N> fixed_string(const CharT (&)[N]) -> fixed_string<N>;
template <size_t N> fixed_string(fixed_string<N>) -> fixed_string<N>;

}

#endif
