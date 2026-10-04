#ifndef CTLL__FIXED_STRING__GPP
#define CTLL__FIXED_STRING__GPP

#ifndef CTLL_IN_A_MODULE
#include <utility>
#include <cstddef>
#include <string_view>
#include <array>
#include <cstdint>
#endif

#include "utilities.hpp"

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

struct construct_from_pointer_t { };

constexpr auto construct_from_pointer = construct_from_pointer_t{};

CTLL_EXPORT template <size_t N> struct fixed_string {
	char32_t content[N] = {};
	size_t real_size{0};
	bool correct_flag{true};
	
	template <typename T> constexpr fixed_string(construct_from_pointer_t, const T * input) noexcept {
		if constexpr (std::is_same_v<T, char>) {
			#ifdef CTRE_STRING_IS_UTF8
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
			#else
				for (size_t i{0}; i < N; ++i) {
					content[i] = static_cast<uint8_t>(input[i]);
					real_size++;
				}
			#endif
#if defined(__cpp_char8_t)
		} else if constexpr (std::is_same_v<T, char8_t>) {
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
#endif
		} else if constexpr (std::is_same_v<T, char16_t>) {
			size_t out{0};
			for (size_t i{0}; i < N; ++i) {
				length_value_t info = length_and_value_of_utf16_code_point(input[i]);
				if (info.length == 2) {
					if (++i < N) {
						if ((input[i] & 0b1111'1100'0000'0000) == 0b1101'1100'0000'0000) {
							content[out++] = ((info.value << 10) | (input[i] & 0b0000'0011'1111'1111)) + 0x10000;
						} else {
							correct_flag = false;
							break;
						}
					}
				} else {
					content[out++] = info.value;
				}
			}
			real_size = out;
		} else if constexpr (std::is_same_v<T, wchar_t> || std::is_same_v<T, char32_t>) {
			for (size_t i{0}; i < N; ++i) {
				content[i] = static_cast<char32_t>(input[i]);
				real_size++;
			}
		}
	}
	
	template <typename T> constexpr fixed_string(const std::array<T, N> & in) noexcept: fixed_string{construct_from_pointer, in.data()} { }
	template <typename T> constexpr fixed_string(const T (&input)[N+1]) noexcept: fixed_string{construct_from_pointer, input} { }
	
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
template <typename CharT, size_t N> fixed_string(const std::array<CharT,N> &) -> fixed_string<N>;

template <size_t N> fixed_string(fixed_string<N>) -> fixed_string<N>;

template <ctll::fixed_string... Strings>
constexpr auto Concatenate() noexcept
{
    char32_t Data[(Strings.size() + ...) + 1];

    size_t Index = 0;

    ([&](auto &Item) mutable
     {
         for (size_t i = 0; i < Item.size(); i++)
         {
             Data[Index++] = Item[i];
         } }(Strings),
     ...);

    return ctll::fixed_string<(Strings.size() + ...)>(Data);
}

template <size_t Index, ctll::fixed_string String>
constexpr auto Split() noexcept
{
    char32_t First[Index + 1];
    char32_t Second[String.size() - Index + 1];

    size_t Counter = 0;

    for (size_t i = 0; i < (sizeof(First) / sizeof(char32_t)) - 1; i++)
    {
        First[i] = String[Counter++];
    }

    for (size_t i = 0; i < (sizeof(Second) / sizeof(char32_t)) - 1; i++)
    {
        Second[i] = String[Counter++];
    }

    return std::make_tuple(ctll::fixed_string(First), ctll::fixed_string(Second));
}

template <ctll::fixed_string Original, ctll::fixed_string Text>
constexpr size_t Find(size_t Index)
{
    if (!Original.size())
        return static_cast<size_t>(-1);

    for (size_t i = Index; i < Original.size() - Text.size() + 1; i++)
    {
        if (Original[i] != Text[0])
            continue;

        bool found = true;

        for (size_t j = 1; j < Text.size(); j++)
        {
            if (Original[i + j] != Text[j])
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            return i;
        }
    }

    return static_cast<size_t>(-1);
}

// Make Lenght use a struct

template <ctll::fixed_string Original, ctll::fixed_string Phrase>
constexpr size_t FindCount()
{
    if (!Original.size())
        return static_cast<size_t>(-1);

    size_t Count = 0;
    size_t Index = 0;

    while ((Index = Find<Original, Phrase>(Index)) != static_cast<size_t>(-1))
    {
        Count++;
        Index += Phrase.size();
    }

    return Count;
}

template <size_t Original, size_t Count, size_t PSize, size_t TSize>
constexpr size_t ReplacedSize() noexcept
{
    if (!Original)
        return 0;

    if (PSize == TSize)
        return Original;

    if constexpr (TSize > PSize)
    {
        return Original + Count * (TSize - PSize);
    }
    else
    {
        return Original - Count * (PSize - TSize);
    }
}

template <ctll::fixed_string Original, ctll::fixed_string Phrase, ctll::fixed_string Text>
constexpr auto Replace(size_t Index)
{
    char32_t Data[ReplacedSize<Original.size(), FindCount<Original, Phrase>(), Phrase.size(), Text.size()>() + 1];
    size_t OCounter = 0, RCounter = 0;

    while ((Index = Find<Original, Phrase>(Index + 1)) != static_cast<size_t>(-1))
    {
        for (; RCounter < Index; RCounter++)
        {
            Data[OCounter++] = Original[RCounter];
        }

        for (size_t i = 0; i < Text.size(); i++)
        {
            Data[OCounter++] = Text[i];
        }

        RCounter += Phrase.size();
    }

    for (; RCounter < Original.size(); RCounter++)
    {
        Data[OCounter++] = Original[RCounter];
    }

    return ctll::fixed_string(Data);
}

template <size_t L, size_t N, size_t M>
constexpr auto Replace(ctll::fixed_string<L> Original, ctll::fixed_string<N> Phrase, ctll::fixed_string<M> Text, size_t Index)
{
    char32_t Data[ReplacedSize<Original.size(), FindCount<Original, Phrase>(), Phrase.size(), Text.size()>() + 1];
    size_t OCounter = 0, RCounter = 0;

    while ((Index = Find(Original, Phrase, Index + 1)) != static_cast<size_t>(-1))
    {
        for (; RCounter < Index; RCounter++)
        {
            Data[OCounter++] = Original[RCounter];
        }

        for (size_t i = 0; i < M; i++)
        {
            Data[OCounter++] = Text[i];
        }

        RCounter += N;
    }

    for (; RCounter < L; RCounter++)
    {
        Data[OCounter++] = Original[RCounter];
    }

    return ctll::fixed_string(Data);
}

}

#endif
