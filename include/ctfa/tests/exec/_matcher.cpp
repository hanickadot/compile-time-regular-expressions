#include <cstdio>
//#include <ctfa/transformations/determinize2.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/transformations/determinize.hpp>
#include <ctfa/block.hpp>
//#include <ctfa/basic/debug.hpp>
#include <ctfa/matcher/match.hpp>


bool match(const char * subject) {
    static constexpr auto azplus = ctfa::plus<ctfa::block::range<'a','z'>>;
    static constexpr auto _09plus = ctfa::plus<ctfa::block::range<'0','9'>>;
    static constexpr auto az_plus_09_plus = ctfa::concat<azplus,_09plus>;

    if (ctfa::table_search_ptr<az_plus_09_plus>(subject)) {
		return true;
	} else {
		return false;
	}
}


int main(int argc, char ** argv) {
	if (argc < 2) return 1;
	
	if (match(argv[1])) {
		puts("match");
	} else {
		puts("not match");
	}
	return 0;
}

