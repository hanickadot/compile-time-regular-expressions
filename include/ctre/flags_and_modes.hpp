#ifndef CTRE_V2__CTRE__FLAGS_AND_MODES__HPP
#define CTRE_V2__CTRE__FLAGS_AND_MODES__HPP

namespace ctre {

struct singleline { };
struct multiline { };

struct flags {
	enum values: uint8_t {
		block_empty_match = 1u,
		multiline = 1u << 1u,
	};
	
	uint8_t value = 0;
	constexpr CTRE_FORCE_INLINE flags(ctre::singleline) { }
	constexpr CTRE_FORCE_INLINE flags(ctre::multiline): value{values::multiline} { }
};

constexpr CTRE_FORCE_INLINE auto not_empty_match(flags f) {
	f.value |= flags::block_empty_match;
	return f;
}

constexpr CTRE_FORCE_INLINE auto consumed_something(flags f, bool condition = true) {
	if (condition) f.value &= ~flags::block_empty_match;
	return f;
}

constexpr CTRE_FORCE_INLINE bool cannot_be_empty_match(flags f) {
	return f.value & flags::block_empty_match;
}

constexpr CTRE_FORCE_INLINE bool multiline_mode(flags f) {
	return f.value & flags::multiline;
}

} // namespace ctre

#endif