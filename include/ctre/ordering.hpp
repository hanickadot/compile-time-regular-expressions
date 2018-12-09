#ifndef CTRE__ORDERING__HPP
#define CTRE__ORDERING__HPP

namespace ctre {

enum class partial_ordering { less, greater, equal, unordered };

struct equal_less_greater {
	bool equal : 1;
	bool less : 1;
	bool greater : 1;

	constexpr equal_less_greater() noexcept : equal(0), less(0), greater(0) { }
	constexpr equal_less_greater(bool e, bool l, bool g) noexcept : equal(e), less(l), greater(g) { }
};

}  //  namespace ctre

#endif  // CTRE__ORDERING__HPP
