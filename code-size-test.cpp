#include "pregexp.hpp"

int main(int argc, char ** argv) {
	using namespace sre;
	
	static_assert(
		std::is_same<
			RegExp<Begin,Select<Char<'a','b','c'>,String<'x','y','z'>>,Plus<Anything>,End>,
			decltype("^(?:[abc]|xyz).+$"_pre)
		>::value
	);
	
	if (argc >= 2 && "^(?:[abc]|xyz).+$"_pre.match(argv[1])) {
		puts("match");
		return 0;
	} else {
		puts("not match");
		return 1;
	}
}
