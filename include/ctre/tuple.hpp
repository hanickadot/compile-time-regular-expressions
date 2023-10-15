#ifndef CTRE_TUPLE_HPP
#define CTRE_TUPLE_HPP

#ifndef CTRE_IN_MODULE
#include <compare>
#include <utility>
#include <concepts>
#include <iosfwd>
#include <type_traits>
#endif

namespace ctre {

struct no_tuple_element_selected {
	template <typename T> constexpr friend auto operator+(no_tuple_element_selected, T & rhs) -> T & {
		return rhs;
	}
	template <typename T> constexpr friend auto operator+(T & lhs, no_tuple_element_selected) -> T & {
		return lhs;
	}
	template <typename T> constexpr friend auto operator+(no_tuple_element_selected, const T & rhs) -> const T & {
		return rhs;
	}
	template <typename T> constexpr friend auto operator+(const T & lhs, no_tuple_element_selected) -> const T & {
		return lhs;
	}
	constexpr friend auto operator+(no_tuple_element_selected, no_tuple_element_selected) -> no_tuple_element_selected {
		return {};
	}
};

template <bool Value, typename T> constexpr auto select_if(T && arg) -> decltype(auto) {
	if constexpr (Value) {
		return std::forward<T>(arg);
	} else {
		return no_tuple_element_selected{};
	}
}

template <size_t I, typename... Ts> constexpr auto select_nth(Ts && ... args) -> decltype(auto) {
	static_assert(I < sizeof...(Ts));

	return [&]<size_t... Idx>(std::index_sequence<Idx...>) -> decltype(auto) {
		return (no_tuple_element_selected{} + ... + select_if<I == Idx>(std::forward<Ts>(args)));
	}(std::make_index_sequence<sizeof...(Ts)>());
}

template <typename... Ts> class tuple {
	static constexpr auto build_storage(Ts... args) {
		return [...items = std::move(args)](auto && fnc) mutable -> decltype(auto) {
			return fnc(items...);
		};
	}
	
	static constexpr auto default_storage() {
		return build_storage(Ts{}...);
	}
	
	using storage_type = decltype(build_storage(std::declval<Ts>()...));
	
	mutable storage_type storage;
	
public:
	
	static constexpr auto size = std::integral_constant<size_t, sizeof...(Ts)>{};

	constexpr tuple() noexcept requires(std::default_initializable<Ts> && ...): storage{default_storage()}{ }

	constexpr tuple(Ts... args) noexcept: storage{build_storage(args...)} { }
	
	constexpr tuple(tuple && orig) noexcept requires(std::move_constructible<Ts> && ...): storage{std::move(orig.storage)} { }

	constexpr tuple(const tuple & orig) noexcept requires(std::copy_constructible<Ts> && ...): storage{orig.storage([](const auto & ... rhs) { return build_storage(rhs...); })} { }
	
	constexpr tuple & operator=(const tuple & orig) noexcept requires(std::is_copy_assignable_v<Ts> && ...) {
		storage([&](auto & ... lhs){
			orig.storage([&](const auto & ... rhs){
				((void)(lhs = rhs), ...);
			});
		});
		return *this;
	}
	
	constexpr tuple & operator=(tuple && orig) noexcept requires(std::is_move_assignable_v<Ts> && ...) {
		storage([&](auto & ... lhs){
			orig.storage([&](auto & ... rhs){
				((void)std::swap(lhs, rhs), ...);
			});
		});
		return *this;
	}
	
	constexpr ~tuple() noexcept = default;
	
	// comparison
	constexpr friend bool operator==(const tuple & lhs, const tuple & rhs) noexcept {
		return lhs.storage([&](const auto & ... lhs_value){
			return rhs.storage([&](const auto & ... rhs_value){
				return ((lhs_value == rhs_value) && ...);
			});
		});
	}
	
	constexpr friend auto operator<=>(const tuple & lhs, const tuple & rhs) noexcept {
		return lhs.storage([&](const auto & ... lhs_value){
			return rhs.storage([&](const auto & ... rhs_value){
				using result_type = std::common_comparison_category_t<decltype(lhs_value <=> rhs_value)...>;
				
				auto r = result_type::equivalent;
				
				((r = (lhs_value <=> rhs_value), std::is_eq(r)) && ...);
				
				return r;
			});
		});
	}
	
	// printing with iostream
	template <typename CharT, typename Traits> friend auto & operator<<(std::basic_ostream<CharT, Traits> & str, const tuple & tpl) {
		if constexpr (size()) {
			tpl.storage([&](const auto & first, const auto & ... tail) {
				((str << first), ((void)(str << ' ' << tail), ...));
			});
		}
		
		return str;
	}

	// support for get
	template <size_t I> constexpr auto & get() & noexcept requires (I < size()) {
		return storage([](auto & ... value) -> decltype(auto) {
			return select_nth<I>(value...);
		});
	}
	
	template <size_t I> constexpr const auto & get() const & noexcept requires (I < size()) {
		return storage([](const auto & ... value) -> decltype(auto) {
			return select_nth<I>(value...);
		});
	}
	
	template <size_t I> constexpr auto get() && noexcept requires (I < size()) {
		return storage([](auto & ... value) -> decltype(auto) {
			return select_nth<I>(std::move(value)...);
		});
	}
	
	template <size_t I> constexpr auto get() const && noexcept requires (I < size()) {
		return storage([](const auto & ... value) -> decltype(auto) {
			return select_nth<I>(std::move(value)...);
		});
	}
};

// this is niebloid to avoid writing ctre::template get<N>(tpl)
template <size_t I> constexpr auto get = [](auto && tpl) noexcept -> decltype(auto) {
	return tpl.get<I>();
};

template <typename... Ts> constexpr auto tie(Ts & ... args) -> tuple<Ts &...> {
	return {args...};
}

} // namespace ctre

// tuple protocol to support structured bindings

namespace std {

template <typename... Ts> struct tuple_size<ctre::tuple<Ts...>> : public decltype(ctre::tuple<Ts...>::size) { };

template <size_t N, typename... Ts> struct tuple_element<N, ctre::tuple<Ts...>> {
public:
	using type = decltype(
		std::declval<const ctre::tuple<Ts...> &>().template get<N>()
	);
};

} // namespace std

#endif