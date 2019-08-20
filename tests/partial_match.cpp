#include <ctre.hpp>
#include <string_view>

static constexpr inline auto pattern =
ctll::basic_fixed_string{ "[-+]?([0-9]{1,19}([.][0-9]{0,2})?|[.][0-9]{1,2})" };

static_assert(ctre::re<pattern>().match("-", ctre::partial_match));
static_assert(ctre::re<pattern>().match(".", ctre::partial_match));
static_assert(!ctre::re<pattern>().match("a", ctre::partial_match));
static_assert(ctre::re<pattern>().match("-1", ctre::partial_match));
static_assert(ctre::re<pattern>().match(".12", ctre::partial_match));
static_assert(ctre::re<pattern>().match("+12.34", ctre::partial_match));
static_assert(!ctre::re<pattern>().match("12.34duh", ctre::partial_match));
