#include <ctfa/helper/info.hpp>
#include <ctfa/block.hpp>

template <auto v> struct identify;

namespace one {
	static constexpr auto fa1 = ctfa::block::unit<'a'>;

	static_assert(ctfa::info<fa1>::states == 2);
	static_assert(ctfa::info<fa1>::max_transitions == 1);
	static_assert(ctfa::info<fa1>::final_states == 1);
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

	static_assert(ctfa::info<fa1>::states == 4);
	static_assert(ctfa::info<fa1>::max_transitions == 1);
	static_assert(ctfa::info<fa1>::final_states == 1);
}

namespace test3 {
	static constexpr auto fa1 = ctfa::finite_automaton<10, 10>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	},{
		ctfa::state{0}, ctfa::state{1}
	}};

	static_assert(ctfa::info<fa1>::states == 3);
	static_assert(ctfa::info<fa1>::max_transitions == 4);
	static_assert(ctfa::info<fa1>::final_states == 2);
}
