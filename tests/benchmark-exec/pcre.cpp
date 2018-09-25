#include <cstdio>
#include <iostream>
#include <fstream> 
#include <regex>
#include "ctre.hpp"

extern "C" {

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>

}

int main (int argc, char ** argv)
{
	int errornumber = 0;
	size_t erroroffset = 0;
	auto * pattern = reinterpret_cast<const unsigned char *>("([aAbB]{4,}|[xXyY]{4,}|[1234]{4,})0");
	pcre2_code * re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
	
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (pcre2_match(re, reinterpret_cast<const unsigned char *>(line.c_str()), line.length(), 0, 0, NULL, NULL) >= 0) {
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
	
	pcre2_code_free(re);
}
