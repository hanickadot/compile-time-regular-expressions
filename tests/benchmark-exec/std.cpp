#include "common.hpp"

int main (int argc, char ** argv)
{
	auto re = std::regex(PATTERN);
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return std::regex_search(line.begin(), line.end(), re);
	});
}
