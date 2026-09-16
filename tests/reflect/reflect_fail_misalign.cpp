// EXPECTED COMPILE FAILURE.
// GREP: named capture does not match field name at this position
// Mixed named+positional: the named capture "word" sits at position 2 but
// field "word" is at position 0, so the names disagree at that slot.

#include <ctre-reflect.hpp>

struct bad { std::string_view word, a, c; };  // 'word' at slot 0, named group at slot 2

auto x = ctre::reflect::match<bad, "(\\d+)-(?<word>\\w+)-(\\d+)">("12-abc-34");
