#include "common.hpp"

int main (int argc, char ** argv)
{
	re2::RE2 re("ABCD|DEFGH|EFGHI|A{4,}");
	
	benchmark(argc, argv, [&] (std::string_view line) { 
		return bool(re2::RE2::PartialMatch(re2::StringPiece(line.data(), line.length()), re));
	});
}
