#include <ctfa/fa.hpp>
#include <ctfa/transformation.hpp>
#include <ctfa/reachability.hpp>
#include <ctfa/block.hpp>
#include <iostream>
#include "debug.hpp"

int main() {
	static constexpr auto aloha = ctfa::concat<ctfa::block::unit<'a'>, ctfa::block::unit<'l'>, ctfa::block::unit<'o'>, ctfa::block::unit<'h'>, ctfa::block::unit<'a'>>;
	my_debug<aloha>();
	
	static constexpr auto hello = ctfa::concat<ctfa::block::unit<'h'>, ctfa::block::unit<'e'>, ctfa::block::unit<'l'>, ctfa::block::unit<'l'>, ctfa::block::unit<'o'>>;
	my_debug<hello>();
	
	static constexpr auto aloha_or_hello = ctfa::alternative<aloha, hello>;
	my_debug<aloha_or_hello>();
	
	static constexpr auto aloha_or_hello_2 = ctfa::remove_unreachable<aloha_or_hello>;
	my_debug<aloha_or_hello_2>();
	
	static constexpr auto aloha2 = ctfa::concat<ctfa::block::string<'a','l','o','h','a'>, ctfa::block::unit<' '>, ctfa::block::string<'a','m','i','g','o','s'>>;
	my_debug<aloha2>();
}