#include "common.hpp"

static constexpr inline auto pattern = ctll::fixed_string(PATTERN);

int main (int argc, char ** argv)
{
	benchmark(argc, argv, "CTRE (dfa)", "#2a76dd", [] (auto sv) { 
		return ctre::fast_search<pattern>(sv);
	});
}

