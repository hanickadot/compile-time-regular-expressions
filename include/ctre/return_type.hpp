#ifndef CTRE__RETURN_TYPE__HPP
#define CTRE__RETURN_TYPE__HPP

#include "id.hpp"
#include "utf8.hpp"
#include <type_traits>
#include <tuple>
#include <string_view>
#include <string>
#include <iterator>

namespace ctre {

constexpr bool is_random_accessible(const std::random_access_iterator_tag &) { return true; }
constexpr bool is_random_accessible(...) { return false; }


struct not_matched_tag_t { };

static constexpr inline auto not_matched = not_matched_tag_t{};
	
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
	
		// TODO explicit
		constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
			return _matched;
		}
		
		constexpr CTRE_FORCE_INLINE const auto * data_unsafe() const noexcept {
			#if __cpp_char8_t >= 201811
			if constexpr (std::is_same_v<Iterator, utf8_iterator>) {
				return _begin.ptr;
			} else {
				return &*_begin;
			}
			#else
			return &*_begin;
			#endif
		}
		
		constexpr CTRE_FORCE_INLINE const auto * data() const noexcept {
			constexpr bool must_be_contiguous_iterator = is_random_accessible(typename std::iterator_traits<Iterator>::iterator_category{});
			
			static_assert(must_be_contiguous_iterator, "To access result as a pointer you need to provide a random access iterator/range to regex.");
			
			return data_unsafe();
		}

		constexpr CTRE_FORCE_INLINE auto size() const noexcept {
			return static_cast<size_t>(std::distance(begin(), end()));
		}
		
		constexpr CTRE_FORCE_INLINE size_t unit_size() const noexcept {
			#if __cpp_char8_t >= 201811
			if constexpr (std::is_same_v<Iterator, utf8_iterator>) {
				return static_cast<size_t>(std::distance(_begin.ptr, _end.ptr));
			}
			#endif
			return static_cast<size_t>(std::distance(begin(), end()));
		}

		template <typename It = Iterator> constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
			// random access, because C++ (waving hands around)
			constexpr bool must_be_contiguous_iterator = is_random_accessible(typename std::iterator_traits<std::remove_const_t<It>>::iterator_category{});
			
			static_assert(must_be_contiguous_iterator, "To convert capture into a basic_string_view you need to provide a pointer or a contiguous iterator/range to regex.");
	
			return std::basic_string_view<char_type>(data_unsafe(), static_cast<size_t>(unit_size()));
		}
		
		constexpr CTRE_FORCE_INLINE std::basic_string<char_type> to_string() const noexcept {
			#if __cpp_char8_t >= 201811
			if constexpr (std::is_same_v<Iterator, utf8_iterator>) {
				return std::basic_string<char_type>(data_unsafe(), static_cast<size_t>(unit_size()));
			}
			#endif
			return std::basic_string<char_type>(begin(), end());
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
	};
};

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
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
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
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
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
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
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
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
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
	
	template <size_t Id, typename = std::enable_if_t<decltype(_captures)::template exists<Id>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		return _captures.template select<Id>();
	}
	template <typename Name, typename = std::enable_if_t<decltype(_captures)::template exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		return _captures.template select<Name>();
	}
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
	template <ctll::fixed_string Name, typename = std::enable_if_t<decltype(_captures)::template exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
#else
	template <const auto & Name, typename = std::enable_if_t<decltype(_captures)::template exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
#endif
		return _captures.template select<Name>();
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
};

template <typename Iterator, typename... Captures> regex_results(Iterator, ctll::list<Captures...>) -> regex_results<Iterator, Captures...>;

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
