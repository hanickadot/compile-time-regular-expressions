#include <iostream>
#include <ctfa/transformation.hpp>
#include <ctfa/block.hpp>
#include <ctfa/basic/debug.hpp>

#include "debug.hpp"

int main() {
	static constexpr auto star = ctfa::star_one<ctfa::block::unit<'a'>>::result;
	
	static constexpr auto fa1 = ctfa::minimize_one< star >::result;
	static constexpr auto fa2 = ctfa::minimize_one< ctfa::concat_two<star, star>::result >::result;
	static constexpr auto fa3 = ctfa::minimize_one< ctfa::concat_two<ctfa::concat_two<star, star>::result, star>::result >::result;
	
	static constexpr auto expected = ctfa::finite_automaton<2, 1>{{
		ctfa::transition{ctfa::state{0}, ctfa::state{0}, ctfa::matcher::unit<'a'>}
	},{
		ctfa::state{0}
	}};
	
	static_assert(expected == fa1);
	//static_assert(expected == fa2);
	
	my_debug<expected>();
	my_debug<fa2>();
}