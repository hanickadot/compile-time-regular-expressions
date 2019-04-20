#include <ctfa/fa.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

int main() {
	static constexpr auto az_09 = ctfa::concat<ctfa::block::range<'a','z'>, ctfa::block::range<'0','9'>>;
	my_debug<az_09>();
	static constexpr auto az_plus = ctfa::plus<ctfa::block::range<'a','z'>>;
	my_debug<az_plus>();
	
	static constexpr auto az_plus_09 = ctfa::concat<az_plus, ctfa::block::range<'0','9'>>;
	my_debug<az_plus_09>();
	static constexpr auto az_plus_09_plus = ctfa::concat<ctfa::plus<ctfa::block::range<'a','z'>>, ctfa::plus<ctfa::block::range<'0','9'>>>;
	my_debug<az_plus_09_plus>();
}