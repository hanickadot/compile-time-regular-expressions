#include <cstdio>
#include <iostream>
#include <fstream> 
#include "ctfa/match.hpp"

using namespace ctfa;

////constexpr auto re = "ABCD|DEFGH|EFGHI|A{4,}"_ctre;
using RE1 = ctfa::fa<
       ctfa::state< 0, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, sink<0>>,
       ctfa::state< 1, eq<'A',  5>, eq<'B',  8>, eq<'D', 16>, eq<'E', 17>, sink<0>>,
       ctfa::state< 2, eq<'A', 11>, eq<'D', 14>, eq<'E', 17>, sink<0>>,
       ctfa::state< 3, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'H',  6>, sink<0>>,
       ctfa::state< 4, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'I', 15>, sink<0>>,
 ctfa::final_state< 5, eq<'A',  5>, eq<'B',  8>, eq<'D', 16>, eq<'E', 17>, sink<0>>,
 ctfa::final_state< 6, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'I', 15>, sink<0>>,
       ctfa::state< 7, eq<'A',  1>, eq<'B',  8>, eq<'D', 16>, eq<'E', 17>, sink<0>>,
       ctfa::state< 8, eq<'A', 11>, eq<'C',  2>, eq<'D', 16>, eq<'E', 17>, sink<0>>,
       ctfa::state< 9, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'G',  3>, sink<0>>,
       ctfa::state<10, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'H',  4>, sink<0>>,
       ctfa::state<11, eq<'A',  7>, eq<'B',  8>, eq<'D', 16>, eq<'E', 17>, sink<0>>,
       ctfa::state<12, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'F',  9>, sink<0>>,
       ctfa::state<13, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'G', 10>, sink<0>>,
 ctfa::final_state<14, eq<'A', 11>, eq<'D', 16>, eq<'E', 12>, sink<0>>,
 ctfa::final_state<15, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, sink<0>>,
       ctfa::state<16, eq<'A', 11>, eq<'D', 16>, eq<'E', 12>, sink<0>>,
       ctfa::state<17, eq<'A', 11>, eq<'D', 16>, eq<'E', 17>, eq<'F', 13>, sink<0>>
>;

// ([aAbB]{4,}|[xXyY]{4,}|[1234]{4,})0

using RE2 = ctfa::fa<
      state< 0, range<'a','b',11>, range<'x','y',12>, range<'1','3',13>, sink<0>>,
      state< 1, range<'a','b', 5>, range<'x','y',12>, range<'1','3',13>, sink<0>>,
      state< 2, range<'a','b',11>, range<'x','y', 6>, range<'1','3',13>, sink<0>>,
      state< 3, range<'a','b',11>, range<'x','y',12>, range<'1','3', 7>, sink<0>>,
final_state< 4, range<'a','b',11>, range<'x','y',12>, range<'1','3',13>, sink<0>>,
      state< 5, range<'a','b', 5>, range<'x','y',12>, range<'1','3',13>, eq<'0', 4>, sink<0>>,
      state< 6, range<'a','b',11>, range<'x','y', 6>, range<'1','3',13>, eq<'0', 4>, sink<0>>,
      state< 7, range<'a','b',11>, range<'x','y',12>, range<'1','3', 7>, eq<'0', 4>, sink<0>>,
      state< 8, range<'a','b', 1>, range<'x','y',12>, range<'1','3',13>, sink<0>>,
      state< 9, range<'a','b',11>, range<'x','y', 2>, range<'1','3',13>, sink<0>>,
      state<10, range<'a','b',11>, range<'x','y',12>, range<'1','3', 3>, sink<0>>,
      state<11, range<'a','b', 8>, range<'x','y',12>, range<'1','3',13>, sink<0>>,
      state<12, range<'a','b',11>, range<'x','y', 9>, range<'1','3',13>, sink<0>>,
      state<13, range<'a','b',11>, range<'x','y',12>, range<'1','3',10>, sink<0>>
>;

int main (int argc, char ** argv)
{
	auto grep = [&](auto && stream) {
		std::string line;
		while (std::getline(stream, line)) {
			if (ctfa::search<RE1>(line)) {
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

