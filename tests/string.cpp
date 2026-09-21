#include <ctll/fixed_string.hpp>

constexpr auto a = ctll::fixed_string{"\0\1"};

static_assert(a.correct());
static_assert(a.size() == 2u);

constexpr auto b = ctll::fixed_string{"\2"};

static_assert(b.correct());
static_assert(b.size() == 1u);

constexpr auto c = ctll::fixed_string{""};

static_assert(c.correct());
static_assert(c.size() == 0u);



constexpr auto a_b = a + b;
static_assert(a_b.correct());
static_assert(a_b.size() == 3u);
static_assert(a_b[0] == U'\0');
static_assert(a_b[1] == U'\1');
static_assert(a_b[2] == U'\2');

constexpr auto empty = ctll::fixed_string{""};

constexpr auto empty_empty = empty + empty;
static_assert(empty_empty.correct());
static_assert(empty_empty.size() == 0u);

constexpr auto empty_a = empty + a;
static_assert(empty_a.correct());
static_assert(empty_a.size() == 2u);
static_assert(empty_a.is_same_as(a));

constexpr auto a_empty = a + empty;
static_assert(a_empty.correct());
static_assert(a_empty.size() == 2u);
static_assert(a_empty.is_same_as(a));

constexpr auto hello = ctll::fixed_string{"hello"};
constexpr auto world = ctll::fixed_string{" world"};
constexpr auto hello_world = hello + world;
static_assert(hello_world.correct());
static_assert(hello_world.size() == 11u);
static_assert(hello_world.is_same_as(ctll::fixed_string{"hello world"}));

#if __cpp_char8_t
constexpr auto u8_left = ctll::fixed_string{u8"€"};
constexpr auto u8_right = ctll::fixed_string{u8"a"};
constexpr auto u8_concat = u8_left + u8_right;
static_assert(u8_concat.correct());
static_assert(u8_concat.size() == 2u);
static_assert(u8_concat.is_same_as(ctll::fixed_string{u8"€a"}));
#endif

constexpr auto u16_left = ctll::fixed_string{u"€"};
constexpr auto u16_right = ctll::fixed_string{u"a"};
constexpr auto u16_concat = u16_left + u16_right;
static_assert(u16_concat.correct());
static_assert(u16_concat.size() == 2u);
static_assert(u16_concat.is_same_as(ctll::fixed_string{u"€a"}));

constexpr auto u32_left = ctll::fixed_string{U"€"};
constexpr auto u32_right = ctll::fixed_string{U"a"};
constexpr auto u32_concat = u32_left + u32_right;
static_assert(u32_concat.correct());
static_assert(u32_concat.size() == 2u);
static_assert(u32_concat.is_same_as(ctll::fixed_string{U"€a"}));

#if __cpp_char8_t
constexpr auto u8_invalid = ctll::fixed_string{u8"\x80"};
static_assert(!u8_invalid.correct());
static_assert(!(u8_invalid + u8_left).correct());
static_assert(!(u8_left + u8_invalid).correct());
#endif

constexpr auto u16_invalid = ctll::fixed_string{u"\xD800 "};
static_assert(!u16_invalid.correct());
static_assert(!(u16_invalid + u16_left).correct());
static_assert(!(u16_left + u16_invalid).correct());