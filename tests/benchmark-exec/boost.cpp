#include <cstdio>
#include <iostream>
#include <fstream> 
#include <regex>
#include "ctre.hpp"
#include <boost/regex.hpp>

extern "C" {

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>

}

int main (int argc, char ** argv)
{
	
	auto re = boost::regex( "ABCD|DEFGH|EFGHI|A{4,}" );
	//std::regex re("([aAbB]{4,}|[xXyY]{4,}|[1234]{4,})0");
	
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (regex_search(line, re)) {
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
}
