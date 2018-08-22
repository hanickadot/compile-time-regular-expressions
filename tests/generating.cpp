#include <ctre.hpp>

using namespace ctre::test_literals;

// basics
static_assert(ctll::size(decltype("abc"_pcre_gen)::output_type()) == 1);
static_assert(std::is_same_v<decltype(ctll::front(decltype("abc"_pcre_gen)::output_type())), ctre::string<'a','b','c'>>);