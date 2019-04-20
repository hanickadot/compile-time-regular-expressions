#include <ctfa/transformations/optional.hpp>
#include <ctfa/block.hpp>

template <auto v> struct identify;

namespace one {
	static constexpr auto fa1 = ctfa::block::unit<'a'>;

	static constexpr auto fa = ctfa::optional_one<fa1>::result;

	static constexpr auto expected = ctfa::finite_automaton<2, 2>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>}
	},{
		ctfa::state{0},
		ctfa::state{2}
	}};

	static_assert(fa == one::expected);
}

namespace none {
	static constexpr auto fa1 = ctfa::block::empty;

	static constexpr auto fa = ctfa::optional_one<fa1>::result;

	static constexpr auto expected = ctfa::finite_automaton<0, 2>{{},{
		ctfa::state{0},
		ctfa::state{1}
	}};

	static_assert(fa == none::expected);
}




