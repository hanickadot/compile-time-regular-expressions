#include <cstdio>
#include <iostream>
#include <fstream> 

extern "C" {

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>

}

int main (int argc, char ** argv)
{
	int errornumber = 0;
	size_t erroroffset = 0;
	//const unsigned char * pattern = "^[0-9]{4,16}?[aA]";
	auto * pattern = reinterpret_cast<const unsigned char *>("ABCD|DEFGH|EFGHI|A{4,}+");
	pcre2_code * re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
	if (!re) {
		std::cerr << "can't compile RE\n";
		return 1;
	}
	
	size_t subject_length;
	pcre2_match_data * match_data = pcre2_match_data_create_from_pattern(re, NULL);
	
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (pcre2_match(re, reinterpret_cast<const unsigned char *>(line.c_str()), line.length(), 0, 0, match_data, NULL) >= 0) {
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
	
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	
	return 0;
}
