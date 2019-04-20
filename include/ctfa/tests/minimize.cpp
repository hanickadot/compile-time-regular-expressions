#include <ctfa/transformations/minimize.hpp>
#include <ctfa/transformations/concat.hpp>
#include <ctfa/transformations/star.hpp>
#include <ctfa/transformations/plus.hpp>
#include <ctfa/block.hpp>

template <auto v> struct identify;

namespace one {
	static constexpr auto fa1 = ctfa::finite_automaton<3, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{2}, ctfa::matcher::unit<'a'>}
	},{
		ctfa::state{2}
	}};

	static constexpr auto fa = ctfa::minimize_one<fa1>::result;

	static constexpr auto expected = ctfa::finite_automaton<2, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{1}, ctfa::matcher::unit<'a'>}
	},{
		ctfa::state{1}
	}};

	static_assert(fa == one::expected);
}

namespace empty {
	static constexpr auto fa1 = ctfa::block::empty;

	static constexpr auto fa = ctfa::minimize_one<fa1>::result;

	static constexpr auto expected = ctfa::block::empty;

	static_assert(fa == empty::expected);
}

namespace ugly {
	static constexpr auto star = ctfa::star_one<ctfa::block::unit<'a'>>::result;
	
	static constexpr auto fa1 = ctfa::minimize_one< star >::result;
	//static constexpr auto fa2 = ctfa::minimize_one< ctfa::concat_two<star, star>::result >::result;
	//static constexpr auto fa3 = ctfa::minimize_one< ctfa::concat_two<ctfa::concat_two<star, star>::result, star>::result >::result;
	
	static constexpr auto expected = ctfa::finite_automaton<2, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{0}, ctfa::matcher::unit<'a'>}
	},{
		ctfa::state{0}
	}};

	static_assert(fa1 == ugly::expected);
	// static_assert(fa2 == ugly::expected); // TODO this needs to be determinized
	// static_assert(fa3 == ugly::expected); // TODO this needs to be determinized
}

namespace two {
	static constexpr auto fa1 = ctfa::finite_automaton<2, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'b'>}
	},{
		ctfa::state{2}
	}};

	static constexpr auto fa = ctfa::plus_one<fa1>::result;
	
	static constexpr auto min = ctfa::minimize_one<fa>::result;

	static constexpr auto expected = ctfa::finite_automaton<3, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{2}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	},{
		ctfa::state{2}
	}};

	static_assert(min == two::expected);
}

#if false // need determinization
namespace both_non_empty {
	static constexpr auto fa = ctfa::finite_automaton<4, 2>({
		ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{4}, ctfa::matcher::unit<'b'>},
		ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'b'>},
	},{
		ctfa::state{2}, ctfa::state{4} 
	});
	
	static constexpr auto min = ctfa::minimize_one<fa>::result;
	
	static constexpr auto expected = ctfa::finite_automaton<2, 1>({
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
		ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'b'>}
	},{
		ctfa::state{1}
	});
	
	static_assert(min == both_non_empty::expected);
}
#endif



