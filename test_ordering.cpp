#include <ctre.hpp>

#include <iostream>

std::ostream& operator<<(std::ostream& os, ctre::partial_ordering ord) {
	switch (ord) {
		case ctre::partial_ordering::less :
			return os << "less";
		case ctre::partial_ordering::greater :
			return os << "greater";
		case ctre::partial_ordering::equal :
			return os << "equal";
		case ctre::partial_ordering::unordered :
			return os << "unordered";
		default :
			return os << "BUMM!";
	}
}

int main(int argc, char ** argv) {
	using namespace ctre::literals;
	constexpr auto re = "(?<ab>[b-c])\\g{ab}"_ctre;

	std::cout << ctre::partial_ordering(re.match(argv[1])) << std::endl;
	std::cout << ctre::partial_ordering(re.ordered_match(argv[1])) << std::endl;
	
	return 0;
}
