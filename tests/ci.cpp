#include <ctre.hpp>

#if CTRE_CNTTP_COMPILER_CHECK

#define TEST_MATCH(pattern, subject) ctre::match<pattern>(subject)

#define TEST_CI_MATCH(pattern, subject) ctre::match<pattern, ctre::case_insensitive>(subject)

#else

#define TEST_MATCH(pattern, subject) []{ static constexpr inline auto _ptn ## __LINE__ = ctll::fixed_string(pattern); return ctre::re<_ptn ## i__LINE__d, ctre::match_method, ctre::case_insensitive>(subject); }()

#define TEST_CI_MATCH(pattern, subject) []{ static constexpr inline auto _ptn ## __LINE__ = ctll::fixed_string(pattern); return ctre::re<_ptn ## i__LINE__d, ctre::match_method, ctre::case_insensitive>(subject); }()

#endif

static_assert(TEST_MATCH("aloha","aloha"));
static_assert(!TEST_MATCH("aloha","ALOHA"));

static_assert(TEST_CI_MATCH("aloha", "aloha"));
static_assert(TEST_CI_MATCH("aloha", "ALOHA"));
