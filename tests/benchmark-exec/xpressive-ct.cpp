#include "common.hpp"
#include <boost/xpressive/regex_primitives.hpp>

int main (int argc, char ** argv)
{
	// "ABCDE-[0-9]+"
	// [a-z0-9]+?abc[0-9]
	
	using svregex = boost::xpressive::basic_regex<std::string_view::const_iterator>;
	//svregex re = svregex::compile(PATTERN);
	
	//([A-Z0-9]|-)+?ABC-[0-9]
	
	svregex re = (boost::xpressive::as_xpr('A')) >> 'B' >> 'C' >> 'D' >> 'E' >> '-' >> +(boost::xpressive::alnum);
	//svregex re = (-+(boost::xpressive::alnum | '-')) >> 'A' >> 'B' >> 'C' >> '-' >> boost::xpressive::digit;
	
	benchmark(argc, argv, "xpressive (static)", "#81efff",[&] (std::string_view line) { 
		return bool(boost::xpressive::regex_search(line, re));
	});
}
