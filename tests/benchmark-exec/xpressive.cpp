#include "common.hpp"

int main (int argc, char ** argv)
{
	using svregex = boost::xpressive::basic_regex<std::string_view::const_iterator>;
	svregex re = svregex::compile( "[0-9a-fA-F]{8,16}" );
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return bool(boost::xpressive::regex_search(line, re));
	});
}
