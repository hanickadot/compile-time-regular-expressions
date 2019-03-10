#include "common.hpp"

static constexpr inline auto pattern = ctll::fixed_string(PATTERN);

int main (int argc, char ** argv)
{
	benchmark(argc, argv, [] (auto sv) { 
		return bool(ctre::search<pattern>(sv));
	});
}

