#include <ctre.hpp>
#include <string_view>

template <typename T> struct identify;

#if !__cpp_nontype_template_parameter_class
#define CTRE_CREATE(pattern) (pattern ## _ctre)
#define CTRE_SYNTAX(pattern) (pattern ## _ctre_syntax)
#else

template <ctll::fixed_string input> constexpr auto create() {
	constexpr auto _input = input;
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<ctre::pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctre::regular_expression(re());
}

template <ctll::fixed_string input> constexpr bool syntax() {
	constexpr auto _input = input;
	
	return ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template correct_with<ctre::pcre_context<>>;
}


#define CTRE_CREATE(pattern) create<pattern>()
#define CTRE_SYNTAX(pattern) syntax<pattern>()

#endif

using namespace ctre::literals;
using namespace ctre::test_literals;
using namespace std::string_view_literals;

//identify<decltype(ctll::fixed_string{u8"ěščř"})> a;
//identify<decltype(CTRE_CREATE(u8"ěščř"))> i;

static_assert(CTRE_SYNTAX(u8"a+"));
static_assert(CTRE_SYNTAX(u8"😍+"));
static_assert(CTRE_CREATE(u8"😍+").match(U"😍"));
//identify<decltype(CTRE_CREATE(u8"😍+"))> i;
//static_assert(CTRE_CREATE(u8"😍+").match(U"😍"));
//ctre::match<u8"😍">(u8"😍😍😍");

//static_assert(CTRE_CREATE(u8"😍+").match(u8"😍😍😍"));
//static_assert(CTRE_CREATE(U"ěščř").match(u8"ěščř"));
//static_assert(CTRE_CREATE(u"ěščř").match(u8"ěščř"));
//static_assert(CTRE_CREATE(L"ěščř").match(u8"ěščř"));
//static_assert(CTRE_CREATE(u8"ěščř").match(u8"ěščř"));
//
//static_assert(CTRE_SYNTAX("\\p{Latin}"));
//static_assert(!CTRE_SYNTAX("\\p{Latin42}"));
//
//static_assert(CTRE_CREATE("\\p{Latin}").match("a"sv));
//static_assert(CTRE_CREATE("\\p{Emoji}").match("a"sv));
//
