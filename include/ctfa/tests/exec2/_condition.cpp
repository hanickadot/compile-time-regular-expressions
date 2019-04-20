#include <ctfa/fa.hpp>

int main() {
	
	constexpr auto az = ctfa::matcher::range<'a','z'>;
	constexpr auto af = ctfa::matcher::range<'a','f'>;
	//static_assert(az.collides(af));
	constexpr auto _09 = ctfa::matcher::range<'0','9'>;
	//static_assert(not az.collides(_09));
	constexpr auto _3 = ctfa::matcher::unit<'3'>;
	//static_assert(_09.collides(_3));
}