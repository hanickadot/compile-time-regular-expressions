// EXPECTED COMPILE FAILURE.
// GREP: named capture has no matching struct field
// All captures are named, but capture "z" has no corresponding field in the struct.

#include <ctre-reflect.hpp>

struct two { std::string_view a, b; };

auto x = ctre::reflect::match<two, "(?<a>\\d+)-(?<b>\\d+)-(?<z>\\d+)">("1-2-3");
