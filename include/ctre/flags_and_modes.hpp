#ifndef CTRE_V2__CTRE__FLAGS_AND_MODES__HPP
#define CTRE_V2__CTRE__FLAGS_AND_MODES__HPP

namespace ctre {

struct singleline { };
struct multiline { };

struct flags {
	bool block_empty_match = false;
	bool multiline = false;
	constexpr CTRE_FORCE_INLINE flags(ctre::singleline) { }
	constexpr CTRE_FORCE_INLINE flags(ctre::multiline): multiline{true} { }
};

constexpr CTRE_FORCE_INLINE auto not_empty_match(flags f) {
	f.block_empty_match = true;
	return f;
}

constexpr CTRE_FORCE_INLINE auto consumed_something(flags f, bool condition = true) {
	if (condition) f.block_empty_match = false;
	return f;
}

constexpr CTRE_FORCE_INLINE bool cannot_be_empty_match(flags f) {
	return f.block_empty_match;
}

constexpr CTRE_FORCE_INLINE bool multiline_mode(flags f) {
	return f.multiline;
}

} // namespace ctre

#endif