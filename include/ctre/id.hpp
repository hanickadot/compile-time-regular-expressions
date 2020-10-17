#ifndef CTRE__ID__HPP
#define CTRE__ID__HPP

#include <type_traits>

namespace ctre {
	
template <auto... Name> struct id {
	static constexpr auto name = ctll::fixed_string<sizeof...(Name)>{{Name...}};
	
	friend constexpr auto operator==(id<Name...>, id<Name...>) noexcept -> std::true_type { return {}; }

	template <auto... Other> friend constexpr auto operator==(id<Name...>, id<Other...>) noexcept -> std::false_type { return {}; }
	
	template <typename T> friend constexpr auto operator==(id<Name...>, T) noexcept -> std::false_type { return {}; }

	template <typename T> friend constexpr auto operator==(T, id<Name...>) noexcept -> std::false_type { return {}; }
};

}


#endif
