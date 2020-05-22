#ifndef CTRE__RETURN_TYPE__HPP
#define CTRE__RETURN_TYPE__HPP

#include "id.hpp"
#include <type_traits>
#include <tuple>
#include <string_view>
#include <string>

namespace ctre {
	
struct not_matched_tag_t { };

static constexpr inline auto not_matched = not_matched_tag_t{};

struct other_ids {
	constexpr other_ids(...) noexcept { }
};

struct other_types {
	constexpr other_types(...) noexcept { }
};

struct without_name { };

#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
struct other_names { };

template <CTLL_FIXED_STRING Name> struct name_tag {
	constexpr operator other_names() const noexcept { return other_names{}; }
};

template <typename Name> struct get_name_tag_t {
	// around GCC ICE
	static constexpr auto value = Name::name;
	using type = name_tag<value>;
};

template <> struct get_name_tag_t<without_name> {
	using type = without_name;
};

template <typename Name> using get_name_tag = get_name_tag_t<Name>::type;
#endif



template <size_t Id> struct id_tag { };

struct ignore_result_tag {
	constexpr friend auto operator||(ignore_result_tag, ignore_result_tag) -> ignore_result_tag { return {}; }
	template <typename T> constexpr friend decltype(auto) operator||(ignore_result_tag, T && val) { return std::forward<T>(val); }
	template <typename T> constexpr friend decltype(auto) operator||(T && val, ignore_result_tag) { return std::forward<T>(val); }
};

static constexpr ignore_result_tag ignore_result{};
	
template <size_t Id, typename Name = void> struct captured_content {
	template <typename Iterator> class storage {
		Iterator _begin{};
		Iterator _end{};
		
		bool _matched{false};
	public:
		using char_type = typename std::iterator_traits<Iterator>::value_type;
		
		using name = std::conditional_t<std::is_same_v<Name, void>, without_name, Name>;
	
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
	
		constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
			return _matched;
		}

		constexpr CTRE_FORCE_INLINE auto size() const noexcept {
			return static_cast<size_t>(std::distance(_begin, _end));
		}

		constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
			return std::basic_string_view<char_type>(&*_begin, static_cast<size_t>(std::distance(_begin, _end)));
		}
		
		constexpr CTRE_FORCE_INLINE auto to_string() const noexcept {
			return std::basic_string<char_type>(begin(), end());
		}
		
		constexpr CTRE_FORCE_INLINE auto view() const noexcept {
			return std::basic_string_view<char_type>(&*_begin, static_cast<size_t>(std::distance(_begin, _end)));
		}
		
		constexpr CTRE_FORCE_INLINE auto str() const noexcept {
			return std::basic_string<char_type>(begin(), end());
		}
		
		constexpr CTRE_FORCE_INLINE operator std::basic_string_view<char_type>() const noexcept {
			return to_view();
		}
		
		constexpr CTRE_FORCE_INLINE explicit operator std::basic_string<char_type>() const noexcept {
			return to_string();
		}
		
		
		constexpr CTRE_FORCE_INLINE const auto & select_by_id(id_tag<Id>) const noexcept {
			return *this;
		}
		constexpr CTRE_FORCE_INLINE auto & select_by_id(id_tag<Id>) noexcept {
			return *this;
		}
		constexpr CTRE_FORCE_INLINE auto select_by_id(other_ids) const noexcept {
			return ignore_result;
		}
		
		constexpr CTRE_FORCE_INLINE auto & select_by_type(name) noexcept {
			return *this;
		}
		constexpr CTRE_FORCE_INLINE auto select_by_type(other_types) const noexcept {
			return ignore_result;
		}

        constexpr CTRE_FORCE_INLINE static bool has_type(name) noexcept {
            return true;
        }

        constexpr CTRE_FORCE_INLINE static bool has_type(other_types) noexcept {
            return false;
        }
		
		constexpr CTRE_FORCE_INLINE static bool has_name([[maybe_unused]] std::basic_string_view<char32_t> requested) noexcept {
			if constexpr (std::is_same_v<Name, void>) {
				return false;
			} else {
				return name::name == requested;
			}
		}

        constexpr CTRE_FORCE_INLINE static bool has_id(size_t requested) noexcept {
            return requested == Id;
        }
		
		#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
		constexpr CTRE_FORCE_INLINE auto & select_by_name(get_name_tag<name>) noexcept {
			return *this;
		}
		constexpr CTRE_FORCE_INLINE auto select_by_name(other_names) const noexcept {
			return ignore_result;
		}
		#else
		// without CNTTP you can't do the trick :(
		template <CTLL_FIXED_STRING RequestedName> constexpr decltype(auto) select_by_name() noexcept {
			if constexpr (has_name(RequestedName)) {
				return *this;
			} else {
				return ignore_result;
			}
		}
		#endif
	};
};

template <typename... Content> struct captures: Content... {
	template <size_t Id> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
    	return (Content::has_id(Id) || ...);
	}
	
	template <CTLL_FIXED_STRING Name> CTRE_FORCE_INLINE static constexpr auto exists() noexcept {
		return (Content::has_name(Name) || ...);
	}
	
	template <typename Name> CTRE_FORCE_INLINE static constexpr auto exists() noexcept {
		return (Content::has_type(Name{}) || ...);
	}
	
	
	template <size_t Id, typename = std::enable_if_t<exists<Id>()>> CTRE_FORCE_INLINE constexpr const auto & get() const noexcept {
		return (Content::select_by_id(id_tag<Id>{}) || ...);
	}

	template <size_t Id, typename = std::enable_if_t<exists<Id>()>> CTRE_FORCE_INLINE constexpr auto & get() noexcept {
		return (Content::select_by_id(id_tag<Id>{}) || ...);
	}
	
	template <CTLL_FIXED_STRING Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr const auto & get() const noexcept {
		#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
		constexpr auto name = Name;
		constexpr auto tag = name_tag<name>{};
	
		using result_type = decltype((Content::select_by_name(tag) || ...));
	
		static_assert(not std::is_same_v<result_type, ignore_result_tag>);

		return (Content::select_by_name(tag) || ...);
		#else
		return (Content::template select_by_name<Name>() || ...);
		#endif
	}
	
	template <CTLL_FIXED_STRING Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() noexcept {
		#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
		constexpr auto name = Name;
		constexpr auto tag = name_tag<name>{};
		
		using result_type = decltype((Content::select_by_name(tag) || ...));
		
		static_assert(not std::is_same_v<result_type, ignore_result_tag>);

		return (Content::select_by_name(tag) || ...);
		#else
		return (Content::template select_by_name<Name>() || ...);
		#endif
	}
	
	template <typename Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr const auto & get() const noexcept {
		return (Content::select_by_type(Name{}) || ...);
	}

	template <typename Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() noexcept {
		return (Content::select_by_type(Name{}) || ...);
	}
};


template <typename Iterator, typename... Captures> class regex_results {
	captures<
		captured_content<0>::template storage<Iterator>, 
		typename Captures::template storage<Iterator>...
	> _captures{};
public:
	using char_type = typename std::iterator_traits<Iterator>::value_type;
	
	constexpr CTRE_FORCE_INLINE regex_results() noexcept { }
	constexpr CTRE_FORCE_INLINE regex_results(not_matched_tag_t) noexcept { }
	
	// special constructor for deducting
	constexpr CTRE_FORCE_INLINE regex_results(Iterator, ctll::list<Captures...>) noexcept { }
	
	template <size_t Id> CTRE_FORCE_INLINE static constexpr bool exists() noexcept {
		return decltype(_captures)::template exists<Id>();
	}
	
	template <CTLL_FIXED_STRING Name> CTRE_FORCE_INLINE static constexpr auto exists() noexcept {
		return decltype(_captures)::template exists<Name>();
	}
	
	template <typename Name> CTRE_FORCE_INLINE static constexpr auto exists() noexcept {
		return decltype(_captures)::template exists<Name>();
	}
	
	template <size_t Id, typename = std::enable_if_t<exists<Id>()>> CTRE_FORCE_INLINE constexpr const auto & get() const noexcept {
		return _captures.template get<Id>();
	}
	
	template <size_t Id, typename = std::enable_if_t<exists<Id>()>> CTRE_FORCE_INLINE constexpr auto & get() noexcept {
		return _captures.template get<Id>();
	}
	
	template <CTLL_FIXED_STRING Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr const auto & get() const noexcept {
		return _captures.template get<Name>();
	}
	
	template <CTLL_FIXED_STRING Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() noexcept {
		return _captures.template get<Name>();
	}
	
	template <typename Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr const auto & get() const noexcept {
		return _captures.template get<Name>();
	}

	template <typename Name, typename = std::enable_if_t<exists<Name>()>> CTRE_FORCE_INLINE constexpr auto get() noexcept {
		return _captures.template get<Name>();
	}
	
	static constexpr size_t size() noexcept {
		return sizeof...(Captures) + 1;
	}
	
	constexpr CTRE_FORCE_INLINE regex_results & matched() noexcept {
		_captures.template get<0>().matched();
		return *this;
	}
	
	constexpr CTRE_FORCE_INLINE regex_results & unmatch() noexcept {
		_captures.template get<0>().unmatch();
		return *this;
	}
	
	constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
		return bool(_captures.template get<0>());
	}
	
	constexpr CTRE_FORCE_INLINE operator std::basic_string_view<char_type>() const noexcept {
		return to_view();
	}
	
	constexpr CTRE_FORCE_INLINE explicit operator std::basic_string<char_type>() const noexcept {
		return to_string();
	}
	
	constexpr CTRE_FORCE_INLINE auto to_view() const noexcept {
		return _captures.template get<0>().to_view();
	}
	
	constexpr CTRE_FORCE_INLINE auto to_string() const noexcept {
		return _captures.template get<0>().to_string();
	}
	
	constexpr CTRE_FORCE_INLINE auto view() const noexcept {
		return _captures.template get<0>().view();
	}
	
	constexpr CTRE_FORCE_INLINE auto str() const noexcept {
		return _captures.template get<0>().to_string();
	}
	
	constexpr CTRE_FORCE_INLINE regex_results & set_start_mark(Iterator pos) noexcept {
		_captures.template get<0>().set_start(pos);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE regex_results & set_end_mark(Iterator pos) noexcept {
		_captures.template get<0>().set_end(pos);
		return *this;
	}
	constexpr CTRE_FORCE_INLINE Iterator get_end_position() const noexcept {
		return _captures.template get<0>().get_end();
	}
	template <size_t Id> CTRE_FORCE_INLINE constexpr regex_results & start_capture(Iterator pos) noexcept {
		_captures.template get<Id>().set_start(pos);
		return *this;
	}
	template <size_t Id> CTRE_FORCE_INLINE constexpr regex_results & end_capture(Iterator pos) noexcept {
		_captures.template get<Id>().set_end(pos).matched();
		return *this;
	}
};

template <typename Iterator, typename... Captures> regex_results(Iterator, ctll::list<Captures...>) -> regex_results<Iterator, Captures...>;

}

// support for structured bindings

#ifndef __EDG__
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

namespace std {
	template <typename... Captures> struct tuple_size<ctre::regex_results<Captures...>> : public std::integral_constant<size_t, ctre::regex_results<Captures...>::size()> { };
	
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
