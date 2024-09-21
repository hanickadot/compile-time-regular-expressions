#include <ctll/fixed_string.hpp>

template <ctll::fixed_string Str> struct helper {
	consteval bool correct() const noexcept {
		return Str.correct();
	}
	consteval size_t size() const noexcept {
		return Str.size();
	}
};

constexpr auto a = helper<"\0\0">{};

static_assert(a.correct());
static_assert(a.size() == 2u);

constexpr auto b = helper<"\0">{};

static_assert(b.correct());
static_assert(b.size() == 1u);

constexpr auto c = helper<"">{};

static_assert(c.correct());
static_assert(c.size() == 0u);
