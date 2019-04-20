#include <ctfa/fa.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

int main() {
	static constexpr auto az_09 = ctfa::concat<ctfa::block::range<'a','z'>, ctfa::block::range<'0','9'>>;
	my_debug<az_09>();
	static constexpr auto az_star = ctfa::star<ctfa::block::range<'a','z'>>;
	my_debug<az_star>();
	
	static constexpr auto n09_star = ctfa::star<ctfa::block::range<'0','9'>>;
	my_debug<n09_star>();
	
	static constexpr auto az_star_09_star = ctfa::concat<az_star, n09_star>;
	my_debug<az_star_09_star>();
}