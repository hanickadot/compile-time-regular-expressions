#include <ranges>
#include "ctre.hpp"

int do_something(std::string_view) {
    return 42;
}

int test(std::string_view in) {
    ctre::range<"abc">(in) | std::views::transform(do_something);
    return 41;
}