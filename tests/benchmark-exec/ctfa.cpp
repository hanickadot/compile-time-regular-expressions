#include "common.hpp"
#include "ctfa.hpp"

// "[a-z0-9]+?abc[0-9]"
static constexpr inline auto az = ctfa::block::range<'a','z'>;
static constexpr inline auto _09 = ctfa::block::range<'0','9'>;
static constexpr inline auto az09 = ctfa::alternative<az,_09>;


static constexpr inline auto pattern = ctfa::concat<ctfa::plus<az09>, ctfa::block::string<'a','b','c'>, _09>;
//static constexpr inline auto pattern = ctfa::concat<ctfa::plus<ctfa::block::range<'a','z'>, ctfa::plus<ctfa::block::range<'0','9'>>>>;

int main (int argc, char ** argv)
{
	benchmark(argc, argv, [] (auto sv) { 
		return bool(ctfa::search<pattern>(sv));
	});
}

