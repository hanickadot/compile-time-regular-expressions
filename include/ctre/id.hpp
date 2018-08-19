#ifndef CTRE_V2__CTRE__ID__HPP
#define CTRE_V2__CTRE__ID__HPP

namespace ctre {
	
template <auto... Str> struct pack_identifier {
	template <auto... OtherStr> constexpr bool operator==(pack_identifier<OtherStr...>) const noexcept {
		return false;
	}
	constexpr bool operator==(pack_identifier) const noexcept {
		return true;
	}
};
	
namespace id_literals {

template <typename CharT, CharT... Str> constexpr auto operator""_id() noexcept
	return pack_identifier<Str...>();
}
	
}

#endif
