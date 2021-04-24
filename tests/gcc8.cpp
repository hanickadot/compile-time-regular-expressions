#include <ctre.hpp>

[[maybe_unused]] static constexpr inline ctll::fixed_string pattern = "([0-9]++),([a-z]++)";

bool match(std::string_view sv) noexcept {
#if CTRE_CNTTP_COMPILER_CHECK
    return ctre::match<"([0-9]++),([a-z]++)">(sv);
#else
	return ctre::match<pattern>(sv);
#endif
}