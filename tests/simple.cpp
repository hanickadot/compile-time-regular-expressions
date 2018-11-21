#include <ctre.hpp>

using namespace ctre::test_literals;

#ifndef EXPERIMENTAL_GCC_9

// basics
static_assert(""_simple_test);
static_assert("a"_simple_test);
static_assert("ab"_simple_test);
static_assert("a|b"_simple_test);
static_assert("n"_simple_test);

#endif