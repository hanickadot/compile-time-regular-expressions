#include <ctfa/block.hpp>
#include <ctfa/transformation.hpp>

template <auto v> struct identify;

namespace construction {
	[[maybe_unused]] static constexpr auto fa1 = ctfa::concat<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>, ctfa::block::unit<'c'>>;
	[[maybe_unused]] static constexpr auto fa2 = ctfa::alternative<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>, ctfa::block::unit<'c'>>;
	[[maybe_unused]] static constexpr auto fa3 = ctfa::star<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>, ctfa::block::unit<'c'>>;
	[[maybe_unused]] static constexpr auto fa4 = ctfa::plus<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>, ctfa::block::unit<'c'>>;
	[[maybe_unused]] static constexpr auto fa5 = ctfa::optional<ctfa::block::unit<'a'>, ctfa::block::unit<'b'>, ctfa::block::unit<'c'>>;
}
