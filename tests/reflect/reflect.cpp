#include <ctre-reflect.hpp>
#include <cassert>
#include <string_view>

#ifndef CTRE_SUPPORTS_CPP26_REFLECTION
#  error "ctre::reflect requires a C++26 compiler with P2996 reflection support (-freflection)"
#else

using namespace std::string_view_literals;

// named captures -> struct fields
struct date { std::string_view year, month, day; };
constexpr auto date_m = ctre::reflect::match<date, "(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})">("2026-06-22");
static_assert(date_m && date_m->year == "2026" && date_m->month == "06" && date_m->day == "22");

// no match -> nullopt
static_assert(!ctre::reflect::match<date, "(?<year>\\d{4})">("xx"));

// positional captures (all-unnamed regex)
struct pos { std::string_view a, b, c; };
constexpr auto pos_m = ctre::reflect::match<pos, "(\\d+)-(\\d+)-(\\d+)">("1-22-333");
static_assert(pos_m && pos_m->a == "1" && pos_m->b == "22" && pos_m->c == "333");

// mixed named+unnamed captures
struct rec { std::string_view a, word, c; };
constexpr auto rec_m = ctre::reflect::match<rec, "(\\d+)-(?<word>\\w+)-(\\d+)">("12-abc-34");
static_assert(rec_m && rec_m->a == "12" && rec_m->word == "abc" && rec_m->c == "34");

// out-of-order named
struct shuffled { std::string_view hour, day, year, month, epoch; };  // regex order: year, month, day
constexpr auto shuf_m = ctre::reflect::match<shuffled, "(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})">("2026-06-22");
static_assert(shuf_m && shuf_m->year == "2026" && shuf_m->month == "06" && shuf_m->day == "22");

// all-named: extra field stays default
struct extra_named { std::string_view year, weekday; };
constexpr auto en_m = ctre::reflect::match<extra_named, "(?<year>\\d{4})">("2026");
static_assert(en_m && en_m->year == "2026" && en_m->weekday.empty());

// positional: extra field stays default
struct extra_pos { std::string_view a, b, unused; };
constexpr auto ep_m = ctre::reflect::match<extra_pos, "(\\w+)-(\\w+)">("foo-bar");
static_assert(ep_m && ep_m->a == "foo" && ep_m->b == "bar" && ep_m->unused.empty());

// numeric conversion
struct nums { int year; unsigned month, day; };
constexpr auto num_m = ctre::reflect::match<nums, "(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})">("2026-06-22");
static_assert(num_m && num_m->year == 2026 && num_m->month == 6u && num_m->day == 22u);

// conversion failure fails the whole match
struct port { std::string host; int n; };
static_assert(!ctre::reflect::match<port, "(?<host>\\w+):(?<n>\\w+)">("localhost:abc")); // n="abc"
static_assert( ctre::reflect::match<port, "(?<host>\\w+):(?<n>\\w+)">("localhost:80")->n == 80);

// overflow fails the match
struct s8 { signed char b; };
static_assert(!ctre::reflect::match<s8, "(\\d+)">("99999"));

// optional groups
struct frac { std::string_view whole; std::optional<std::string_view> frac; };
constexpr auto no_frac  = ctre::reflect::match<frac, "(?<whole>\\d+)(?<frac>\\.\\d+)?">("42");
constexpr auto with_frac = ctre::reflect::match<frac, "(?<whole>\\d+)(?<frac>\\.\\d+)?">("42.5");
static_assert(no_frac  && !no_frac->frac.has_value());
static_assert(with_frac &&  with_frac->frac.has_value() && *with_frac->frac == ".5");

struct ver { int major; std::optional<int> minor; };
static_assert(ctre::reflect::match<ver, "(\\d+)(?:\\.(\\d+))?">("3")->minor == std::nullopt);
static_assert(ctre::reflect::match<ver, "(\\d+)(?:\\.(\\d+))?">("3.14")->minor == 14);
static_assert(!ctre::reflect::match<ver, "(\\d+)(?:\\.(\\w+))?">("3.xx")); // optional num garbage -> fail

// search/starts_with
struct kv { std::string_view k, v; };
static_assert(ctre::reflect::search<kv, "(?<k>\\w+)=(?<v>\\w+)">("x; a=1")->k == "a");
static_assert(ctre::reflect::starts_with<kv, "(?<k>\\w+)">("hello world")->k == "hello");

// modifiers
static_assert( ctre::reflect::match<kv, "(?<k>[a-z]+)=(?<v>[a-z]+)", ctre::case_insensitive>("FOO=BAR")->k == "FOO");
static_assert(!ctre::reflect::match<kv, "(?<k>[a-z]+)=(?<v>[a-z]+)">("FOO=BAR")); // no modifier -> no match

// search_all: skips conversion failures, sums numeric fields
constexpr int kept = [] {
    int sum = 0;
    struct one { int v; };
    for (auto m : ctre::reflect::search_all<one, "(\\w+)">("10 x 20 y 30")) sum += m.v; // x,y dropped
    return sum;
}();
static_assert(kept == 60);

constexpr int count_kv = [] {
    int c = 0;
    for (auto m : ctre::reflect::search_all<kv, "(?<k>\\w+)=(?<v>\\w+)">("a=1 b=2 c=3")) { (void)m; ++c; }
    return c;
}();
static_assert(count_kv == 3);

// tokenize
struct token { std::string_view name; int qty; std::optional<std::string_view> unit; };
constexpr auto toks = [] {
    std::array<token, 3> out{};
    int i = 0;
    for (auto m : ctre::reflect::tokenize<token, "(?<name>\\w+):(?<qty>\\d+)(?<unit>[a-z]+)?;?">("apple:3kg;pear:10;plum:7g;"))
        out[i++] = std::move(m);
    return out;
}();
static_assert(toks[0].name == "apple" && toks[0].qty == 3 && toks[0].unit.has_value() && *toks[0].unit == "kg");
static_assert(toks[1].name == "pear"  && toks[1].qty == 10 && !toks[1].unit.has_value());
static_assert(toks[2].name == "plum"  && toks[2].qty == 7  && *toks[2].unit == "g");

#endif
