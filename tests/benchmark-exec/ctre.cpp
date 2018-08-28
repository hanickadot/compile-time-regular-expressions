#include <ctre.hpp>
#include <cstdio>
#include <iostream>
#include <fstream> 

int main (int argc, char ** argv)
{
	using namespace ctre::literals;
	//constexpr auto re1 = "ABCD|DEFGH|EFGHI|A{4,}+"_pcre;
	constexpr auto re1 = "^[0-9]{4,16}?[aA]"_pcre;

	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (bool(re1.match(line))) {
				std::cout << line << '\n';
			}
		}
	}; 
	
	if (argc == 1) {
		grep(std::cin);
	} else for (int i = 1; i < argc; ++i) {
		std::string fname{std::string(argv[i])};
		grep(std::ifstream(fname, std::ifstream::in));
	}
	
	return 0;
}
