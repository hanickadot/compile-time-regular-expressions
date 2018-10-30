#ifndef CTRE__RETURN_TYPE__HPP
#define CTRE__RETURN_TYPE__HPP

#include "id.hpp"
#include "ordering.hpp"
#include <type_traits>
#include <tuple>
#include <string_view>

namespace ctre {
	
struct not_matched_tag_t { };

static constexpr inline auto not_matched = not_matched_tag_t{};
	
template <size_t Id, typename Name = void> struct captured_content {
	template <typename Iterator> struct storage {
		Iterator _begin{};
		Iterator _end{};
		
		using char_type = typename std::iterator_traits<Iterator>::value_type;
		
		equal_less_greater _elg;
	
		using name = Name;
	
		constexpr CTRE_FORCE_INLINE storage() noexcept : _elg{1, 1, 1} {}
		constexpr CTRE_FORCE_INLINE storage(not_matched_tag_t) noexcept : _elg{0, 0, 0} {}
	
		constexpr CTRE_FORCE_INLINE void matched() noexcept {
			// TODO review how matched() and unmatched() are used and revisit those algorithms.
			_elg = {1, 0, 0};
		}
		constexpr CTRE_FORCE_INLINE void unmatch() noexcept {
			_elg = {0, 0, 0};
		}

		constexpr CTRE_FORCE_INLINE void mask_elg(equal_less_greater elg) {
			_elg.equal &= elg.equal;
			_elg.less &= elg.less;
			_elg.greater &= elg.greater;
		}
		constexpr CTRE_FORCE_INLINE void mask_lg(equal_less_greater elg) {
			_elg.less &= elg.less;
			_elg.greater &= elg.greater;
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
	
		constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
			return _elg.equal;
		}

		constexpr CTRE_FORCE_INLINE bool is_less() const noexcept {
			return _elg.less;
		}

		constexpr CTRE_FORCE_INLINE bool is_greater() const noexcept {
			return _elg.greater;
		}

		constexpr CTRE_FORCE_INLINE operator equal_less_greater() const noexcept {
			return _elg;
		}

		constexpr CTRE_FORCE_INLINE operator partial_ordering() const noexcept {
			if (_elg.equal) return partial_ordering::equal;
			if (_elg.less) return partial_ordering::less;
			if (_elg.greater) return partial_ordering::greater;
			return partial_ordering::unordered;
		}
		
		constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
			return std::basic_string_view<char_type>(&*_begin, static_cast<size_t>(std::distance(_begin, _end)));
		}
		
		constexpr operator std::basic_string_view<char_type>() const noexcept {
			return to_view();
		}
		
		constexpr CTRE_FORCE_INLINE static size_t get_id() noexcept {
			return Id;
		}
	};
};

struct capture_not_exists_tag { };

static constexpr inline auto capture_not_exists = capture_not_exists_tag{};

template <typename... Captures> struct captures;

template <typename Head, typename... Tail> struct captures<Head, Tail...>: captures<Tail...> {
	Head head{};
	constexpr CTRE_FORCE_INLINE captures() noexcept { }
	constexpr CTRE_FORCE_INLINE captures(not_matched_tag_t tag) noexcept : head(tag) { }
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
};

template <> struct captures<> {
	constexpr CTRE_FORCE_INLINE captures() noexcept { }
	template <size_t> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		return false;
	}
	template <typename> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		return false;
	}
	template <size_t> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		return capture_not_exists;
	}
	template <typename> CTRE_FORCE_INLINE constexpr auto & select() const noexcept {
		return capture_not_exists;
	}
};

template <typename Iterator, typename... Captures> struct regex_results {
	using char_type = typename std::iterator_traits<Iterator>::value_type;
	
	captures<captured_content<0>::template storage<Iterator>, typename Captures::template storage<Iterator>...> _captures{};
	
	constexpr CTRE_FORCE_INLINE regex_results() noexcept { }
	constexpr CTRE_FORCE_INLINE regex_results(not_matched_tag_t tag) noexcept : _captures(tag) { }
	
	// special constructor for deducting
	constexpr CTRE_FORCE_INLINE regex_results(Iterator, ctll::list<Captures...>) noexcept { }
	
	template <size_t Id, typename = std::enable_if_t<decltype(_captures)::template exists<Id>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		return _captures.template select<Id>();
	}
	template <typename Name, typename = std::enable_if_t<decltype(_captures)::template exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() const noexcept {
		return _captures.template select<Name>();
	}
	static constexpr size_t size() noexcept {
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
	constexpr CTRE_FORCE_INLINE regex_results & mask_elg(equal_less_greater elg) noexcept {
		_captures.template select<0>().mask_elg(elg);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_results & mask_lg(equal_less_greater elg) noexcept {
		_captures.template select<0>().mask_lg(elg);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
		return bool(_captures.template select<0>());
	}
	constexpr CTRE_FORCE_INLINE bool is_less() const noexcept {
		return _captures.template select<0>().is_less();
	}
	constexpr CTRE_FORCE_INLINE bool is_greater() const noexcept {
		return _captures.template select<0>().is_greater();
	}
	constexpr CTRE_FORCE_INLINE operator equal_less_greater() const noexcept {
		return equal_less_greater(_captures.template select<0>());
	}
	constexpr CTRE_FORCE_INLINE operator partial_ordering() const noexcept {
		return partial_ordering(_captures.template select<0>());
	}
	
	constexpr operator std::basic_string_view<char_type>() const noexcept {
		return to_view();
	}
	
	constexpr auto to_view() const noexcept {
		return _captures.template select<0>().to_view();
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
};

template <typename Iterator, typename... Captures> regex_results(Iterator, ctll::list<Captures...>) -> regex_results<Iterator, Captures...>;

}

// support for structured bindings

namespace std {
	template <typename... Captures> class tuple_size<ctre::regex_results<Captures...>> : public std::integral_constant<size_t, ctre::regex_results<Captures...>::size()> { };
	
	template <size_t N, typename... Captures> class tuple_element<N, ctre::regex_results<Captures...>> {
	public:
		using type = decltype(
			std::declval<const ctre::regex_results<Captures...> &>().template get<N>()
		);
	};
}

#endif
