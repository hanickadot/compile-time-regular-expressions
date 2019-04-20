#include <ctfa/fa.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

int main() {
	static constexpr auto a = ctfa::block::unit<'a'>;
	my_debug<a>();
	static constexpr auto b = ctfa::block::unit<'b'>;
	my_debug<b>();
	static constexpr auto ab = ctfa::concat<a,b>;
	my_debug<ab>();
	static constexpr auto a_or_b = ctfa::alternative<a,b>;
	my_debug<a_or_b>();
	static constexpr auto opt_a_or_b = ctfa::optional<a_or_b>;
	my_debug<opt_a_or_b>();
}