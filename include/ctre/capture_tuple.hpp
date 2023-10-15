#ifndef CTRE__CAPTURE_TUPLE__HPP
#define CTRE__CAPTURE_TUPLE__HPP

#include "id.hpp"
#include "utf8.hpp"

#ifndef CTRE_IN_MODULE
#include <type_traits>
#include <tuple>
#include <string_view>
#include <string>
#include <iterator>
#include <iosfwd>
#if __has_include(<charconv>)
#include <charconv>
#endif
#if __has_include(<format>)
#include <format>
#endif
#endif

namespace ctre {

template <size_t Id> struct capture_number_id { };
template <typename Name> struct capture_name_id { };
	
struct no_capture_selected {
	template <typename T> constexpr friend auto operator+(no_capture_selected, T & rhs) -> T & {
		return rhs;
	}
	template <typename T> constexpr friend auto operator+(T & lhs, no_capture_selected) -> T & {
		return lhs;
	}
	template <typename T> constexpr friend auto operator+(no_capture_selected, const T & rhs) -> const T & {
		return rhs;
	}
	template <typename T> constexpr friend auto operator+(const T & lhs, no_capture_selected) -> const T & {
		return lhs;
	}
	constexpr friend auto operator+(no_capture_selected, no_capture_selected) -> no_capture_selected {
		return {};
	}
};

template <size_t Id, typename Name = void> struct capture_identifier {
	constexpr friend bool operator==(capture_identifier, capture_number_id<Id>) noexcept {
		return true;
	}
	template <size_t OtherId> constexpr friend bool operator==(capture_identifier, capture_number_id<OtherId>) noexcept {
		return false;
	}
	constexpr friend bool operator==(capture_identifier, capture_name_id<Name>) noexcept {
		return true;
	}
	template <typename OtherName> constexpr friend bool operator==(capture_identifier, capture_name_id<OtherName>) noexcept {
		return false;
	}
};

template <typename Iterator, typename Sentinel = Iterator> struct capture_tuple_storage {
	using iterator_type = Iterator;

	[[no_unique_address]] Iterator first{};
	[[no_unique_address]] Sentinel last{};
	
	constexpr capture_tuple_storage() = default;
	constexpr capture_tuple_storage(const capture_tuple_storage &) = default;
	constexpr capture_tuple_storage(capture_tuple_storage &&) = default;
	
	constexpr capture_tuple_storage & operator=(const capture_tuple_storage &) = default;
	constexpr capture_tuple_storage & operator=(capture_tuple_storage &&) = default;
	
	
	constexpr bool matched() noexcept {
		return true; // TODO
	}
};

template <typename Id, typename Storage> struct capture_tuple_item {
	using identifier_type = Id;
	using storage_type = Storage;
	
	static_assert(std::move_constructible<storage_type>);
	static_assert(std::copy_constructible<storage_type>);
	
	
	[[no_unique_address]] storage_type storage{};
	
	//constexpr capture_tuple_item() = default;
	
	template <size_t OtherId> constexpr friend auto operator==(const capture_tuple_item & item, capture_number_id<OtherId> rhs) noexcept {
		if constexpr (identifier_type{} == rhs) {
			return item;
		} else {
			return no_capture_selected{};
		}
	}
	
	template <typename OtherName> constexpr friend auto operator==(const capture_tuple_item & item, capture_name_id<OtherName> rhs) noexcept {
		if constexpr (identifier_type{} == rhs) {
			return item;
		} else {
			return no_capture_selected{};
		}
	}
	
	template <size_t OtherId> constexpr friend auto operator==(capture_tuple_item & item, capture_number_id<OtherId> rhs) noexcept {
		if constexpr (identifier_type{} == rhs) {
			return item;
		} else {
			return no_capture_selected{};
		}
	}

	template <typename OtherName> constexpr friend auto operator==(capture_tuple_item & item, capture_name_id<OtherName> rhs) noexcept {
		if constexpr (identifier_type{} == rhs) {
			return item;
		} else {
			return no_capture_selected{};
		}
	}
};

template <typename> struct foo { };

template <typename...> struct identify;

template <bool Value, typename T> constexpr decltype(auto) select_if(T && arg) {
	if constexpr (Value) {
		return std::forward<T>(arg);
	} else {
		return no_capture_selected{};
	}
}

template <size_t I, typename... Ts> constexpr auto & select_nth(Ts && ... args) {
	static_assert(I < sizeof...(Ts));

	const auto lmb = [&]<size_t... Idx>(std::index_sequence<Idx...>) -> decltype(auto) {
		//identify<decltype(select_if<I == Idx>(std::forward<Ts>(args)))...> i;
		
		return (no_capture_selected{} + ... + select_if<I == Idx>(std::forward<Ts>(args)));
	};

	//identify<decltype(lmb(std::make_index_sequence<sizeof...(Ts)>()))> i;

	return lmb(std::make_index_sequence<sizeof...(Ts)>());
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
	
	static constexpr auto copy_storage(const Ts & ... args) {
		return build_storage(args...);
	}
	
	using storage_type = decltype(build_storage(Ts{}...));
	
	mutable storage_type storage;
	
public:
	
	static constexpr auto size = std::integral_constant<size_t, sizeof...(Ts)>{};

	constexpr tuple() noexcept: storage{default_storage()}{ }

	constexpr tuple(Ts... args) noexcept: storage{build_storage(args...)} { }
	
	constexpr tuple(tuple && orig) noexcept: storage{std::move(orig.storage)} { }

	constexpr tuple(const tuple & orig) noexcept: storage{orig.storage([](const auto & ... rhs) { return build_storage(rhs...); })} { }
	
	constexpr tuple & operator=(const tuple & orig) noexcept {
		storage([&](auto & ... lhs){
			orig.storage([&](const auto & ... rhs){
				((void)(lhs = rhs), ...);
			});
		});
		return *this;
	}
	
	constexpr tuple & operator=(tuple && orig) noexcept {
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
	
	// printing
	template <typename CharT, typename Traits> friend auto & operator<<(std::basic_ostream<CharT, Traits> & str, const tuple & tpl) {
		if constexpr (size()) {
			tpl.storage([&](const auto & first, const auto & ... tail) {
				(str << first);
				((void)(str << ';' << tail), ...);
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

template <size_t I> constexpr auto get = [](auto && tpl) noexcept -> decltype(auto) {
	return tpl.get<I>();
};

template <typename Storage, typename... CapturesIds> struct capture_tuple {
	static constexpr auto build_lambda() {
		return [...items = foo<CapturesIds>{}]() {
			//return ((items == Id{}) + ... + no_capture_selected{});
		};
	}

	using tuple_storage_type = decltype(build_lambda());
	
	tuple_storage_type internal_tuple{build_lambda()};
	
	constexpr capture_tuple(...) noexcept { }
	constexpr capture_tuple(const capture_tuple &) noexcept { }
	constexpr capture_tuple(capture_tuple &&) noexcept { }
	
	constexpr capture_tuple & operator=(const capture_tuple & other) noexcept {
		internal_tuple = other.internal_tuple;
		return *this;
	}
	constexpr capture_tuple & operator=(capture_tuple && other) noexcept {
		internal_tuple = std::move(other.internal_tuple);
		return *this;
	}
	
	constexpr capture_tuple & matched() noexcept {
		// TODO
		return *this;
	}
	
	constexpr capture_tuple & unmatch() noexcept {
		// TODO
		return *this;
	}
	
	
	constexpr auto get_end_position() const noexcept {
		return typename Storage::iterator_type{};
	}
	
	constexpr operator bool() const noexcept {
		return true;
	}
	
	template <size_t Id> constexpr capture_tuple & start_capture(const typename Storage::iterator_type &) noexcept {
		return *this;
	}
	
	template <size_t Id> constexpr capture_tuple & end_capture(const typename Storage::iterator_type &) noexcept {
		return *this;
	}
	
	
	constexpr capture_tuple & set_start_mark(const typename Storage::iterator_type &) noexcept {
		return *this;
	}
	
	constexpr capture_tuple & set_end_mark(const typename Storage::iterator_type &) noexcept {
		return *this;
	}
};

template <typename Iterator, typename... CapturesIds> capture_tuple(Iterator, ctll::list<CapturesIds...>) -> capture_tuple<capture_tuple_storage<Iterator>, CapturesIds...>;

template <typename ResultIterator, typename Pattern> using return_type = decltype(capture_tuple(std::declval<ResultIterator>(), find_captures(Pattern{})));

}

namespace std {

template <typename... Ts> struct tuple_size<ctre::tuple<Ts...>> : public decltype(ctre::tuple<Ts...>::size) { };

template <size_t N, typename... Ts> struct tuple_element<N, ctre::tuple<Ts...>> {
public:
	using type = decltype(
		std::declval<const ctre::tuple<Ts...> &>().template get<N>()
	);
};

}

#endif