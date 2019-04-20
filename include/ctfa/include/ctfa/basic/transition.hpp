#ifndef CTFA__BASIC__TRANSITION__HPP
#define CTFA__BASIC__TRANSITION__HPP

#include "state.hpp"

namespace ctfa {

struct transition {
	state source;
	state target;
	condition cond;
	constexpr bool operator<(state current) const noexcept {
		return source < current;
	}
	constexpr bool operator==(state current) const noexcept {
		return source == current;
	}
	constexpr bool operator!=(state current) const noexcept {
		return !operator==(current);
	}
	constexpr bool operator<(const transition & rhs) const noexcept {
		if (source == rhs.source) {
			if (cond == rhs.cond) {
				return target < rhs.target;
			}
			return cond < rhs.cond;
		}
		return source < rhs.source;
	}
	constexpr bool operator==(const transition & rhs) const noexcept {
		return source == rhs.source && target == rhs.target && cond == rhs.cond;
	}
	constexpr bool operator!=(const transition & rhs) const noexcept {
		return !operator==(rhs);
	}
	constexpr bool match(char32_t current) const noexcept {
		return cond.match(current);
	}
	constexpr bool is_start() const noexcept {
		return source.is_start();
	}
	constexpr transition prefix(state base) const noexcept {
		return transition{
			source.prefix(base),
			target.prefix(base),
			cond
		};
	}
	constexpr transition prefix_base(state base) const noexcept {
		return transition{
			source.prefix_base(base),
			target.prefix_base(base),
			cond
		};
	}
};

}

#endif
