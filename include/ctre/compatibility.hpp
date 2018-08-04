#ifndef CTRE_V2__CTRE__COMPATIBILITY__HPP
#define CTRE_V2__CTRE__COMPATIBILITY__HPP

namespace ctre {
	
template <typename CharT, size_t N> struct basic_fixed_string;

// check if it's really basic_fixed_string

template <typename T> struct is_basic_fixed_string {
	static constexpr const bool value = false;
};

template <typename CharT, size_t N> struct is_basic_fixed_string<basic_fixed_string<CharT, N>>{
	static constexpr const bool value = true;
};

// emulating CTiNTTP using GCC's concepts:

template <auto value> concept bool FixedString = is_basic_fixed_string<std::decay_t<decltype(value)>>::value;

template <auto value> concept bool Grammar = true;


} // ctre

#endif
