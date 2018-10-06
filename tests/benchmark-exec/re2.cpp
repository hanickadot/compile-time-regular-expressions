#include <cstdio>
#include <iostream>
#include <fstream> 
#include <regex>
#include "ctre.hpp"
#include <re2/re2.h>

extern "C" {

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>

}

int main (int argc, char ** argv)
{
	re2::RE2 re("ABCD|DEFGH|EFGHI|A{4,}");
	
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (re2::RE2::PartialMatch(line, re)) {
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
