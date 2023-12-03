#ifndef CTRE__RETURN_TYPE__HPP
#define CTRE__RETURN_TYPE__HPP

#include "id.hpp"
#include "utf8.hpp"
#include <type_traits>
#include <tuple>
#include <string_view>
#include <string>
#include <iterator>
#ifdef _MSC_VER
#include <memory>
#endif
#include <iosfwd>
#if __has_include(<charconv>)
#include <charconv>
#endif
#if __cpp_concepts >= 202002L
#include <concepts>
#endif

namespace ctre {

constexpr auto is_random_accessible_f(const std::random_access_iterator_tag &) { return std::true_type{}; }
constexpr auto is_random_accessible_f(...) { return std::false_type{}; }

template <typename T> constexpr auto is_reverse_iterator_f(const std::reverse_iterator<T> &) { return std::true_type{}; }
constexpr auto is_reverse_iterator_f(...) { return std::false_type{}; }

template <typename T> constexpr bool is_random_accessible = decltype(is_random_accessible_f(std::declval<const T &>())){};
template <typename T> constexpr bool is_reverse_iterator = decltype(is_reverse_iterator_f(std::declval<const T &>())){};

struct not_matched_tag_t { };

constexpr inline auto not_matched = not_matched_tag_t{};
	
template <size_t Id, typename Name = void> struct captured_content {
	template <typename Iterator> class storage {
		Iterator _begin{};
		Iterator _end{};
		
		bool _matched{false};
	public:
		using char_type = typename std::iterator_traits<Iterator>::value_type;
		
		using name = Name;
	
		constexpr CTRE_FORCE_INLINE storage() noexcept {}
	
		constexpr CTRE_FORCE_INLINE void matched() noexcept {
			_matched = true;
		}
		constexpr CTRE_FORCE_INLINE void unmatch() noexcept {
			_matched = false;
		}
		constexpr CTRE_FORCE_INLINE void set_start(Iterator pos) noexcept {
			_begin = pos;
		}
		constexpr CTRE_FORCE_INLINE storage & set_end(Iterator pos) noexcept {
			_end = pos;
			return *this;
		}
		constexpr CTRE_FORCE_INLINE Iterator get_end() const noexcept {
			return _end;
		}
		
	
		constexpr auto begin() const noexcept {
			return _begin;
		}
		constexpr auto end() const noexcept {
			return _end;
		}
	
		constexpr explicit CTRE_FORCE_INLINE operator bool() const noexcept {
			return _matched;
		}
		
		template <typename It = Iterator> constexpr CTRE_FORCE_INLINE const auto * data_unsafe() const noexcept {
			static_assert(!is_reverse_iterator<It>, "Iterator in your capture must not be reverse!");
			
			#if __cpp_char8_t >= 201811
			if constexpr (std::is_same_v<Iterator, utf8_iterator>) {
				return _begin.ptr;
			} else { // I'm doing this to avoid warning about dead code
			#endif
			
			#ifdef _MSC_VER
			return std::to_address(_begin); // I'm enabling this only for MSVC for now, as some clang old versions with various libraries (random combinations) has different problems
			#else
			return &*_begin; 
			#endif
			
			#if __cpp_char8_t >= 201811
			}
			#endif
		}
		
		template <typename It = Iterator> constexpr CTRE_FORCE_INLINE const auto * data() const noexcept {
			constexpr bool must_be_contiguous_nonreverse_iterator = is_random_accessible<typename std::iterator_traits<It>::iterator_category> && !is_reverse_iterator<It>;
			
			static_assert(must_be_contiguous_nonreverse_iterator, "To access result as a pointer you need to provide a random access iterator/range to regex (which is not reverse iterator based).");
			
			return data_unsafe();
		}

		constexpr CTRE_FORCE_INLINE auto size() const noexcept {
			return static_cast<size_t>(std::distance(begin(), end()));
		}
		
		constexpr CTRE_FORCE_INLINE size_t unit_size() const noexcept {
			#if __cpp_char8_t >= 201811
			if constexpr (std::is_same_v<Iterator, utf8_iterator>) {
				return static_cast<size_t>(std::distance(_begin.ptr, _end.ptr));
			} else {
				return static_cast<size_t>(std::distance(begin(), end()));
			}
			#else
			return static_cast<size_t>(std::distance(begin(), end()));
			#endif
		}
		
#if __has_include(<charconv>)
		template <typename R = int, typename... Ts> constexpr CTRE_FORCE_INLINE auto to_number(Ts && ... args) const noexcept -> R {
			R result{0};
			const auto view = to_view();
			std::from_chars(view.data(), view.data() + view.size(), result, std::forward<Ts>(args)...);
			return result;
		}
#endif
		
		template <typename It = Iterator> constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
			// random access, because C++ (waving hands around)
			constexpr bool must_be_nonreverse_contiguous_iterator = is_random_accessible<typename std::iterator_traits<std::remove_const_t<It>>::iterator_category> && !is_reverse_iterator<It>;
			
			static_assert(must_be_nonreverse_contiguous_iterator, "To convert capture into a basic_string_view you need to provide a pointer or a contiguous non-reverse iterator/range to regex.");
	
			return std::basic_string_view<char_type>(data_unsafe(), static_cast<size_t>(unit_size()));
		}
		
		constexpr CTRE_FORCE_INLINE std::basic_string<char_type> to_string() const noexcept {
			#if __cpp_char8_t >= 201811
			if constexpr (std::is_same_v<Iterator, utf8_iterator>) {
				return std::basic_string<char_type>(data_unsafe(), static_cast<size_t>(unit_size()));
			} else {
				return std::basic_string<char_type>(begin(), end());
			}
			#else
			return std::basic_string<char_type>(begin(), end());
			#endif
		}
		
		constexpr CTRE_FORCE_INLINE auto view() const noexcept {
			return to_view();
		}
		
		constexpr CTRE_FORCE_INLINE auto str() const noexcept {
			return to_string();
		}
		
		constexpr CTRE_FORCE_INLINE operator std::basic_string_view<char_type>() const noexcept {
			return to_view();
		}
		
		constexpr CTRE_FORCE_INLINE explicit operator std::basic_string<char_type>() const noexcept {
			return to_string();
		}
		
		constexpr CTRE_FORCE_INLINE static size_t get_id() noexcept {
			return Id;
		}
		
		friend CTRE_FORCE_INLINE constexpr bool operator==(const storage & lhs, std::basic_string_view<char_type> rhs) noexcept {
			return bool(lhs) ? lhs.view() == rhs : false;
		}
		friend CTRE_FORCE_INLINE constexpr bool operator!=(const storage & lhs, std::basic_string_view<char_type> rhs) noexcept {
			return bool(lhs) ? lhs.view() != rhs : false;
		}
		friend CTRE_FORCE_INLINE constexpr bool operator==(std::basic_string_view<char_type> lhs, const storage & rhs) noexcept {
			return bool(rhs) ? lhs == rhs.view() : false;
		}
		friend CTRE_FORCE_INLINE constexpr bool operator!=(std::basic_string_view<char_type> lhs, const storage & rhs) noexcept {
			return bool(rhs) ? lhs != rhs.view() : false;
		}
		friend CTRE_FORCE_INLINE std::ostream & operator<<(std::ostream & str, const storage & rhs) {
			return str << rhs.view();
		}
	};
};

#if __cpp_concepts >= 202002L
template <typename T> concept capture_group = requires(const T & cap) {
	{ T::get_id() } -> std::same_as<size_t>;
	{ cap.view() };
	{ cap.str() };
	{ cap.to_string() };
	{ cap.to_view() };
	{ cap.unit_size() } -> std::same_as<size_t>;
	{ cap.size() } -> std::same_as<size_t>;
	{ static_cast<bool>(cap) };
	{ cap.data() };
	{ cap.data_unsafe() };
	{ cap.begin() };
	{ cap.end() };
};
#endif

struct capture_not_exists_tag { };

static constexpr inline auto capture_not_exists = capture_not_exists_tag{};

template <typename... Captures> struct captures;

template <typename Head, typename... Tail> struct captures<Head, Tail...>: captures<Tail...> {
	Head head{};
	constexpr CTRE_FORCE_INLINE captures() noexcept { }
	template <size_t id> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		if constexpr (id == Head::get_id()) {
			return true;
		} else {
			return captures<Tail...>::template exists<id>();
		}
	}
	template <typename Name> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		if constexpr (std::is_same_v<Name, typename Head::name>) {
			return true;
		} else {
			return captures<Tail...>::template exists<Name>();
		}
	}
#if CTRE_CNTTP_COMPILER_CHECK
	template <ctll::fixed_string Name> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
#else
	template <const auto & Name> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
#endif
		if constexpr (std::is_same_v<typename Head::name, void>) {
			return captures<Tail...>::template exists<Name>();
		} else {
			if constexpr (Head::name::name.is_same_as(Name)) {
				return true;
			} else {
				return captures<Tail...>::template exists<Name>();
			}
		}
	}
	template <size_t id> CTRE_FORCE_INLINE constexpr auto & select() noexcept {
		if constexpr (id == Head::get_id()) {
			return head;
		} else {
			return captures<Tail...>::template select<id>();
		}
	}
	template <typename Name> CTRE_FORCE_INLINE constexpr auto & select() noexcept {
		if constexpr (std::is_same_v<Name, typename Head::name>) {
			return head;
		} else {
			return captures<Tail...>::template select<Name>();
		}
	}
	template <size_t id> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		if constexpr (id == Head::get_id()) {
			return head;
		} else {
			return captures<Tail...>::template select<id>();
		}
	}
	template <typename Name> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		if constexpr (std::is_same_v<Name, typename Head::name>) {
			return head;
		} else {
			return captures<Tail...>::template select<Name>();
		}
	}
#if CTRE_CNTTP_COMPILER_CHECK
	template <ctll::fixed_string Name> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
#else
	template <const auto & Name> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
#endif
		if constexpr (std::is_same_v<typename Head::name, void>) {
			return captures<Tail...>::template select<Name>();
		} else {
			if constexpr (Head::name::name.is_same_as(Name)) {
				return head;
			} else {
				return captures<Tail...>::template select<Name>();
			}
		}
	}
};

template <> struct captures<> {
	constexpr CTRE_FORCE_INLINE captures() noexcept { }
	template <size_t> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		return false;
	}
	template <typename> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		return false;
	}
#if CTRE_CNTTP_COMPILER_CHECK
	template <ctll::fixed_string> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
#else
	template <const auto &> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
#endif
		return false;
	}
	template <size_t> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		return capture_not_exists;
	}
	template <typename> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		return capture_not_exists;
	}
#if CTRE_CNTTP_COMPILER_CHECK
	template <ctll::fixed_string> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
#else
	template <const auto &> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
#endif
		return capture_not_exists;
	}
};

template <typename Iterator, typename... Captures> class regex_results {
	captures<captured_content<0>::template storage<Iterator>, typename Captures::template storage<Iterator>...> _captures{};
public:
	using char_type = typename std::iterator_traits<Iterator>::value_type;
	
	constexpr CTRE_FORCE_INLINE regex_results() noexcept { }
	constexpr CTRE_FORCE_INLINE regex_results(not_matched_tag_t) noexcept { }
	
	// special constructor for deducting
	constexpr CTRE_FORCE_INLINE regex_results(Iterator, ctll::list<Captures...>) noexcept { }
	
	template <size_t Id> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		constexpr bool capture_of_provided_id_must_exists = decltype(_captures)::template exists<Id>();
		static_assert(capture_of_provided_id_must_exists);
		
		if constexpr (capture_of_provided_id_must_exists) {
			return _captures.template select<Id>();
		} else {
			return false;
		}
	}
	template <typename Name> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		constexpr bool capture_of_provided_name_must_exists = decltype(_captures)::template exists<Name>();
		static_assert(capture_of_provided_name_must_exists);
	
		if constexpr (capture_of_provided_name_must_exists) {
			return _captures.template select<Name>();
		} else {
			return false;
		}
	}
#if CTRE_CNTTP_COMPILER_CHECK
	template <ctll::fixed_string Name> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
#else
	template <const auto & Name> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
#endif
		constexpr bool capture_of_provided_name_must_exists = decltype(_captures)::template exists<Name>();
		static_assert(capture_of_provided_name_must_exists);
	
		if constexpr (capture_of_provided_name_must_exists) {
			return _captures.template select<Name>();
		} else {
			return false;
		}
	}
	static constexpr size_t count() noexcept {
		return sizeof...(Captures) + 1;
	}
	constexpr CTRE_FORCE_INLINE regex_results & matched() noexcept {
		_captures.template select<0>().matched();
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_results & unmatch() noexcept {
		_captures.template select<0>().unmatch();
		return *this;
	}
	constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
		return bool(_captures.template select<0>());
	}
	
	constexpr CTRE_FORCE_INLINE operator std::basic_string_view<char_type>() const noexcept {
		return to_view();
	}
	
	constexpr CTRE_FORCE_INLINE explicit operator std::basic_string<char_type>() const noexcept {
		return to_string();
	}
	
#if __has_include(<charconv>)
	template <typename R = int, typename... Ts> constexpr CTRE_FORCE_INLINE auto to_number(Ts && ... args) const noexcept -> R {
		return _captures.template select<0>().template to_number<R>(std::forward<Ts>(args)...);
	}
#endif
	
	constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
		return _captures.template select<0>().to_view();
	}
	
	constexpr CTRE_FORCE_INLINE auto to_string() const noexcept {
		return _captures.template select<0>().to_string();
	}
	
	constexpr CTRE_FORCE_INLINE auto view() const noexcept {
		return _captures.template select<0>().view();
	}
	
	constexpr CTRE_FORCE_INLINE auto str() const noexcept {
		return _captures.template select<0>().to_string();
	}
	
	constexpr CTRE_FORCE_INLINE size_t size() const noexcept {
		return _captures.template select<0>().size();
	}
	
	constexpr CTRE_FORCE_INLINE const auto * data() const noexcept {
		return _captures.template select<0>().data();
	}
	
	constexpr CTRE_FORCE_INLINE regex_results & set_start_mark(Iterator pos) noexcept {
		_captures.template select<0>().set_start(pos);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_results & set_end_mark(Iterator pos) noexcept {
		_captures.template select<0>().set_end(pos);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE Iterator get_end_position() const noexcept {
		return _captures.template select<0>().get_end();
	}
	template <size_t Id> CTRE_FORCE_INLINE constexpr regex_results & start_capture(Iterator pos) noexcept {
		_captures.template select<Id>().set_start(pos);
		return *this;
	}
	template <size_t Id> CTRE_FORCE_INLINE constexpr regex_results & end_capture(Iterator pos) noexcept {
		_captures.template select<Id>().set_end(pos).matched();
		return *this;
	}
	constexpr auto begin() const noexcept {
		return _captures.template select<0>().begin();
	}
	constexpr auto end() const noexcept {
		return _captures.template select<0>().end();
	}
	friend CTRE_FORCE_INLINE constexpr bool operator==(const regex_results & lhs, std::basic_string_view<char_type> rhs) noexcept {
		return bool(lhs) ? lhs.view() == rhs : false;
	}
	friend CTRE_FORCE_INLINE constexpr bool operator!=(const regex_results & lhs, std::basic_string_view<char_type> rhs) noexcept {
		return bool(lhs) ? lhs.view() != rhs : true;
	}
	friend CTRE_FORCE_INLINE constexpr bool operator==(std::basic_string_view<char_type> lhs, const regex_results & rhs) noexcept {
		return bool(rhs) ? lhs == rhs.view() : false;
	}
	friend CTRE_FORCE_INLINE constexpr bool operator!=(std::basic_string_view<char_type> lhs, const regex_results & rhs) noexcept {
		return bool(rhs) ? lhs != rhs.view() : true;
	}
	friend CTRE_FORCE_INLINE std::ostream & operator<<(std::ostream & str, const regex_results & rhs) {
		return str << rhs.view();
	}
};

template <size_t Id, typename Iterator, typename... Captures> constexpr auto get(const regex_results<Iterator, Captures...> & results) noexcept {
	return results.template get<Id>();
}

template <typename Iterator, typename... Captures> regex_results(Iterator, ctll::list<Captures...>) -> regex_results<Iterator, Captures...>;

template <typename> struct is_regex_results_t: std::false_type { };

template <typename Iterator, typename... Captures> struct is_regex_results_t<regex_results<Iterator, Captures...>>: std::true_type { };

template <typename T> constexpr bool is_regex_results_v = is_regex_results_t<T>();

#if __cpp_concepts >= 202002L
template <typename T> concept capture_groups = is_regex_results_v<T>;
#endif

template <typename ResultIterator, typename Pattern> using return_type = decltype(regex_results(std::declval<ResultIterator>(), find_captures(Pattern{})));

}

// support for structured bindings

#ifndef __EDG__
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

namespace std {
	template <typename... Captures> struct tuple_size<ctre::regex_results<Captures...>> : public std::integral_constant<size_t, ctre::regex_results<Captures...>::count()> { };
	
	template <size_t N, typename... Captures> struct tuple_element<N, ctre::regex_results<Captures...>> {
	public:
		using type = decltype(
			std::declval<const ctre::regex_results<Captures...> &>().template get<N>()
		);
	};
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif

#endif
