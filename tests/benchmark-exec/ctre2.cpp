#include <ctre.hpp>
#include <cstdio>
#include <iostream>
#include <fstream>
#include "line.hpp"

int main (int argc, char ** argv)
{
	using namespace ctre::literals;
	//constexpr auto re1 = "ABCD|DEFGH|EFGHI|A{4,}+"_pcre;
	constexpr auto re1 = "^[0-9]{4,16}?[aA]"_pcre;

	auto grep = [&](file_handler stream) {
		buffer<4096> buf;
		while (true) {
			if (auto line = stream.get_line(buf); line.has_value()) {
				if (bool(re1.match(*line))) {
					fwrite(line->begin(), line->length(), 1, stdout);
					fputc('\n', stdout);
				}
			} else {
				break;
			}
		}
	}; 
	
	if (argc == 1) {
		grep(stdin);
	} else for (int i = 1; i < argc; ++i) {
		grep(argv[i]);
	}
	
	return 0;
}

