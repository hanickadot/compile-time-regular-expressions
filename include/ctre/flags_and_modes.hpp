#ifndef CTRE_V2__CTRE__FLAGS_AND_MODES__HPP
#define CTRE_V2__CTRE__FLAGS_AND_MODES__HPP

namespace ctre {

struct singleline { };
struct multiline { };

struct multiline_f { };
struct block_empty_match_f { };

template <typename Req> constexpr bool is_flag(Req) noexcept { return true; }
template <typename Req> constexpr bool is_flag(...) noexcept { return false; }

template <typename... Val> struct flags {
	static constexpr bool block_empty_match = (is_flag<block_empty_match_f>(Val{}) || ... || false);
	static constexpr bool multiline = (is_flag<multiline_f>(Val{}) || ... || false);
	
	constexpr CTRE_FORCE_INLINE flags(...) { }
	template <typename T> constexpr CTRE_FORCE_INLINE static auto add(T) noexcept -> flags<Val..., T> { return {}; }
};

template <bool Cond> struct conditional_flag_helper;

template <> struct conditional_flag_helper<true> {
	template <typename T> using type = flags<T>;
};

template <> struct conditional_flag_helper<false> {
	template <typename> using type = flags<>;
};

template <bool Cond, typename T> using conditional_flag = typename conditional_flag_helper<Cond>::template type<T>;

template <typename... Lhs, typename... Rhs> constexpr CTRE_FORCE_INLINE auto operator+(flags<Lhs...>, flags<Rhs...>) noexcept {
	return flags<Lhs..., Rhs...>{};
}

template <typename T, typename... Flags> constexpr CTRE_FORCE_INLINE auto filter_out(flags<Flags...>) noexcept {
	return (flags<>{} + ... + conditional_flag<!is_flag<T>(Flags{}), Flags>{});
}

flags(singleline) -> flags<>;
flags(multiline) -> flags<multiline_f>;

template <typename... F> constexpr CTRE_FORCE_INLINE auto block_empty_match(flags<F...> f) noexcept {
	if constexpr (f.block_empty_match) {
		return f;
	} else {
		return f.add(block_empty_match_f{});
	}
}

template <typename... F> constexpr CTRE_FORCE_INLINE auto consumed_something(flags<F...> f) noexcept {
	return filter_out<block_empty_match_f>(f);
}

template <typename... F> constexpr CTRE_FORCE_INLINE bool cannot_be_empty_match(flags<F...> f) noexcept {
	return f.block_empty_match;
}

template <typename... F> constexpr CTRE_FORCE_INLINE bool multiline_mode(flags<F...> f) noexcept {
	return f.multiline;
}

} // namespace ctre

#endif