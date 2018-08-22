#include <ctll.hpp>
#include <ctre/pcre2.hpp>

// if we have C++20 we can get FixedString as template parameter
// without it we need to deal with template pack

template <typename T> struct identify;

#if !__has_cpp_attribute(__cpp_nontype_template_parameter_class)
template <typename CharT, CharT... input> static inline constexpr auto string = ctll::basic_fixed_string<CharT, sizeof...(input)>({input...});




template <typename CharT, CharT... charpack> __attribute__((always_inline)) constexpr auto operator""_pcre() noexcept {
	constexpr auto & input = string<CharT, charpack...>;
#else
template <basic_fixed_string input> __attribute__((always_inline)) constexpr auto operator""_pcre() noexcept {
#endif
	constexpr auto out = ctll::parser<ctre::pcre2, input>::decide();
	return bool(out);
}

void fnc() {
	static_assert("abc"_pcre);
}