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


// UTS #18 Level 1: RL1.1: Hex Notation
static_assert(CTRE_CREATE(U"\\u{1F92A}").match(U"🤪"));
static_assert(CTRE_CREATE(U"\\u20AC").match(U"€"));
// TODO multiple character inside \u{AA BB CC}
// TODO deal with normalization 1.1.1

// UTS #18 Level 1: RL1.2: Properties
// TODO only \p and \P is not supported
// static_assert(CTRE_SYNTAX(U"\\p{L}"));
// static_assert(CTRE_SYNTAX(U"\\p{Letter}"));
// static_assert(CTRE_CREATE(U"\\p{Letter}+").match(U"abcDEF"));
// static_assert(CTRE_CREATE(U"\\p{Ll}+").match(U"abcdef"));
// static_assert(CTRE_CREATE(U"\\p{Lu}+").match(U"ABCD"));
// static_assert(!CTRE_CREATE(U"\\p{Lu}+").match(U"ABcD"));
// static_assert(CTRE_CREATE(U"\\p{Nd}+").match(U"1234567890"));
// static_assert(!CTRE_CREATE(U"\\p{Nd}+").match(U"1234567890h"));
// static_assert(CTRE_CREATE(U"\\p{script=Latin}+").match(U"abcd"));
// static_assert(CTRE_CREATE(U"\\p{script=Greek}+").match(U"βΩ"));
// static_assert(!CTRE_CREATE(U"\\p{script=Latin}+").match(U"βΩ"));
// static_assert(!CTRE_CREATE(U"\\p{script=Greek}+").match(U"abcd"));
// static_assert(CTRE_CREATE(U"\\p{emoji}+").match(U"🤪😍"));
// static_assert(CTRE_SYNTAX(U"\\p{sc=greek}+?\\p{Emoji}\\p{sc=greek}+?"));
// static_assert(CTRE_CREATE(U"\\p{sc=greek}+?\\p{Emoji}").match(U"αΩ😍"));
// static_assert(CTRE_CREATE(U"\\p{sc=greek}+?\\p{Emoji}\\p{sc=greek}+?").match(U"α😍Ω"));
// static_assert(CTRE_SYNTAX(U"\\p{age=10.0}"));
// static_assert(CTRE_CREATE(U"\\p{age=10.0}").match(U"🤩"));
// static_assert(CTRE_CREATE(U"\\p{block=misc_pictographs}").match(U"🎉"));
// static_assert(CTRE_CREATE(U"\\p{scx=Hira}+").match(U"ゖ"));

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
