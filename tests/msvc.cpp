#include <ctre.hpp>
#include <string_view>

void empty_symbol() { }

using namespace std::string_view_literals;

static inline constexpr auto pattern1 = ctll::fixed_string{"^[\\x30-\\x39]+?$"};
static inline constexpr auto pattern2 = ctll::fixed_string{""};


static_assert(ctre::re<pattern1>().match("123456789"sv));
static_assert(ctre::re<pattern2>().match(""sv));

template <auto & ptn> constexpr bool re() {
#if CTRE_CNTTP_COMPILER_CHECK
	constexpr auto _ptn = ptn;
#else
	constexpr auto & _ptn = ptn;
#endif
	return ctll::parser<ctre::pcre, _ptn, ctre::pcre_actions>::template correct_with<ctre::pcre_context<>>;
}

static_assert(re<pattern2>());

static inline constexpr ctll::fixed_string pat = "hello";

template <auto & ptn> constexpr bool re2() {
#if CTRE_CNTTP_COMPILER_CHECK
	constexpr auto _ptn = ptn;
#else
	constexpr auto & _ptn = ptn;
#endif
	return ctll::parser<ctre::pcre, _ptn, ctre::pcre_actions>::template correct_with<ctre::pcre_context<>>;
}

static_assert(re<pat>());

static_assert(ctre::re<pat>().match("hello"sv));

static_assert(ctre::match<pat>("hello"sv));
