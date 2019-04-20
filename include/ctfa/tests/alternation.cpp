#include <ctfa/transformations/alternative.hpp>
#include <ctfa/block.hpp>

template <auto v> struct identify;

namespace both_non_empty {
	static constexpr auto fa1 = ctfa::block::unit<'a'>;

	static constexpr auto fa2 = ctfa::block::unit<'b'>;

	static constexpr auto fa = ctfa::alternative_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<4, 2>({
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{4}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'b'>},
	},{
		ctfa::state{2}, ctfa::state{4} 
	});
	
	static_assert(fa == both_non_empty::expected);
}

namespace first_empty {
	static constexpr auto fa1 = ctfa::block::empty;

	static constexpr auto fa2 = ctfa::block::unit<'b'>;

	static constexpr auto fa = ctfa::alternative_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<2, 3>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
	},{
		ctfa::state{0}, ctfa::state{1}, ctfa::state{3}  
	}};
	
	static_assert(fa == first_empty::expected);
}

namespace second_empty {
	static constexpr auto fa1 = ctfa::block::unit<'a'>;

	static constexpr auto fa2 = ctfa::block::empty;

	static constexpr auto fa = ctfa::alternative_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<2, 3>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
	},{
		ctfa::state{0}, ctfa::state{2}, ctfa::state{3}  
	}};
	
	static_assert(fa == second_empty::expected);
}

namespace both_empty {
	static constexpr auto fa1 = ctfa::block::empty;

	static constexpr auto fa2 = ctfa::block::empty;

	static constexpr auto fa = ctfa::alternative_two<fa1, fa2>::result;

	static constexpr auto expected = ctfa::finite_automaton<0, 3>{{},{
		ctfa::state{0}, ctfa::state{1}, ctfa::state{2}  
	}};
	
	static_assert(fa == both_empty::expected);
}




