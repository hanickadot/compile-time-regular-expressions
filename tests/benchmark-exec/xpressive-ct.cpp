#include "common.hpp"
#include <boost/xpressive/regex_primitives.hpp>
#include <ctre/xpressive.hpp>

using namespace ctre::test_literals;

static constexpr inline auto input = ctll::fixed_string(PATTERN);
using RE = ctre::regex_builder<input>::type;

int main (int argc, char ** argv)
{
	
	//// [a-z0-9]+?abc[0-9]
	//
	//using svregex = boost::xpressive::basic_regex<std::string_view::const_iterator>;
	////svregex re = svregex::compile(PATTERN);
	//
	////svregex re = (-+boost::xpressive::alnum) >> 'a' >> 'b' >> 'c' >> boost::xpressive::digit;
	//
	//static constexpr auto pattern = CTRE_GEN(PATTERN)
	//// PATTERN="1234.+(....A-|.GG.G.-)"
	//
	////svregex re = '1' >> '2' >> '3' >> '4' >> (+boost::xpressive::any);
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return ctre::xpressive_search_re(line.begin(), line.end(), RE());
	});
}
