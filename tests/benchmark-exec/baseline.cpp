#include "common.hpp"

int main (int argc, char ** argv)
{
	benchmark(argc, argv, [] (auto) { return true; });
	
	return 0;
}
