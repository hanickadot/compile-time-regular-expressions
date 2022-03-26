#include <ctll/fixed_string.hpp>

void empty_symbol() { }

static constexpr auto Pattern = ctll::fixed_string{ LR"(^\s*(\d+)\s+:(\S):$(\S+?)$(\S+?)$(\S+))" };

static_assert(Pattern.size() == 38);

// ordinary string is taken as array of bytes
#ifdef CTRE_STRING_IS_UTF8
static_assert(ctll::fixed_string("ěšč").size() == 3);
static_assert(ctll::fixed_string("😍").size() == 1);
static_assert(ctll::fixed_string("😍")[0] == L'😍');
#else
static_assert(ctll::fixed_string("ěšč").size() == 6); // it's just a bunch of bytes
static_assert(ctll::fixed_string("😍").size() == 4); // it's just a bunch of bytes
#endif

#if __cpp_char8_t
// u8"" is utf-8 encoded
static_assert(ctll::fixed_string(u8"ěšč").size() == 3);
static_assert(ctll::fixed_string(u8"😍").size() == 1);
static_assert(ctll::fixed_string(u8"😍")[0] == U'😍');
#endif

// u"" is utf-16
static_assert(ctll::fixed_string(u"ěšč").size() == 3);
static_assert(ctll::fixed_string(u"😍").size() == 1);
static_assert(ctll::fixed_string(u"😍").is_same_as(ctll::fixed_string(U"😍")));

// U"" is utf-32
static_assert(ctll::fixed_string(U"ěšč").size() == 3);
static_assert(ctll::fixed_string(U"😍").size() == 1);

// everything is converted into utf-32

