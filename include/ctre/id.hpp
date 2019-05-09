#ifndef CTRE__ID__HPP
#define CTRE__ID__HPP

#include <type_traits>

namespace ctre {
	
template <auto... Name> struct id {
	static constexpr auto name = ctll::fixed_string<sizeof...(Name)>{{Name...}};
};
	
template <auto... Name> constexpr auto operator==(id<Name...>, id<Name...>) noexcept -> std::true_type { return {}; }

template <auto... Name1, auto... Name2> constexpr auto operator==(id<Name1...>, id<Name2...>) noexcept -> std::false_type { return {}; }
	
template <auto... Name, typename T> constexpr auto operator==(id<Name...>, T) noexcept -> std::false_type { return {}; }

}


#endif
