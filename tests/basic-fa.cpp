#include "ctfa/match.hpp"

using A = ctfa::state<0, ctfa::range<'a','f', 1>, ctfa::range<'0','9', 2>>;
using B = ctfa::final_state<1, ctfa::range<'a','f', 1>>;
using C = ctfa::final_state<2, ctfa::range<'0','9', 2>>;

using test = ctfa::fa<A,B,C>;

static_assert(ctfa::match<test>("abcdef"));
static_assert(ctfa::match<test>("123456"));
static_assert(!ctfa::match<test>("abc2def"));
static_assert(!ctfa::match<test>("123a456"));

