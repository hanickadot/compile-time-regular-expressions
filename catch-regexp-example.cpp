#include "pregexp.hpp"

int main(int argc, char ** argv) {
	using namespace sre;
	
	static_assert(
		std::is_same<
			RegExp<Begin,StaticCatch<0,1,Select<Plus<Char<'a','b','c'>>,String<'x','y','z'>>>,Plus<Anything>,End>,
			decltype("^([abc]+|xyz).+$"_pre)
		>::value
	);
	
	auto regexp = "^([abc]+|xyz).+$"_pre;
	if (argc >= 2 && regexp.match(argv[1])) {
		auto c = regexp.getCatch<0>()[0];
		printf("match: '%.*s'\n",c.len(),argv[1]+c.begin);
		return 0;
	} else {
		printf("not match\n");
		return 1;
	}
}
