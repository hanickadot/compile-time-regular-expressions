#ifndef CTRE__ORDERING__HPP
#define CTRE__ORDERING__HPP

namespace ctre {

enum class partial_ordering { less, greater, equal, unordered };

struct equal_less_greater {
	bool equal : 1 = false;
	bool less : 1 = false;
	bool greater : 1 = false;
};

}  //  namespace ctre

#endif  // CTRE__ORDERING__HPP
