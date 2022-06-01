#ifndef CTRE__ROTATE__HPP
#define CTRE__ROTATE__HPP

#include "atoms.hpp"
#include "atoms_characters.hpp"
#include "atoms_unicode.hpp"

namespace ctre {

// helper functions
template <size_t Index, typename... Content> auto convert_to_capture(ctll::list<Content...>) -> capture<Index, Content...>;
template <size_t Index, typename Name, typename... Content> auto convert_to_named_capture(ctll::list<Content...>) -> capture_with_name<Index, Name, Content...>;
template <template <size_t, size_t, typename...> typename CycleType, size_t A, size_t B, typename... Content> auto convert_to_repeat(ctll::list<Content...>) -> CycleType<A, B, Content...>;
template <template <typename...> typename ListType, typename... Content> auto convert_to_basic_list(ctll::list<Content...>) -> ListType<Content...>;

template <auto V> struct rotate_value {
	template <auto... Vs> friend constexpr auto operator+(string<Vs...>, rotate_value<V>) noexcept -> string<V, Vs...> { return {}; }
};

struct rotate_for_lookbehind {

// from atoms_characters.hpp
template <auto V> static auto rotate(character<V>) -> character<V>;
template <typename... Content> static auto rotate(negative_set<Content...>) -> negative_set<Content...>;
template <typename... Content> static auto rotate(set<Content...>) -> set<Content...>;
template <auto... Cs> static auto rotate(enumeration<Cs...>) -> enumeration<Cs...>;
template <typename... Content> static auto rotate(negate<Content...>) -> negate<Content...>;
template <auto A, auto B> static auto rotate(char_range<A,B>) -> char_range<A,B>;

// from atoms_unicode.hpp
template <auto... Str> static auto rotate(property_name<Str...>) -> property_name<Str...>;
template <auto... Str> static auto rotate(property_value<Str...>) -> property_value<Str...>;
template <typename T, T Type> static auto rotate(binary_property<T, Type>) -> binary_property<T, Type>;
template <typename T, T Type, auto Value> static auto rotate(property<T, Type, Value>) -> property<T, Type, Value>;

// from atoms.hpp
static auto rotate(accept) -> accept;
static auto rotate(reject) -> reject;
static auto rotate(start_mark) -> start_mark;
static auto rotate(end_mark) -> end_mark;
static auto rotate(end_cycle_mark) -> end_cycle_mark;
static auto rotate(end_lookahead_mark) -> end_lookahead_mark;
static auto rotate(end_lookbehind_mark) -> end_lookbehind_mark;
template <size_t Id> static auto rotate(numeric_mark<Id>) -> numeric_mark<Id>;
static auto rotate(any) -> any;

template <typename... Content> static auto rotate(select<Content...>) -> select<Content...>;
static auto rotate(empty) -> empty;


template <size_t a, size_t b, typename... Content> static auto rotate(repeat<a,b,Content...>) -> decltype(ctre::convert_to_repeat<repeat, a, b>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));
template <size_t a, size_t b, typename... Content> static auto rotate(lazy_repeat<a,b,Content...>) -> decltype(ctre::convert_to_repeat<lazy_repeat, a, b>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));
template <size_t a, size_t b, typename... Content> static auto rotate(possessive_repeat<a,b,Content...>) -> decltype(ctre::convert_to_repeat<possessive_repeat, a, b>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));

template <size_t Index, typename... Content> static auto rotate(capture<Index, Content...>) {
	return ctre::convert_to_capture<Index>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{}));
}

template <size_t Index, typename Name, typename... Content> static auto rotate(capture_with_name<Index, Name, Content...>) {
	return ctre::convert_to_named_capture<Index, Name>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{}));
}

template <size_t Index> static auto rotate(back_reference<Index>) -> back_reference<Index>;
template <typename Name> static auto rotate(back_reference_with_name<Name>) -> back_reference_with_name<Name>;

template <typename... Content> static auto rotate(look_start<Content...>) -> look_start<Content...>;

template <auto... Str> static auto rotate(string<Str...>) -> decltype((string<>{} + ... + rotate_value<Str>{}));

template <typename... Content> static auto rotate(sequence<Content...>) {
	return ctre::convert_to_basic_list<sequence>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{}));
}

// we don't rotate lookaheads
template <typename... Content> static auto rotate(lookahead_positive<Content...>) -> lookahead_positive<Content...>;
template <typename... Content> static auto rotate(lookahead_negative<Content...>) -> lookahead_negative<Content...>;
template <typename... Content> static auto rotate(lookbehind_positive<Content...>) -> lookbehind_positive<Content...>;
template <typename... Content> static auto rotate(lookbehind_negative<Content...>) -> lookbehind_negative<Content...>;

static auto rotate(atomic_start) -> atomic_start;

template <typename... Content> static auto rotate(atomic_group<Content...>) {
	return ctre::convert_to_basic_list<atomic_group>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{}));
}

template <typename... Content> static auto rotate(boundary<Content...>) -> boundary<Content...>;
template <typename... Content> static auto rotate(not_boundary<Content...>) -> not_boundary<Content...>;

static auto rotate(assert_subject_begin) -> assert_subject_begin;
static auto rotate(assert_subject_end) -> assert_subject_end;
static auto rotate(assert_subject_end_line) -> assert_subject_end_line;
static auto rotate(assert_line_begin) -> assert_line_begin;
static auto rotate(assert_line_end) -> assert_line_end;

};

}

#endif
