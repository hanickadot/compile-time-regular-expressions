#include <ctfa/helper/translator.hpp>

constexpr bool test1() {
	constexpr auto out = []{
		ctfa::translator<5,5> t;
		t.add_basic(ctfa::state{0});
		t.add_basic(ctfa::state{1});
		return t.merge(ctfa::set<ctfa::state, 2>({ctfa::state{0}, ctfa::state{1}}));
	}();
	static_assert(out.target == ctfa::state{2});
	return true;
}