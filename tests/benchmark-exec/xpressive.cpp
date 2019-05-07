#include "common.hpp"

int main (int argc, char ** argv)
{
	using svregex = boost::xpressive::basic_regex<std::string_view::const_iterator>;
	svregex re = svregex::compile(PATTERN);
	
	benchmark(argc, argv, "xpressive", "#81efff", [&] (std::string_view line) { 
		return bool(boost::xpressive::regex_search(line, re));
	});
}
