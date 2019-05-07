#include "common.hpp"

int main (int argc, char ** argv)
{
	auto re = boost::regex(PATTERN);
	
	benchmark(argc, argv, "boost::regex", "#9e9e9e", [&] (std::string_view line) { 
		return regex_search(line.begin(), line.end(), re);
	});
}
