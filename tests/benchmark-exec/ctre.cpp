#include "common.hpp"

static constexpr inline auto pattern = ctll::basic_fixed_string("([0-9]{4,16})?[aA]");

int main (int argc, char ** argv)
{
	benchmark(argc, argv, [] (auto sv) { 
		return bool(ctre::search<pattern>(sv));
	});
}

