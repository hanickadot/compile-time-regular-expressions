#ifndef CTFA__CONDITION__HPP
#define CTFA__CONDITION__HPP

#include <string_view>
#include "../utility.hpp"

namespace ctfa {

namespace impl {

struct range {
	char32_t low{};
	char32_t high{};
	template <typename T> CTFA_FORCE_INLINE constexpr bool match(T v) const noexcept {
		return (low <= v) && (v <= high);
	}
	constexpr bool operator<(const range & rhs) const noexcept {
		if (low == rhs.low) {
			return high < rhs.high;
		}
		return low < rhs.low;
	}
	constexpr bool operator==(const range & rhs) const noexcept {
		return low == rhs.low && high == rhs.high;
	}
	constexpr bool operator!=(const range & rhs) const noexcept {
		return low != rhs.low || high != rhs.high;
	}
	template <typename CB> void debug(CB && cb) const {
		if (low == std::numeric_limits<char32_t>::min() && high == std::numeric_limits<char32_t>::max()) {
			cb("anything");
			return;
		}
	
		if (low == std::numeric_limits<char32_t>::min()) cb("MIN");
		else if (low >= 32 && low <= 126) cb('\'',static_cast<char>(low),'\'');
		else cb(low);
		
		if (low != high) {
			cb("..");
			if (high == std::numeric_limits<char32_t>::max()) cb("MAX");
			else if (high >= 32 && high <= 126) cb('\'',static_cast<char>(high),'\'');
			else cb(high);
		}
	}
};

}


struct condition {
	impl::range r;

	constexpr condition() noexcept { }
	
	constexpr condition(const impl::range & r) noexcept: r{r} { }
	
	template <typename T> CTFA_FORCE_INLINE constexpr bool match(T value) const noexcept {
		return r.match(value);
	}
	constexpr bool operator<(const condition & rhs) const noexcept {
		return r < rhs.r;
	}
	constexpr bool operator==(const condition & rhs) const noexcept {
		return r == rhs.r;
	}
	constexpr bool operator!=(const condition & rhs) const noexcept {
		return r != rhs.r;
	}
	template <typename CB> constexpr void debug(CB && cb) const {
		r.debug(cb);
	}
};


namespace matcher {

template <char32_t C> static constexpr auto unit = condition(impl::range{C,C});
template <char32_t A, char32_t B> static constexpr auto range = condition(impl::range{A,B});
static constexpr auto anything = condition(impl::range{std::numeric_limits<char32_t>::min(), std::numeric_limits<char32_t>::max()});

}



}

#endif
