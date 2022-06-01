#ifndef CTRE__ROTATE__HPP
#define CTRE__ROTATE__HPP

#include "atoms.hpp"
#include "atoms_characters.hpp"
#include "atoms_unicode.hpp"
#include <ctll/list.hpp>

namespace ctre {

// helper functions
template <size_t Index, typename... Content> auto convert_to_capture(ctll::list<Content...>) -> capture<Index, Content...>;
template <size_t Index, typename Name, typename... Content> auto convert_to_named_capture(ctll::list<Content...>) -> capture_with_name<Index, Name, Content...>;
template <template <size_t, size_t, typename...> typename CycleType, size_t A, size_t B, typename... Content> auto convert_to_repeat(ctll::list<Content...>) -> CycleType<A, B, Content...>;
template <template <typename...> typename ListType, typename... Content> auto convert_to_basic_list(ctll::list<Content...>) -> ListType<Content...>;

template <auto V> struct rotate_value {
	template <auto... Vs> friend constexpr auto operator+(string<Vs...>, rotate_value<V>) noexcept -> string<V, Vs...> { return {}; }
};

// from atoms_characters.hpp
template <auto V> auto rotate(character<V>) -> character<V>;
template <typename... Content> auto rotate(negative_set<Content...>) -> negative_set<Content...>;
template <typename... Content> auto rotate(set<Content...>) -> set<Content...>;
template <auto... Cs> auto rotate(enumeration<Cs...>) -> enumeration<Cs...>;
template <typename... Content> auto rotate(negate<Content...>) -> negate<Content...>;
template <auto A, auto B> auto rotate(char_range<A,B>) -> char_range<A,B>;

// from atoms_unicode.hpp
template <auto... Str> auto rotate(property_name<Str...>) -> property_name<Str...>;
template <auto... Str> auto rotate(property_value<Str...>) -> property_value<Str...>;
template <typename T, T Type> auto rotate(binary_property<T, Type>) -> binary_property<T, Type>;
template <typename T, T Type, auto Value> auto rotate(property<T, Type, Value>) -> property<T, Type, Value>;

// from atoms.hpp
auto rotate(accept) -> accept;
auto rotate(reject) -> reject;
auto rotate(start_mark) -> start_mark;
auto rotate(end_mark) -> end_mark;
auto rotate(end_cycle_mark) -> end_cycle_mark;
auto rotate(end_lookahead_mark) -> end_lookahead_mark;
auto rotate(end_lookbehind_mark) -> end_lookbehind_mark;
template <size_t Id> auto rotate(numeric_mark<Id>) -> numeric_mark<Id>;
auto rotate(any) -> any;

template <typename... Content> auto rotate(select<Content...>) -> select<Content...>;
auto rotate(empty) -> empty;


template <size_t a, size_t b, typename... Content> auto rotate(repeat<a,b,Content...>) -> decltype(ctre::convert_to_repeat<repeat, a, b>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));
template <size_t a, size_t b, typename... Content> auto rotate(lazy_repeat<a,b,Content...>) -> decltype(ctre::convert_to_repeat<lazy_repeat, a, b>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));
template <size_t a, size_t b, typename... Content> auto rotate(possessive_repeat<a,b,Content...>) -> decltype(ctre::convert_to_repeat<possessive_repeat, a, b>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));

template <size_t Index, typename... Content> auto rotate(capture<Index, Content...>) -> decltype(ctre::convert_to_capture<Index>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));
template <size_t Index, typename Name, typename... Content> auto rotate(capture_with_name<Index, Name, Content...>) -> decltype(ctre::convert_to_named_capture<Index, Name>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));

template <size_t Index> auto rotate(back_reference<Index>) -> back_reference<Index>;
template <typename Name> auto rotate(back_reference_with_name<Name>) -> back_reference_with_name<Name>;

template <typename... Content> auto rotate(look_start<Content...>) -> look_start<Content...>;

template <auto... Str> constexpr auto rotate(string<Str...>) -> decltype((string<>{} + ... + rotate_value<Str>{}));

template <typename... Content> auto rotate(sequence<Content...>) -> decltype(ctre::convert_to_basic_list<sequence>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));

// we don't rotate lookaheads
template <typename... Content> auto rotate(lookahead_positive<Content...>) -> lookahead_positive<Content...>;
template <typename... Content> auto rotate(lookahead_negative<Content...>) -> lookahead_negative<Content...>;
template <typename... Content> auto rotate(lookbehind_positive<Content...>) -> lookbehind_positive<Content...>;
template <typename... Content> auto rotate(lookbehind_negative<Content...>) -> lookbehind_negative<Content...>;

auto rotate(atomic_start) -> atomic_start;

template <typename... Content> auto rotate(atomic_group<Content...>) -> decltype(ctre::convert_to_basic_list<atomic_group>(ctll::rotate(ctll::list<decltype(rotate(Content{}))...>{})));

template <typename... Content> auto rotate(boundary<Content...>) -> boundary<Content...>;
template <typename... Content> auto rotate(not_boundary<Content...>) -> not_boundary<Content...>;

auto rotate(assert_subject_begin) -> assert_subject_begin;
auto rotate(assert_subject_end) -> assert_subject_end;
auto rotate(assert_subject_end_line) -> assert_subject_end_line;
auto rotate(assert_line_begin) -> assert_line_begin;
auto rotate(assert_line_end) -> assert_line_end;
 

}

#endif
