#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

using namespace std::string_view_literals;

static inline constexpr auto pattern1 = ctll::fixed_string{"^[\\x30-\\x39]+?$"};
static inline constexpr auto pattern2 = ctll::fixed_string{""};


static_assert(ctre::re<pattern1>().match("123456789"sv));
static_assert(ctre::re<pattern2>().match(""sv));

template <auto & ptn> constexpr bool re() {
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
	constexpr auto _ptn = ptn;
#else
	constexpr auto & _ptn = ptn;
#endif
	return ctll::parser<ctre::pcre, _ptn, ctre::pcre_actions>::template correct_with<ctre::pcre_context<>>;
}

static_assert(re<pattern2>());

static inline constexpr ctll::fixed_string pat = "hello";

template <auto & ptn> constexpr bool re2() {
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
	constexpr auto _ptn = ptn;
#else
	constexpr auto & _ptn = ptn;
#endif
	return ctll::parser<ctre::pcre, _ptn, ctre::pcre_actions>::template correct_with<ctre::pcre_context<>>;
}

static_assert(re<pat>());

static_assert(ctre::re<pat>().match("hello"sv));

static_assert(ctre::match<pat>("hello"sv));
