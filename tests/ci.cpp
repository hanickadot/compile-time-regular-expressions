#include <ctre.hpp>

static_assert(ctre::match<"aloha">("aloha"));
static_assert(!ctre::match<"aloha">("ALOHA"));

static_assert(ctre::match<"aloha", ctre::case_insensitive>("aloha"));
static_assert(ctre::match<"aloha", ctre::case_insensitive>("ALOHA"));
