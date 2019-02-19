#include "common.hpp"

int main (int argc, char ** argv)
{
	re2::RE2 re(PATTERN);
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return bool(re2::RE2::PartialMatch(re2::StringPiece(line.data(), line.length()), re));
	});
}
