#include "common.hpp"

int main (int argc, char ** argv)
{
	auto re = std::regex( "[0-9a-fA-F]{8,16}" );
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return std::regex_search(line.begin(), line.end(), re);
	});
}
