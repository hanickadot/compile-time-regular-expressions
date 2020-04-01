#include <ctre.hpp>

static constexpr inline ctll::fixed_string pattern = "([0-9]++),([a-z]++)";

bool match(std::string_view sv) noexcept {
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
    return ctre::match<"([0-9]++),([a-z]++)">(sv);
#else
	return ctre::match<pattern>(sv);
#endif
}