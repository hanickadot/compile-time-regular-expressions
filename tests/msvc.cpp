#include <ctre.hpp>
#include <string_view>

using namespace std::string_view_literals;

static inline constexpr auto pattern1 = ctll::basic_fixed_string{"^[\\x30-\\x39]+?$"};
static inline constexpr auto pattern2 = ctll::basic_fixed_string{""};


static_assert(ctre::re<pattern1>().match("123456789"sv));
static_assert(ctre::re<pattern2>().match(""sv));

template <auto & ptn> constexpr bool re() {
	return ctll::parser<ctre::pcre, ptn, ctre::pcre_actions>::template correct_with<ctre::pcre_context<>>;
}

static_assert(re<pattern2>());