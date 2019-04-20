#include <ctfa/transformations/shrink.hpp>
#include <ctfa/block.hpp>

template <auto v> struct identify;

namespace one {
	static constexpr auto fa1 = ctfa::block::unit<'a'>;
	static constexpr auto fa2 = ctfa::shrink_one<fa1>::result;

	static_assert(fa2.transitions.capacity == 1);
	static_assert(fa2.final_states.capacity == 1);
	static_assert(fa1 == fa2);
}
namespace test2 {
	static constexpr auto fa1 = ctfa::finite_automaton<10, 10>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{3}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
	},{
		ctfa::state{3}
	}};
	
	static constexpr auto fa2 = ctfa::shrink_one<fa1>::result;

	static_assert(fa2.transitions.capacity == 4);
	static_assert(fa2.final_states.capacity == 1);
	static_assert(fa1 == fa2);
}

namespace test3 {
	static constexpr auto fa1 = ctfa::finite_automaton<10, 10>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	},{
		ctfa::state{0}, ctfa::state{1}
	}};
	
	static constexpr auto fa2 = ctfa::shrink_one<fa1>::result;

	static_assert(fa2.transitions.capacity == 3);
	static_assert(fa2.final_states.capacity == 2);
	static_assert(fa1 == fa2);
}

namespace test4 {
	static constexpr auto fa1 = ctfa::finite_automaton<10, 10>{{},{
		ctfa::state{0}, ctfa::state{1}
	}};
	
	static constexpr auto fa2 = ctfa::shrink_one<fa1>::result;

	static_assert(fa2.transitions.capacity == 0);
	static_assert(fa2.final_states.capacity == 2);
	static_assert(fa1 == fa2);
}
