#include <ctfa/transformations/concat.hpp>
#include <ctfa/block.hpp>

template <auto v> struct identify;

namespace both_non_empty {
	static constexpr auto fa1 = ctfa::block::unit<'a'>;

	static constexpr auto fa2 = ctfa::block::unit<'b'>;

	static constexpr auto fa = ctfa::concat_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<3, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
	},{
		ctfa::state{3}
	}};

	static_assert(fa == both_non_empty::expected);
}

namespace first_empty {
	static constexpr auto fa1 = ctfa::block::empty;

	static constexpr auto fa2 = ctfa::block::unit<'a'>;

	static constexpr auto fa = ctfa::concat_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<2, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>}
	},{
		ctfa::state{2}
	}};

	static_assert(fa == first_empty::expected);
}

namespace second_empty {
	static constexpr auto fa1 = ctfa::block::unit<'a'>;

	static constexpr auto fa2 = ctfa::block::empty;

	static constexpr auto fa = ctfa::concat_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<1, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	},{
		ctfa::state{1},
		ctfa::state{2}
	}};

	static_assert(fa == second_empty::expected);
}

namespace both_empty {
	static constexpr auto fa1 = ctfa::block::empty;

	static constexpr auto fa2 = ctfa::block::empty;

	static constexpr auto fa = ctfa::concat_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<0, 2>{{},{
		ctfa::state{0}, ctfa::state{1}
	}};

	static_assert(fa == both_empty::expected);
}

namespace multi_final_one_right {
	static constexpr auto fa1 = ctfa::finite_automaton<2, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	},{ctfa::state{1}, ctfa::state{2}}};

	static constexpr auto fa2 = ctfa::block::unit<'c'>;

	static constexpr auto fa = ctfa::concat_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<5, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{4}, ctfa::matcher::unit<'c'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{4}, ctfa::matcher::unit<'c'>},
		ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'c'>}
		
	},{ctfa::state{4}}};

	static_assert(fa == multi_final_one_right::expected);
}

namespace multi_final_two_right {
	static constexpr auto fa1 = ctfa::finite_automaton<2, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	},{ctfa::state{1}, ctfa::state{2}}};

	static constexpr auto fa2 = ctfa::finite_automaton<2, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'c'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'d'>},
	},{ctfa::state{1}, ctfa::state{2}}};

	static constexpr auto fa = ctfa::concat_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<8, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{4}, ctfa::matcher::unit<'c'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{5}, ctfa::matcher::unit<'d'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{4}, ctfa::matcher::unit<'c'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{5}, ctfa::matcher::unit<'d'>},
		ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'c'>},
		ctfa::transition{ctfa::state{3}, ctfa::state{5}, ctfa::matcher::unit<'d'>}
		
	},{ctfa::state{4}, ctfa::state{5}}};

	static_assert(fa == multi_final_two_right::expected);
}

namespace zero_two_right {
	static constexpr auto fa1 = ctfa::block::empty;

	static constexpr auto fa2 = ctfa::finite_automaton<2, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	},{ctfa::state{1}, ctfa::state{2}}};

	static constexpr auto fa = ctfa::concat_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<4, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'b'>}
	},{
		ctfa::state{2}, ctfa::state{3}
	}};

	static_assert(fa == zero_two_right::expected);
}



