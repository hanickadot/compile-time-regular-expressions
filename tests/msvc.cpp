#include <ctre.hpp>
#include <string_view>

using namespace std::string_view_literals;

static inline constexpr auto pattern1 = ctll::basic_fixed_string{"^[\\x30-\\x39]+?$"};

static_assert(ctre::re<pattern1>().match("123456789"sv));
