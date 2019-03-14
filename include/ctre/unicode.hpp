#ifndef CTRE__UNICODE__HPP
#define CTRE__UNICODE__HPP

#include <string_view>

namespace ctre::unicode {

enum class property {
	unknown,
	emoji,
	latin
};

constexpr property property_from_string(std::string_view name) noexcept {
	if (name == "Emoji") {
		return property::emoji;
	} else if (name == "Latin") {
		return property::latin;
	} else {
		return property::unknown;
	}
}

constexpr bool is_defined(property p) noexcept {
	return p != property::unknown;
}

}



#endif
