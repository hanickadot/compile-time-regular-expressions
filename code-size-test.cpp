#include "pregexp.hpp"

int main(int argc, char ** argv) {
	if (argc >= 2 && "^x.+$"_pre.match(argv[1])) {
		puts("match");
		return 0;
	} else {
		puts("not match");
		return 1;
	}
}