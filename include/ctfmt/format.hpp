#ifndef CTFMT__TO_STRING__HPP
#define CTFMT__TO_STRING__HPP

#include "utility.hpp"
#include "../ctll/parser.hpp"
#include "fmt.hpp"
#include "actions.hpp"
#include <string_view>
#include <string>
#include <tuple>
#include "utility.hpp"
#include <cassert>
#include <array>

namespace ctfmt {

struct buffer_wrap {
	
};

struct view_wrap {
	std::string_view view;
	constexpr view_wrap(std::string_view view) noexcept: view{view} { }
	constexpr size_t size() const noexcept {
		return view.size();
	}
	constexpr auto begin() const noexcept {
		return view.begin();
	}
	constexpr auto end() const noexcept {
		return view.end();
	}
};

CTLL_FORCE_INLINE constexpr size_t my_strlen(const char * str) noexcept {
	const char * start = str;
	while (*str++ != '\0');
	return std::distance(start, str);
}


CTLL_FORCE_INLINE constexpr view_wrap input_wrap(const char * str) noexcept {
	return view_wrap{std::string_view(str)};
}

CTLL_FORCE_INLINE constexpr view_wrap input_wrap(std::string_view v) noexcept {
	return view_wrap{v};
}

CTLL_FORCE_INLINE inline view_wrap input_wrap(const std::string & s) noexcept {
	return view_wrap{std::string_view(s)};
}

template <typename... Seq, typename... Args> CTFMT_FORCE_INLINE auto calculate_size_with_impl(sequence<Seq...>, const std::tuple<Args...> & args) noexcept {
	return (calculate_size(Seq(), args) + ... + 0);
}

template <typename Seq, typename... Args> CTFMT_FORCE_INLINE auto calculate_size_with(const std::tuple<Args...> & args) noexcept {
	return calculate_size_with_impl(Seq(), args);
}

template <typename... Seq, typename It, typename... Args> CTFMT_FORCE_INLINE void format_with_impl(sequence<Seq...>, It begin, It end, const std::tuple<Args ...> & args) noexcept {
	(format_into(Seq(), begin, end, args), ...);
}

template <typename Seq, typename It, typename... Args> CTFMT_FORCE_INLINE void format_with(It begin, It end, const std::tuple<Args ...> & args) noexcept {
	format_with_impl(Seq(), begin, end, args);
}

template <typename Seq> struct format_object {
	template <typename... Args> CTFMT_FORCE_INLINE auto format(std::tuple<Args...> args) {
		std::string output;
		output.resize(calculate_size_with<Seq>(args));
		format_with<Seq>(output.begin(), output.end(), args);
		return output;
	}
	template <size_t Size, typename... Args> CTFMT_FORCE_INLINE auto format(std::array<char, Size> & buffer, std::tuple<Args...> args) noexcept {
		size_t size = calculate_size_with<Seq>(args);
		//assert(Size >= size);
		format_with<Seq>(buffer.begin(), buffer.end(), args);
		return std::string_view(buffer.begin(), size);
	}
	template <typename... Args> CTFMT_FORCE_INLINE auto operator()(Args && ... args)  {
		return format(std::make_tuple(input_wrap(std::forward<Args>(args))...));
	}
	template <size_t Size, typename... Args> CTFMT_FORCE_INLINE auto operator()(std::array<char, Size> & buffer, Args && ... args) noexcept {
		return format(buffer, std::make_tuple(input_wrap(std::forward<Args>(args))...));
	}
};

#if __cpp_nontype_template_parameter_class
template <ctll::basic_fixed_string input> CTFMT_FLATTEN constexpr auto format() noexcept {
	constexpr auto _input = input; 
#else
template <auto & input> CTFMT_FLATTEN constexpr auto format() noexcept {
	constexpr auto & _input = input; 
#endif
	
	//using tmp = typename ctll::parser<ctfmt::fmt, _input, ctfmt::actions>::template output<ctll::list<>>;
	using tmp = typename ctll::parser<ctfmt::fmt, _input, ctfmt::actions>::template output<ctll::list<sequence<>>>;
	static_assert(tmp(), "Format Expression contains syntax error.");
	return format_object<decltype(ctll::front(typename tmp::output_type()))>();
}

}

#endif
