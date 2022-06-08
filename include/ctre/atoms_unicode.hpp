#ifndef CTRE__ATOMS_UNICODE__HPP
#define CTRE__ATOMS_UNICODE__HPP

// master branch is not including unicode db (for now)
#include "../unicode-db/unicode_interface.hpp"
#include "flags_and_modes.hpp"

namespace ctre {

// properties name & value

template <auto... Str> struct property_name { };
template <auto... Str> struct property_value { };

template <size_t Sz> constexpr std::string_view get_string_view(const char (& arr)[Sz]) noexcept {
	return std::string_view(arr, Sz);
}


// basic support for binary and type-value properties

template <typename T, T Type> struct binary_property;
template <typename T, T Type, auto Value> struct property;

template <auto Type> using make_binary_property = binary_property<std::remove_const_t<decltype(Type)>, Type>;
template <auto Type, auto Value> using make_property = property<std::remove_const_t<decltype(Type)>, Type, Value>;

// unicode TS#18 level 1.2 general_category
template <uni::detail::binary_prop Property> struct binary_property<uni::detail::binary_prop, Property> {
	template <typename CharT> inline static constexpr bool match_char(CharT c, const flags &) noexcept {
		return uni::detail::get_binary_prop<Property>(static_cast<char32_t>(c));
	}
};

// unicode TS#18 level 1.2.2

enum class property_type {
	script, script_extension, age, block, unknown
};

// unicode TS#18 level 1.2.2

template <uni::script Script> struct binary_property<uni::script, Script> {
	template <typename CharT> inline static constexpr bool match_char(CharT c, const flags &) noexcept {
		return uni::cp_script(c) == Script;
	}
};

template <uni::script Script> struct property<property_type, property_type::script_extension, Script> {
	template <typename CharT> inline static constexpr bool match_char(CharT c, const flags &) noexcept {
		for (uni::script sc: uni::cp_script_extensions(c)) {
			if (sc == Script) return true;
		}
		return false;
	}
};

template <uni::version Age> struct binary_property<uni::version, Age> {
	template <typename CharT> inline static constexpr bool match_char(CharT c, const flags &) noexcept {
		return uni::cp_age(c) <= Age;
	}
};

template <uni::block Block> struct binary_property<uni::block, Block> {
	template <typename CharT> inline static constexpr bool match_char(CharT c, const flags &) noexcept {
		return uni::cp_block(c) == Block;
	}
};

// nonbinary properties

template <typename = void> // Make it always a template as propnamecomp isn't defined yet
constexpr property_type property_type_from_name(std::string_view str) noexcept {
	using namespace std::string_view_literals;
	if (uni::detail::propnamecomp(str, "script"sv) == 0 || uni::detail::propnamecomp(str, "sc"sv) == 0) {
		return property_type::script;
	} else if (uni::detail::propnamecomp(str, "script_extension"sv) == 0 || uni::detail::propnamecomp(str, "scx"sv) == 0) {
		return property_type::script_extension;
	} else if (uni::detail::propnamecomp(str, "age"sv) == 0) {
		return property_type::age;
	} else if (uni::detail::propnamecomp(str, "block"sv) == 0) {
		return property_type::block;
	} else {
		return property_type::unknown;
	}
}

template <property_type Property> struct property_type_builder {
	template <auto... Value> static constexpr auto get() {
		return ctll::reject{};
	}
};

template <auto... Name> struct property_builder {
	static constexpr char name[sizeof...(Name)]{static_cast<char>(Name)...};
	static constexpr property_type type = property_type_from_name(get_string_view(name));

	using helper = property_type_builder<type>;

	template <auto... Value> static constexpr auto get() {
		return helper::template get<Value...>();
	}
};

// unicode TS#18 level 1.2.2 script support

template <> struct property_type_builder<property_type::script> {
	template <auto... Value> static constexpr auto get() {
		constexpr char value[sizeof...(Value)]{static_cast<char>(Value)...};
		constexpr auto sc = uni::detail::script_from_string(get_string_view(value));
		if constexpr (uni::detail::is_unknown(sc)) {
			return ctll::reject{};
		} else {
			return make_binary_property<sc>();
		}
	}
};

template <> struct property_type_builder<property_type::script_extension> {
	template <auto... Value> static constexpr auto get() {
		constexpr char value[sizeof...(Value)]{static_cast<char>(Value)...};
		constexpr auto sc = uni::detail::script_from_string(get_string_view(value));
		if constexpr (uni::detail::is_unknown(sc)) {
			return ctll::reject{};
		} else {
			return make_property<property_type::script_extension, sc>();
		}
	}
};

template <> struct property_type_builder<property_type::age> {
	template <auto... Value> static constexpr auto get() {
		constexpr char value[sizeof...(Value)]{static_cast<char>(Value)...};
		constexpr auto age = uni::detail::age_from_string(get_string_view(value));
		if constexpr (uni::detail::is_unassigned(age)) {
			return ctll::reject{};
		} else {
			return make_binary_property<age>();
		}
	}
};

template <> struct property_type_builder<property_type::block> {
	template <auto... Value> static constexpr auto get() {
		constexpr char value[sizeof...(Value)]{static_cast<char>(Value)...};
		constexpr auto block = uni::detail::block_from_string(get_string_view(value));
		if constexpr (uni::detail::is_unknown(block)) {
			return ctll::reject{};
		} else {
			return make_binary_property<block>();
		}
	}
};

}

#endif
