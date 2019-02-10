#include "common.hpp"

int main (int argc, char ** argv)
{
	auto re = boost::regex( "ABCD|DEFGH|EFGHI|A{4,}" );
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return regex_search(line.begin(), line.end(), re);
	});
}
