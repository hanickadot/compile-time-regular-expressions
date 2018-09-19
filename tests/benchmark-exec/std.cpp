#include <cstdio>
#include <iostream>
#include <fstream> 
#include <regex>

int main (int argc, char ** argv)
{
	//using namespace ctre::literals;
	//constexpr auto re = "ABCD|DEFGH|EFGHI|A{4,}"_pcre;
	std::regex re1("^[0-9]{4,16}?[aA]");
	
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (std::regex_search(line, re1)) {
				std::cout << line << '\n';
			}
			//if (bool(re.match(line))) {
			//	std::cout << line << '\n';
			//}
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
