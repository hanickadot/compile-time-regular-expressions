#include <ctre.hpp>
#include <string_view>

template <typename T> struct identify;
template <size_t N> struct number_id;


#if !__cpp_nontype_template_parameter_class
#define CTRE_CREATE(pattern) (pattern ## _ctre)
#define CTRE_SYNTAX(pattern) (pattern ## _ctre_syntax)
#define CTRE_GEN(pattern) decltype(pattern ## _ctre_gen)

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

template <ctll::fixed_string input> constexpr auto gen() {
	constexpr auto _input = input;
	
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<ctre::pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	return typename tmp::output_type::stack_type();
}


#define CTRE_GEN(pattern) decltype(gen<pattern>())
#define CTRE_CREATE(pattern) create<pattern>()
#define CTRE_SYNTAX(pattern) syntax<pattern>()

#endif

using namespace ctre::literals;
using namespace ctre::test_literals;
using namespace std::string_view_literals;


//identify<decltype(ctll::fixed_string{u8"ěščř"})> a;
//identify<decltype(CTRE_CREATE(u8"ěščř"))> i;


//identify<CTRE_GEN(u8"a+")> a;
//identify<CTRE_GEN(u8"😍")> b;

#if __cpp_char8_t
static_assert(CTRE_SYNTAX(u8"a+"));
static_assert(CTRE_SYNTAX(u8"😍+"));
static_assert(CTRE_CREATE(u8"😍").match(U"😍"));
static_assert(CTRE_CREATE(u8"😍+").match(U"😍"));
static_assert(CTRE_CREATE(u8"😍+").match(U"😍😍😍😍"));
static_assert(CTRE_CREATE(u8"[😍a\\x{1F92A}]+").match(U"😍a😍aa😍😍a🤪"));
static_assert(!CTRE_CREATE(u8"[😍a\\x{1F92A}]+").match(U"😍a😍aa😍😍a🤪x"));

constexpr auto m1 = CTRE_CREATE(u8"[😍a-z\\x{1F92A}]+").match(U"abc😍😍xyz");
static_assert(m1.to_view().length() == 8);
#endif

static_assert(CTRE_SYNTAX(U"a+"));
static_assert(CTRE_SYNTAX(U"😍+"));
static_assert(CTRE_CREATE(U"😍").match(U"😍"));
static_assert(CTRE_CREATE(U"😍+").match(U"😍"));
static_assert(CTRE_CREATE(U"😍+").match(U"😍😍😍😍"));
static_assert(CTRE_CREATE(U"[😍a\\x{1F92A}]+").match(U"😍a😍aa😍😍a🤪"));
static_assert(!CTRE_CREATE(U"[😍a\\x{1F92A}]+").match(U"😍a😍aa😍😍a🤪x"));

constexpr auto m2 = CTRE_CREATE(U"[😍a-z\\x{1F92A}]+").match(U"abc😍😍xyz");
static_assert(m2.to_view().length() == 8);

//identify<decltype(CTRE_CREATE(u8"😍+"))> i;
//static_assert(CTRE_CREATE(u8"😍+").match(U"😍"));
//ctre::match<u8"😍">(u8"😍😍😍");

//static_assert(CTRE_CREATE(u8"😍+").match(u8"😍😍😍"));
static_assert(CTRE_CREATE(U"[ěščřabc]+").match(U"ěěcěěař"));
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
