#include <cstdio>
#include <iostream>
#include <fstream> 
#include <regex>
#include "ctre.hpp"
#include <re2/re2.h>
#include <boost/regex.hpp>

extern "C" {

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>

}

int main (int argc, char ** argv)
{
	int errornumber = 0;
	size_t erroroffset = 0;
	const auto * pattern = reinterpret_cast<const unsigned char *>("[0-9a-fA-F]{8,16}");
	pcre2_code * re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
	
	if (!re) {
		std::cerr << "compilation failed\n";
		return 1;
	}
	
	int rc = pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);
	
	pcre2_match_context * mcontext = pcre2_match_context_create(NULL);
	
	pcre2_jit_stack * jit_stack = pcre2_jit_stack_create(32*1024, 512*1024, NULL);
	
	pcre2_jit_stack_assign(mcontext, NULL, jit_stack);
	
	//auto match_data = pcre2_match_data_create(re, 10);
	
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (pcre2_match(re, reinterpret_cast<const unsigned char *>(line.c_str()), line.length(), 0, 0, NULL, mcontext) >= 0) {
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
	//pcre2_match_data_free(match_data);
	pcre2_match_context_free(mcontext);
	pcre2_jit_stack_free(jit_stack);
}
