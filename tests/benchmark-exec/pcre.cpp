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

void explain_error(int err) {
	char buffer[512];
	pcre2_get_error_message(err, reinterpret_cast<unsigned char *>(&buffer), 512);
	std::cerr << "error: " << err << ": " << buffer << "\n";
}

int main (int argc, char ** argv)
{
	int errornumber = 0;
	size_t erroroffset = 0;
	auto * pattern = reinterpret_cast<const unsigned char *>(".*RZ66X.*");
	pcre2_code * re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
	
	if (!re) {
		explain_error(errornumber);
	}
	
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (auto out = pcre2_match(re, reinterpret_cast<const unsigned char *>(line.c_str()), line.length(), 0, 0, NULL, NULL); out >= 0) {
				std::cout << line << '\n';
			} else {
				explain_error(out);
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
