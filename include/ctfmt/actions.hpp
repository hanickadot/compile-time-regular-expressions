#ifndef CTFMT__ACTIONS__HPP
#define CTFMT__ACTIONS__HPP

#include <cstdint>
#include <algorithm>
#include <tuple>

namespace ctfmt {

template <int64_t Number> struct number { };
template <int64_t Value> struct placeholder { };
template <char... Text> struct text { };

template <int64_t Value, typename... Args> constexpr size_t calculate_size(placeholder<Value>, const std::tuple<Args...> & tuple) {
	return std::get<Value>(tuple).size();
}

template <char... Text, typename... Args> constexpr size_t calculate_size(text<Text...>, const std::tuple<Args...> &) {
	return sizeof...(Text);
}

template <int64_t Value, typename It, typename... Args> constexpr void format_into(placeholder<Value>, It & begin, const It, const std::tuple<Args...> & tuple) {
	const auto & obj = std::get<Value>(tuple);
	begin = std::copy(obj.begin(), obj.end(), begin);
}

template <char... Text, typename It, typename... Args> constexpr void format_into(text<Text...>, It & begin, const It, const std::tuple<Args...> &) {
	((*begin++ = Text), ...);
}

template <typename...> struct sequence { };

struct actions {


template <auto V, typename... Ts> static constexpr auto apply(fmt::start_text, ctll::term<V>, ctll::list<Ts...>) {
	return ctll::list<text<V>, Ts...>{};
}

template <char... Text, auto V, typename... Ts> static constexpr auto apply(fmt::push_text, ctll::term<V>, ctll::list<text<Text...>, Ts...>) {
	return ctll::list<text<Text..., V>, Ts...>{};
}

template <char... Text, typename Symbol, typename... Ts> static constexpr auto apply(fmt::finish, Symbol, ctll::list<text<Text...>, sequence<Ts...>>) {
	return ctll::list<sequence<Ts..., text<Text...>>>{};
}

template <auto V, typename... Ts> static constexpr auto apply(fmt::start_digit, ctll::term<V>, ctll::list<Ts...>) {
	return ctll::list<number<(V - '0')>, Ts...>{};
}

template <int64_t Value, auto V, typename... Ts> static constexpr auto apply(fmt::push_digit, ctll::term<V>, ctll::list<number<Value>, Ts...>) {
	return ctll::list<number<(Value * 10 + (V - '0'))>, Ts...>{};
}

template <int64_t Value, typename Symbol, typename... Ts> static constexpr auto apply(fmt::finish, Symbol, ctll::list<number<Value>, sequence<Ts...>>) {
	return ctll::list<sequence<Ts..., placeholder<Value>>>{};
}


	
};

}

#endif
