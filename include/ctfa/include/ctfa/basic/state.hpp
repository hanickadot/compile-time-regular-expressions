#ifndef CTFA__BASIC__STATE__HPP
#define CTFA__BASIC__STATE__HPP

#include "condition.hpp"

namespace ctfa {

struct state {
	int id{-1};
	
	static constexpr int placeholder = -2;
	static constexpr int invalid = -1;
	static constexpr int start = 0;
	
	constexpr bool operator<(const state & rhs) const noexcept {
		return id < rhs.id;
	}
	constexpr bool operator<=(const state & rhs) const noexcept {
		return id <= rhs.id;
	}
	constexpr bool operator==(const state & rhs) const noexcept {
		return id == rhs.id;
	}
	constexpr bool operator!=(const state & rhs) const noexcept {
		return id != rhs.id;
	}
	constexpr state prefix_base(state base) const noexcept {
		return state{id+base.id};
	}
	constexpr state prefix(state base) const noexcept {
		return state{id+base.id};
	}
	constexpr state next() const noexcept {
		return state{id+1};
	}
	constexpr bool is_start() const noexcept {
		return id == start;
	}
};

static constexpr auto start_state = state{state::start};
static constexpr auto invalid_state = state{state::invalid};

}

#endif
