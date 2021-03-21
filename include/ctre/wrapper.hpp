#ifndef CTRE__WRAPPER__HPP
#define CTRE__WRAPPER__HPP

#include "evaluation.hpp"
#include "utility.hpp"
#include "utf8.hpp"
#include "return_type.hpp"
#include "range.hpp"
#include <string_view>

namespace ctre {

template <typename RE, typename Method = void, typename Modifier = singleline> struct regular_expression;
	
struct zero_terminated_string_end_iterator {
	constexpr inline zero_terminated_string_end_iterator() = default;
	constexpr CTRE_FORCE_INLINE friend bool operator==(const char * ptr, zero_terminated_string_end_iterator) noexcept {
		return *ptr == '\0';
	} 
	constexpr CTRE_FORCE_INLINE friend bool operator==(const wchar_t * ptr, zero_terminated_string_end_iterator) noexcept {
		return *ptr == 0;
	} 
	constexpr CTRE_FORCE_INLINE friend bool operator!=(const char * ptr, zero_terminated_string_end_iterator) noexcept {
		return *ptr != '\0';
	} 
	constexpr CTRE_FORCE_INLINE friend bool operator!=(const wchar_t * ptr, zero_terminated_string_end_iterator) noexcept {
		return *ptr != 0;
	} 
	constexpr CTRE_FORCE_INLINE friend bool operator==(zero_terminated_string_end_iterator, const char * ptr) noexcept {
		return *ptr == '\0';
	} 
	constexpr CTRE_FORCE_INLINE friend bool operator==(zero_terminated_string_end_iterator, const wchar_t * ptr) noexcept {
		return *ptr == 0;
	} 
	constexpr CTRE_FORCE_INLINE friend bool operator!=(zero_terminated_string_end_iterator, const char * ptr) noexcept {
		return *ptr != '\0';
	} 
	constexpr CTRE_FORCE_INLINE friend bool operator!=(zero_terminated_string_end_iterator, const wchar_t * ptr) noexcept {
		return *ptr != 0;
	} 
};

template <typename T> class RangeLikeType {
	template <typename Y> static auto test(Y *) -> decltype(std::declval<const Y &>().begin(), std::declval<const Y &>().end(), std::true_type());
	template <typename> static auto test(...) -> std::false_type;
public:
	static inline constexpr bool value = decltype(test<std::remove_reference_t<std::remove_const_t<T>>>( nullptr ))::value;
};

struct match_method {
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin orig_begin, IteratorBegin begin, IteratorEnd end, RE) noexcept {
		using result_iterator = std::conditional_t<std::is_same_v<ResultIterator, void>, IteratorBegin, ResultIterator>;
		
		return evaluate(orig_begin, begin, end, Modifier{}, return_type<result_iterator, RE>{}, ctll::list<start_mark, RE, assert_subject_end, end_mark, accept>());
	}
	
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end, RE) noexcept {
		return exec<Modifier, ResultIterator>(begin, begin, end, RE{});
	}
};

struct search_method {
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin orig_begin, IteratorBegin begin, IteratorEnd end, RE) noexcept {
		using result_iterator = std::conditional_t<std::is_same_v<ResultIterator, void>, IteratorBegin, ResultIterator>;
		using front_re = decltype(pop_and_get_front(extract_leading_string(ctll::list<RE>{})));
		constexpr bool fixed = starts_with_anchor(Modifier{}, ctll::list<RE>{});
	
		auto it = begin;
		if constexpr (is_string(front_re{}.front) && size(front_re{}.list)) {
			auto it2 = search_for_string(it, end, front_re{}.front);
			return_type<result_iterator, RE> result{};
			for (; end != it2.position;) {
				result.set_start_mark(it2.position);
				result = evaluate(orig_begin, it2.end_position, end, Modifier{}, result, ctll::list<start_mark, decltype(make_into_sequence(front_re{}.list)), end_mark, accept>());
				if (result) {
					return result;
				}
				result.unmatch();
				std::advance(it2.position, 1);
				it2 = search_for_string(it2.position, end, front_re{}.front);
			}
			result.set_start_mark(it2.position);
			return result = evaluate(orig_begin, it2.end_position, end, Modifier{}, result, ctll::list<start_mark, decltype(make_into_sequence(front_re{}.list)), end_mark, accept>());
		} else if constexpr (is_string(front_re{}.front)) {
			auto it2 = search_for_string(it, end, front_re{}.front);
			return_type<result_iterator, RE> result{};
			result.set_start_mark(it2.position);
			result.set_end_mark(it2.end_position);
			if (it2.match)
				result.matched();
			return result;
		} else {
			for (; end != it && !fixed; ++it) {
				if (auto out = evaluate(orig_begin, it, end, Modifier{}, return_type<result_iterator, RE>{}, ctll::list<start_mark, RE, end_mark, accept>())) {
					return out;
				}
			}
			// in case the RE is empty or fixed
			return evaluate(orig_begin, it, end, Modifier{}, return_type<result_iterator, RE>{}, ctll::list<start_mark, RE, end_mark, accept>());
		}
	}
	
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end, RE) noexcept {
		return exec<Modifier, ResultIterator>(begin, begin, end, RE{});
	}
};

struct starts_with_method {
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin orig_begin, IteratorBegin begin, IteratorEnd end, RE) noexcept {
		using result_iterator = std::conditional_t<std::is_same_v<ResultIterator, void>, IteratorBegin, ResultIterator>;
		return evaluate(orig_begin, begin, end, Modifier{}, return_type<result_iterator, RE>{}, ctll::list<start_mark, RE, end_mark, accept>());
	}
	
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end, RE) noexcept {
		return exec<Modifier, ResultIterator>(begin, begin, end, RE{});
	}
};

// wrapper which calls search on input
struct range_method {
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end, RE) noexcept {
		using result_iterator = std::conditional_t<std::is_same_v<ResultIterator, void>, IteratorBegin, ResultIterator>;
		using wrapped_regex = regular_expression<RE, search_method, Modifier>;
	
		return regex_range<IteratorBegin, IteratorEnd, wrapped_regex, result_iterator>(begin, end);
	}
};

struct tokenize_method {
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end, RE) noexcept {
		using result_iterator = std::conditional_t<std::is_same_v<ResultIterator, void>, IteratorBegin, ResultIterator>;
		using wrapped_regex = regular_expression<RE, starts_with_method, Modifier>;
	
		return regex_range<IteratorBegin, IteratorEnd, wrapped_regex, result_iterator>(begin, end);
	}
};

struct iterator_method {
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end, RE) noexcept {
		using result_iterator = std::conditional_t<std::is_same_v<ResultIterator, void>, IteratorBegin, ResultIterator>;
		using wrapped_regex = regular_expression<RE, search_method, Modifier>;
	
		return regex_iterator<IteratorBegin, IteratorEnd, wrapped_regex, result_iterator>(begin, end);
	}
	constexpr CTRE_FORCE_INLINE static auto exec() noexcept {
		return regex_end_iterator{};
	}
};

template <typename RE, typename Method, typename Modifier> struct regular_expression {
	constexpr CTRE_FORCE_INLINE regular_expression() noexcept { }
	constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept { }
	
	template <typename ResultIterator, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec_with_result_iterator(IteratorBegin orig_begin, IteratorBegin begin, IteratorEnd end) noexcept {
		return Method::template exec<Modifier, ResultIterator>(orig_begin, begin, end, RE{});
	}
	template <typename ResultIterator, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec_with_result_iterator(IteratorBegin begin, IteratorEnd end) noexcept {
		return Method::template exec<Modifier, ResultIterator>(begin, end, RE{});
	}
	constexpr CTRE_FORCE_INLINE static auto exec() noexcept {
		return Method::template exec();
	}
	template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end) noexcept {
		return Method::template exec<Modifier>(begin, end, RE{});
	}
	static constexpr CTRE_FORCE_INLINE auto exec(const char * s) noexcept {
		return Method::template exec<Modifier>(s, zero_terminated_string_end_iterator(), RE{});
	}
	static constexpr CTRE_FORCE_INLINE auto exec(const wchar_t * s) noexcept {
		return Method::template exec<Modifier>(s, zero_terminated_string_end_iterator(), RE{});
	}
	static constexpr CTRE_FORCE_INLINE auto exec(std::string_view sv) noexcept {
		return exec(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto exec(std::wstring_view sv) noexcept {
		return exec(sv.begin(), sv.end());
	}
#if __cpp_char8_t >= 201811
	static constexpr CTRE_FORCE_INLINE auto exec(std::u8string_view sv) noexcept {
		return exec_with_result_iterator<const char8_t *>(utf8_range(sv).begin(), utf8_range(sv).end());
	}
#endif
	static constexpr CTRE_FORCE_INLINE auto exec(std::u16string_view sv) noexcept {
		return exec(sv.begin(), sv.end());
	}
	static constexpr CTRE_FORCE_INLINE auto exec(std::u32string_view sv) noexcept {
		return exec(sv.begin(), sv.end());
	}
	template <typename Range, typename = typename std::enable_if<RangeLikeType<Range>::value>::type> static constexpr CTRE_FORCE_INLINE auto exec(Range && range) noexcept {
		return exec(std::begin(range), std::end(range));
	}
	
	// another api
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto operator()(Args && ... args) const noexcept {
		return exec(std::forward<Args>(args)...);
	}
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto try_extract(Args && ... args) const noexcept {
		return exec(std::forward<Args>(args)...);
	}
	
	// for compatibility with _ctre literal
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto match(Args && ... args) noexcept {
		return regular_expression<RE, match_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto search(Args && ... args) noexcept {
		return regular_expression<RE, search_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto starts_with(Args && ... args) noexcept {
		return regular_expression<RE, starts_with_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto range(Args && ... args) noexcept {
		return regular_expression<RE, range_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto tokenize(Args && ... args) noexcept {
		return regular_expression<RE, tokenize_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto iterator(Args && ... args) noexcept {
		return regular_expression<RE, iterator_method, singleline>::exec(std::forward<Args>(args)...);
	}
	
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_match(Args && ... args) noexcept {
		return regular_expression<RE, match_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_search(Args && ... args) noexcept {
		return regular_expression<RE, search_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_starts_with(Args && ... args) noexcept {
		return regular_expression<RE, starts_with_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_range(Args && ... args) noexcept {
		return regular_expression<RE, range_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_tokenize(Args && ... args) noexcept {
		return regular_expression<RE, tokenize_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_iterator(Args && ... args) noexcept {
		return regular_expression<RE, iterator_method, multiline>::exec(std::forward<Args>(args)...);
	}
};


#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
#define CTRE_REGEX_INPUT_TYPE ctll::fixed_string
template <auto input> struct regex_builder {
	static constexpr auto _input = input;
	using _tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(_tmp(), "Regular Expression contains syntax error.");
	using type = ctll::conditional<(bool)(_tmp()), decltype(ctll::front(typename _tmp::output_type::stack_type())), ctll::list<reject>>;
};
#else
#define CTRE_REGEX_INPUT_TYPE const auto &
template <const auto & input> struct regex_builder {
	using _tmp = typename ctll::parser<ctre::pcre, input, ctre::pcre_actions>::template output<pcre_context<>>;
	static_assert(_tmp(), "Regular Expression contains syntax error.");
	using type = ctll::conditional<(bool)(_tmp()), decltype(ctll::front(typename _tmp::output_type::stack_type())), ctll::list<reject>>;
};
#endif

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto match = regular_expression<typename regex_builder<input>::type, match_method, singleline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto search = regular_expression<typename regex_builder<input>::type, search_method, singleline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto starts_with = regular_expression<typename regex_builder<input>::type, starts_with_method, singleline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto range = regular_expression<typename regex_builder<input>::type, range_method, singleline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto tokenize = regular_expression<typename regex_builder<input>::type, tokenize_method, singleline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto iterator = regular_expression<typename regex_builder<input>::type, iterator_method, singleline>();

static constexpr inline auto sentinel = regex_end_iterator();


template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto multiline_match = regular_expression<typename regex_builder<input>::type, match_method, multiline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto multiline_search = regular_expression<typename regex_builder<input>::type, search_method, multiline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto multiline_starts_with = regular_expression<typename regex_builder<input>::type, starts_with_method, multiline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto multiline_range = regular_expression<typename regex_builder<input>::type, range_method, multiline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto multiline_tokenize = regular_expression<typename regex_builder<input>::type, tokenize_method, multiline>();

template <CTRE_REGEX_INPUT_TYPE input> static constexpr inline auto multiline_iterator = regular_expression<typename regex_builder<input>::type, iterator_method, multiline>();

static constexpr inline auto multiline_sentinel = regex_end_iterator();


}

#endif
