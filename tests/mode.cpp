#include <ctre.hpp>

void empty_symbol() { }

using namespace ctre::test_literals;

#if !CTRE_CNTTP_COMPILER_CHECK
#define CTRE_TEST(pattern) (pattern ## _ctre_test)
#else

template <typename...> struct identify_me;

template <ctll::fixed_string input> constexpr bool test() {
	constexpr auto _input = input;

	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<ctre::pcre_context<>>;
	//static_assert(tmp(), "Regular Expression contains syntax error.");
	//using re = decltype(front(typename tmp::output_type::stack_type()));
	//identify_me<typename tmp::output_type::stack_type> i;
	////return ctre::regular_expression(re());
	return tmp();
}

#define CTRE_TEST(pattern) test<pattern>()
#endif


// basics
static_assert(!CTRE_TEST("(?)"));
static_assert(CTRE_TEST("(?i)"));
static_assert(CTRE_TEST("(?ic)"));
static_assert(CTRE_TEST("(?icsm)"));

// after
static_assert(CTRE_TEST("(?i)x"));
static_assert(CTRE_TEST("(?ic)x"));
static_assert(CTRE_TEST("(?icsm)x"));

// before
static_assert(CTRE_TEST("y(?i)"));
static_assert(CTRE_TEST("y(?ic)"));
static_assert(CTRE_TEST("y(?icsm)"));

static_assert(CTRE_TEST("(?i)Tom|Sawyer|Huckleberry|Finn"));


