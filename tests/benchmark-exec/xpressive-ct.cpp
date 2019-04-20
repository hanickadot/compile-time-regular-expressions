#include "common.hpp"
#include <boost/xpressive/regex_primitives.hpp>

int main (int argc, char ** argv)
{
	// [a-z0-9]+?abc[0-9]
	
	using svregex = boost::xpressive::basic_regex<std::string_view::const_iterator>;
	//svregex re = svregex::compile(PATTERN);
	
	svregex re = (-+boost::xpressive::alnum) >> 'a' >> 'b' >> 'c' >> boost::xpressive::digit;
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return bool(boost::xpressive::regex_search(line, re));
	});
}
