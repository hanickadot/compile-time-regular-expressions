#include <ctre.hpp>

std::optional<std::string_view> match(std::string_view sv) {
	if (auto match = regex<"^(name:[a-z]+):">(sv); match) {
		return {true, match.get<"name">().first()};
	} else {
		return std::nullopt;
	}
} 