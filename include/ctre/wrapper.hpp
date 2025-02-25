#ifndef CTRE__WRAPPER__HPP
#define CTRE__WRAPPER__HPP

#include "evaluation.hpp"
#include "range.hpp"
#include "return_type.hpp"
#include "utf8.hpp"
#include "utility.hpp"
#ifndef CTRE_IN_A_MODULE
#include <string_view>
#endif

namespace ctre {

CTRE_EXPORT template <typename RE, typename Method = void, typename Modifier = singleline> struct regular_expression;

struct zero_terminated_string_end_iterator {
	// this is here only because I want to support std::make_reverse_iterator
	using self_type = zero_terminated_string_end_iterator;
	using value_type = char;
	using reference = char &;
	using pointer = const char *;
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = int;

	// it's just sentinel it won't be ever called
	auto operator++() noexcept -> self_type &;
	auto operator++(int) noexcept -> self_type;
	auto operator--() noexcept -> self_type &;
	auto operator--(int) noexcept -> self_type;
	friend auto operator==(self_type, self_type) noexcept -> bool;
	auto operator*() noexcept -> reference;

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
	static constexpr bool value = decltype(test<std::remove_reference_t<std::remove_const_t<T>>>(nullptr))::value;
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

		constexpr bool fixed = starts_with_anchor(Modifier{}, ctll::list<RE>{});

		auto it = begin;

		for (; end != it && !fixed; ++it) {
			if (auto out = evaluate(orig_begin, it, end, Modifier{}, return_type<result_iterator, RE>{}, ctll::list<start_mark, RE, end_mark, accept>())) {
				return out;
			}
		}

		// in case the RE is empty or fixed
		auto out = evaluate(orig_begin, it, end, Modifier{}, return_type<result_iterator, RE>{}, ctll::list<start_mark, RE, end_mark, accept>());

		// ALERT: ugly hack
		// propagate end even if it didn't match (this is needed for split function)
		if (!out) out.set_end_mark(it);
		return out;
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

struct split_method {
	template <typename Modifier = singleline, typename ResultIterator = void, typename RE, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec(IteratorBegin begin, IteratorEnd end, RE) noexcept {
		using result_iterator = std::conditional_t<std::is_same_v<ResultIterator, void>, IteratorBegin, ResultIterator>;
		using wrapped_regex = regular_expression<RE, search_method, Modifier>;

		return regex_split_range<IteratorBegin, IteratorEnd, wrapped_regex, result_iterator>(begin, end);
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

CTRE_EXPORT template <typename RE, typename Method, typename Modifier> struct regular_expression {
	constexpr CTRE_FORCE_INLINE regular_expression() noexcept { }
	constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept { }

	template <typename ResultIterator, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec_with_result_iterator(IteratorBegin orig_begin, IteratorBegin begin, IteratorEnd end) noexcept {
		return Method::template exec<Modifier, ResultIterator>(orig_begin, begin, end, RE{});
	}
	template <typename ResultIterator, typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto exec_with_result_iterator(IteratorBegin begin, IteratorEnd end) noexcept {
		return Method::template exec<Modifier, ResultIterator>(begin, end, RE{});
	}
	template <typename Range> constexpr CTRE_FORCE_INLINE static auto multi_exec(Range && range) noexcept {
		return multi_subject_range<Range, regular_expression>{std::forward<Range>(range)};
	}
	constexpr CTRE_FORCE_INLINE static auto exec() noexcept {
		return Method::exec();
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
#ifdef CTRE_ENABLE_UTF8_RANGE
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
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto operator()(Args &&... args) const noexcept {
		return exec(std::forward<Args>(args)...);
	}
	// api for pattern matching
	template <typename... Args> CTRE_FORCE_INLINE constexpr auto try_extract(Args &&... args) const noexcept {
		return exec(std::forward<Args>(args)...);
	}

	// for compatibility with _ctre literal
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto match(Args &&... args) noexcept {
		return regular_expression<RE, match_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto search(Args &&... args) noexcept {
		return regular_expression<RE, search_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto starts_with(Args &&... args) noexcept {
		return regular_expression<RE, starts_with_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto range(Args &&... args) noexcept {
		return regular_expression<RE, range_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto split(Args &&... args) noexcept {
		return regular_expression<RE, split_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto tokenize(Args &&... args) noexcept {
		return regular_expression<RE, tokenize_method, singleline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto iterator(Args &&... args) noexcept {
		return regular_expression<RE, iterator_method, singleline>::exec(std::forward<Args>(args)...);
	}

	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_match(Args &&... args) noexcept {
		return regular_expression<RE, match_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_search(Args &&... args) noexcept {
		return regular_expression<RE, search_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_starts_with(Args &&... args) noexcept {
		return regular_expression<RE, starts_with_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_range(Args &&... args) noexcept {
		return regular_expression<RE, range_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_split(Args &&... args) noexcept {
		return regular_expression<RE, split_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_tokenize(Args &&... args) noexcept {
		return regular_expression<RE, tokenize_method, multiline>::exec(std::forward<Args>(args)...);
	}
	template <typename... Args> static constexpr CTRE_FORCE_INLINE auto multiline_iterator(Args &&... args) noexcept {
		return regular_expression<RE, iterator_method, multiline>::exec(std::forward<Args>(args)...);
	}
};

// range style API support for tokenize/range/split operations
template <typename Range, typename RE, typename Modifier> constexpr auto operator|(Range && range, regular_expression<RE, range_method, Modifier> re) noexcept {
	return re.exec(std::forward<Range>(range));
}

template <typename Range, typename RE, typename Modifier> constexpr auto operator|(Range && range, regular_expression<RE, tokenize_method, Modifier> re) noexcept {
	return re.exec(std::forward<Range>(range));
}

template <typename Range, typename RE, typename Modifier> constexpr auto operator|(Range && range, regular_expression<RE, split_method, Modifier> re) noexcept {
	return re.exec(std::forward<Range>(range));
}

template <typename Range, typename RE, typename Modifier> constexpr auto operator|(Range && range, regular_expression<RE, iterator_method, Modifier> re) noexcept = delete;

template <typename Range, typename RE, typename Method, typename Modifier> constexpr auto operator|(Range && range, regular_expression<RE, Method, Modifier> re) noexcept {
	return re.multi_exec(std::forward<Range>(range));
}

// error reporting of problematic position in a regex
template <size_t> struct problem_at_position; // do not define!

template <> struct problem_at_position<~static_cast<size_t>(0)> {
	constexpr operator bool() const noexcept {
		return true;
	}
};

#if CTRE_CNTTP_COMPILER_CHECK
#define CTRE_REGEX_INPUT_TYPE ctll::fixed_string
#define CTRE_REGEX_TEMPLATE_COPY_TYPE auto
#else
#define CTRE_REGEX_INPUT_TYPE const auto &
#define CTRE_REGEX_TEMPLATE_COPY_TYPE const auto &
#endif

template <CTRE_REGEX_TEMPLATE_COPY_TYPE input> struct regex_builder {
	static constexpr auto _input = input;
	using result = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<pcre_context<>>;

	static constexpr auto n = result::is_correct ? ~static_cast<size_t>(0) : result::position;

	static_assert(result::is_correct && problem_at_position<n>{}, "Regular Expression contains syntax error.");

	using type = ctll::conditional<result::is_correct, decltype(ctll::front(typename result::output_type::stack_type())), ctll::list<reject>>;
};

// case-sensitive

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto match = regular_expression<typename regex_builder<input>::type, match_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto search = regular_expression<typename regex_builder<input>::type, search_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto starts_with = regular_expression<typename regex_builder<input>::type, starts_with_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto search_all = regular_expression<typename regex_builder<input>::type, range_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> [[deprecated("use search_all")]] constexpr auto range = regular_expression<typename regex_builder<input>::type, range_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto split = regular_expression<typename regex_builder<input>::type, split_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto tokenize = regular_expression<typename regex_builder<input>::type, tokenize_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto iterator = regular_expression<typename regex_builder<input>::type, iterator_method, ctll::list<singleline, Modifiers...>>();

CTRE_EXPORT constexpr auto sentinel = regex_end_iterator();

// multiline

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto multiline_match = regular_expression<typename regex_builder<input>::type, match_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto multiline_search = regular_expression<typename regex_builder<input>::type, search_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto multiline_starts_with = regular_expression<typename regex_builder<input>::type, starts_with_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto multiline_search_all = regular_expression<typename regex_builder<input>::type, range_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> [[deprecated("use multiline_search_all")]] constexpr auto multiline_range = regular_expression<typename regex_builder<input>::type, range_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto multiline_split = regular_expression<typename regex_builder<input>::type, split_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto multiline_tokenize = regular_expression<typename regex_builder<input>::type, tokenize_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT template <CTRE_REGEX_INPUT_TYPE input, typename... Modifiers> constexpr auto multiline_iterator = regular_expression<typename regex_builder<input>::type, iterator_method, ctll::list<multiline, Modifiers...>>();

CTRE_EXPORT constexpr auto multiline_sentinel = regex_end_iterator();

} // namespace ctre


#endif
