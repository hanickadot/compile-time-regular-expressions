#include <cstdio>
#include <iostream>
#include <fstream> 
#include <regex>
#include "ctre.hpp"
#include <boost/regex.hpp>

int main (int argc, char ** argv)
{
	
	auto re = boost::regex( "([aAbB]{4,}|[xXyY]{4,}|[1234]{4,})0" );
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
