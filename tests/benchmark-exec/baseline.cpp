#include "common.hpp"

int main (int argc, char ** argv)
{
	benchmark(argc, argv, "baseline", "#000000", [] (auto) { return true; });
	
	return 0;
}
