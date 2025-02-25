#include <ctll/fixed_string.hpp>

constexpr auto a = ctll::fixed_string{"\0\0"};

static_assert(a.correct());
static_assert(a.size() == 2u);

constexpr auto b = ctll::fixed_string{"\0"};

static_assert(b.correct());
static_assert(b.size() == 1u);

constexpr auto c = ctll::fixed_string{""};

static_assert(c.correct());
static_assert(c.size() == 0u);
