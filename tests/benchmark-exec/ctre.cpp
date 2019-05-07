#include "common.hpp"

static constexpr inline auto pattern = ctll::fixed_string(PATTERN);

int main (int argc, char ** argv)
{
	// #6998d8
	benchmark(argc, argv, "CTRE", "#2a76dd", [] (auto sv) { 
		return bool(ctre::search<pattern>(sv));
	});
}

