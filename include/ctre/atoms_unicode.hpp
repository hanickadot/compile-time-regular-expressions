#ifndef CTRE__ATOMS_UNICODE__HPP
#define CTRE__ATOMS_UNICODE__HPP

#include "../ext-unicode-db/singleheader/ext/unicode.hpp"

namespace ctre {

// properties name & value

template <auto... Str> struct property_name { };
template <auto... Str> struct property_value { };

template <size_t Sz> constexpr std::string_view get_string_view(const std::array<char, Sz> & arr) noexcept {
	return std::string_view(arr.data(), arr.size());
}

enum class special_binary_property {
	unknown, any, assigned, ascii
};

constexpr special_binary_property special_binary_property_from_string(std::string_view str) noexcept {
	using namespace std::string_view_literals;
	if (uni::__pronamecomp(str, "any"sv) == 0) {
		return special_binary_property::any;
	} else if (uni::__pronamecomp(str, "assigned"sv) == 0) {
		return special_binary_property::assigned;
	} else if (uni::__pronamecomp(str, "ascii"sv) == 0) {
		return special_binary_property::ascii;
	} else {
		return special_binary_property::unknown;
	}
}

// basic support for binary and type-value properties

template <auto Name> struct binary_property;
template <auto Name, auto Value> struct property;

// unicode TS#18 level 1.2 general_category
template <uni::category Category> struct binary_property<Category> {
	template <typename CharT> inline static constexpr bool match_char(CharT c) noexcept {
		return uni::cp_is<Category>(c);
	}
};

// unicode TS#18 level 1.2 any/assigned/ascii
template <special_binary_property Prop> struct binary_property<Prop> {
	template <typename CharT> inline static constexpr bool match_char(CharT c) noexcept {
		if constexpr (Prop == special_binary_property::any) {
			return uni::cp_is_valid(c);
		} else if constexpr (Prop == special_binary_property::assigned) {
			return uni::cp_is_assigned(c);
		} else if constexpr (Prop == special_binary_property::ascii) {
			return uni::cp_is_ascii(c);
		} else {
			return false;
		}
		
	}
};

// unicode TS#18 level 1.2 script
template <uni::script Script> struct binary_property<Script> {
	template <typename CharT> inline static constexpr bool match_char(CharT c) noexcept {
		return uni::cp_script(c) == Script;
	}
};

// nonbinary properties

enum class property_type {
	script, script_extension, unknown
};

constexpr property_type property_type_from_name(std::string_view str) noexcept {
	using namespace std::string_view_literals;
	if (uni::__pronamecomp(str, "script"sv) == 0) {
		return property_type::script;
	} else if (uni::__pronamecomp(str, "script_extension"sv) == 0) {
		return property_type::script_extension;
	} else {
		return property_type::unknown;
	}
}

template <property_type Property> struct property_type_builder {
	template <auto... Value> constexpr auto get() {
		return ctll::reject{};
	}
};

template <auto... Name> struct property_builder {
	static constexpr std::array<char, sizeof...(Name)> name{static_cast<char>(Name)...};
	static constexpr property_type type = property_type_from_name(get_string_view(name));
	
	using helper = property_type_builder<type>;
	
	template <auto... Value> static constexpr auto get() {
		return helper::template get<Value...>();
	}
};

// unicode TS#18 level 1.2.2 script support

template <> struct property_type_builder<property_type::script> {
	template <auto... Value> static constexpr auto get() {
		constexpr std::array<char, sizeof...(Value)> value{Value...};
		constexpr auto sc = uni::__script_from_string(get_string_view(value));
		if constexpr (sc == uni::script::unknown) {
			return ctll::reject{};
		} else {
			return property<property_type::script, sc>();
		}
	}
};

template <uni::script Script> struct property<property_type::script, Script> {
	template <typename CharT> inline static constexpr bool match_char(CharT c) noexcept {
		return uni::cp_script(c) == Script;
	}
};


}

#endif 
