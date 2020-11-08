#ifndef CTRE__UTF8__HPP
#define CTRE__UTF8__HPP

#if __cpp_char8_t >= 201811

#include "utility.hpp"
#include <string_view>
#include <iterator>

namespace ctre {

struct utf8_iterator {
	using self_type = utf8_iterator;
	using value_type = char8_t;
	using reference = char8_t;
	using pointer = const char8_t *;
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = int;
	
	struct sentinel {

	};
	
	const char8_t * ptr{nullptr};
	const char8_t * end{nullptr};
	
	constexpr friend bool operator!=(const utf8_iterator & lhs, sentinel) {
		return lhs.ptr < lhs.end;
	}
	
	constexpr friend bool operator!=(sentinel, const utf8_iterator & rhs) {
		return rhs.ptr < rhs.end;
	}
	
	constexpr friend bool operator!=(const utf8_iterator & lhs, const utf8_iterator & rhs) {
		return lhs.ptr != rhs.ptr;
	}
	
	constexpr friend bool operator==(const utf8_iterator & lhs, sentinel) {
		return lhs.ptr >= lhs.end;
	}
	
	constexpr friend bool operator==(sentinel, const utf8_iterator & rhs) {
		return rhs.ptr >= rhs.end;
	}
	
	constexpr utf8_iterator & operator=(const char8_t * rhs) {
		ptr = rhs;
		return *this;
	}
	
	constexpr operator const char8_t *() const noexcept {
		return ptr;
	}
	
	constexpr utf8_iterator & operator++() noexcept {
		// the contant is mapping from first 5 bits of first code unit to length of UTF8 code point -1
		// xxxxx -> yy (5 bits to 2 bits)
		// 5 bits are 32 combination, and for each I need 2 bits, hence 64 bit constant
		// (*ptr >> 2) & 0b111110 look at the left 5 bits ignoring the least significant
		// & 0b11u  selects only needed two bits
		// +1  because each iteration is at least one code unit forward
		
		ptr += ((0x3A55000000000000ull >> ((*ptr >> 2) & 0b111110u)) & 0b11u) + 1;
		return *this;
	}
	
	constexpr utf8_iterator & operator--() noexcept {
		if (ptr > end) {
			ptr = end-1;
		} else {
			--ptr;
		}
		
		while ((*ptr & 0b11000000u) == 0b10'000000) {
			--ptr;
		}
		
		return *this;
	}
	
	constexpr utf8_iterator operator--(int) noexcept {
		auto self = *this;
		this->operator--();
		return self;
	}
	
	constexpr utf8_iterator operator++(int) noexcept {
		auto self = *this;
		this->operator++();
		return self;
	}
	
	constexpr utf8_iterator operator+(unsigned step) const noexcept {
		utf8_iterator result = *this;
		while (step > 0) {
			++result;
			step--;
		}
		return result;
	}
	
	constexpr utf8_iterator operator-(unsigned step) const noexcept {
		utf8_iterator result = *this;
		while (step > 0) {
			--result;
			step--;
		}
		return result;
	}
	
	constexpr char32_t operator*() const noexcept {
		constexpr char32_t mojibake = 0xFFFDull;
		
		// quickpath
		if (!(*ptr & 0b1000'0000u)) CTRE_LIKELY {
			return *ptr;
		}
 
		// calculate length based on first 5 bits
		const unsigned length = ((0x3A55000000000000ull >> ((*ptr >> 2) & 0b111110u)) & 0b11u);

		// actual length is number + 1 bytes
		
		// length 0 here means it's a bad front unit
		if (!length) CTRE_UNLIKELY {
			return mojibake;
		}

		// if part of the utf-8 sequence is past the end
		if (((ptr + length) >= end)) CTRE_UNLIKELY {
			return mojibake;
		}
		
		if ((ptr[1] & 0b1100'0000u) != 0b1000'0000) CTRE_UNLIKELY {
			return mojibake;
		}

		const char8_t mask = (0b0011'1111u >> length);
		
		// length = 1 (2 bytes) mask = 0b0001'1111u
		// length = 2 (3 bytes) mask = 0b0000'1111u
		// length = 3 (4 bytes) mask = 0b0000'0111u

		// remove utf8 front bits, get only significant part
		// and add first trailing unit

		char32_t result = ((ptr[0] & mask) << 6) | (ptr[1] & 0b0011'1111u);

		// add rest of trailing units
		if (length == 1) CTRE_LIKELY {
			return result;
		}

		if ((ptr[2] & 0b1100'0000u) != 0b1000'0000) CTRE_UNLIKELY {
			return mojibake;
		}

		result = (result << 6) | (ptr[2] & 0b0011'1111u);

		if (length == 2) CTRE_LIKELY {
			return result;
		}

		if ((ptr[3] & 0b1100'0000u) != 0b1000'0000) CTRE_UNLIKELY {
			return mojibake;
		}

		return (result << 6) | (ptr[3] & 0b0011'1111u);
	}
};

struct utf8_range {
	std::u8string_view range;
	constexpr utf8_range(std::u8string_view r) noexcept: range{r} { }
	
	constexpr auto begin() const noexcept {
		return utf8_iterator{range.data(), range.data() + range.size()};
	}
	constexpr auto end() const noexcept {
		return utf8_iterator::sentinel{};
	}
};

}

#endif

#endif