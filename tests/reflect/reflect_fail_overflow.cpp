// EXPECTED COMPILE FAILURE.
// GREP: more captures than struct fields
// Positional mode: 3 capture groups but struct only has 2 fields.

#include <ctre-reflect.hpp>

struct two { std::string_view a, b; };

auto x = ctre::reflect::match<two, "(\\d+)-(\\d+)-(\\d+)">("1-2-3");
