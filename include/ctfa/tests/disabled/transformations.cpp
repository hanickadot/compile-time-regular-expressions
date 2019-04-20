#include <ctfa/fa.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/block.hpp>
#include <ctfa/minimize.hpp>

static constexpr auto a = ctfa::block::unit<'a'>;

static_assert(ctfa::block::unit<'a'> == ctfa::finite_automaton<1,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>}
},{
	ctfa::state{1}
}});

// ab
static_assert(ctfa::concat<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>> == ctfa::finite_automaton<2,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'b'>}
},{
	ctfa::state{2}
}});


// a*
static_assert(ctfa::star<ctfa::block::unit<'a'>> == ctfa::finite_automaton<2,2>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{1}, ctfa::matcher::unit<'a'>}
},{
	ctfa::state{0}, ctfa::state{1}
}});

// a+
static_assert(ctfa::plus<ctfa::block::unit<'a'>> == ctfa::finite_automaton<2,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{1}, ctfa::matcher::unit<'a'>}
},{
	ctfa::state{1}
}});

// a+|b
static_assert(ctfa::alternative<ctfa::plus<ctfa::block::unit<'a'>>, ctfa::block::unit<'b'>> == ctfa::finite_automaton<3,2>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{1}, ctfa::matcher::unit<'a'>}
},{
	ctfa::state{1},
	ctfa::state{2}
}});

// (a+|b)*
static_assert(ctfa::star<ctfa::alternative<ctfa::plus<ctfa::block::unit<'a'>>, ctfa::block::unit<'b'>>> == ctfa::finite_automaton<6,3>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{2}, ctfa::matcher::unit<'b'>}
},{
	ctfa::state{0},
	ctfa::state{1},
	ctfa::state{2}
}});

// min((a+|b)*)
static_assert(ctfa::minimize<ctfa::star<ctfa::alternative<ctfa::plus<ctfa::block::unit<'a'>>, ctfa::block::unit<'b'>>>> == ctfa::finite_automaton<2,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{0}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{0}, ctfa::matcher::unit<'b'>}
},{
	ctfa::state{0}
}});

// a?
static_assert(ctfa::optional<ctfa::block::unit<'a'>> == ctfa::finite_automaton<1,2>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>}
},{
	ctfa::state{0},
	ctfa::state{1}
}});

// a|b
static_assert(ctfa::alternative<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>> == ctfa::finite_automaton<2,2>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>}
},{
	ctfa::state{1},
	ctfa::state{2}
}});

// min(a|b)
static_assert(ctfa::minimize<ctfa::alternative<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>>> == ctfa::finite_automaton<2,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'b'>}
},{
	ctfa::state{1}
}});

static constexpr auto min_before_1 = ctfa::finite_automaton<4,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'a'>}
},{
	ctfa::state{3}
}};

static constexpr auto min_after_1 = ctfa::finite_automaton<3,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>}
},{
	ctfa::state{2}
}};

static_assert(ctfa::minimize<min_before_1> == min_after_1);


static constexpr auto min_before_2 = ctfa::finite_automaton<8,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{3}, ctfa::state{5}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{4}, ctfa::state{6}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{5}, ctfa::state{6}, ctfa::matcher::unit<'a'>},
},{
	ctfa::state{6}
}};

static constexpr auto min_after_2 = ctfa::finite_automaton<6,1>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'a'>},
},{
	ctfa::state{4}
}};

static_assert(ctfa::minimize<min_before_2> == min_after_2);

static constexpr auto min_before_3 = ctfa::finite_automaton<8,2>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{2}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{3}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{3}, ctfa::state{5}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{4}, ctfa::state{6}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{5}, ctfa::state{6}, ctfa::matcher::unit<'a'>},
},{
	ctfa::state{3}, ctfa::state{6}
}};

static constexpr auto min_after_3 = ctfa::finite_automaton<6,2>{{
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{0}, ctfa::state{1}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{1}, ctfa::state{2}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'a'>},
	ctfa::transition{ctfa::state{2}, ctfa::state{3}, ctfa::matcher::unit<'b'>},
	ctfa::transition{ctfa::state{3}, ctfa::state{4}, ctfa::matcher::unit<'a'>},
},{
	ctfa::state{2},
	ctfa::state{4}
}};

static_assert(ctfa::minimize<min_before_3> == min_after_3);

//static constexpr auto min_before_4 = ctfa::concat<ctfa::star<ctfa::block::unit<'a'>>, ctfa::star<ctfa::block::unit<'a'>>>;
//static constexpr auto min_after_4 = ctfa::star<ctfa::block::unit<'a'>>;
//
//static_assert(ctfa::minimize<min_before_4> == min_after_4);
//


