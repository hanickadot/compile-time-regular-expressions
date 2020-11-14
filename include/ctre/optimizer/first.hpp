#ifndef CTRE__FIRST__HPP
#define CTRE__FIRST__HPP

#include "../atoms_characters.hpp"
#include "../atoms_unicode.hpp"
#include "../atoms.hpp"

namespace ctre {

struct first_lookahead_result {
	bool matches{false};
	
	constexpr CTRE_FORCE_INLINE first_lookahead_result() noexcept { }
	constexpr CTRE_FORCE_INLINE first_lookahead_result(not_matched_tag_t) noexcept { }
	
	constexpr CTRE_FORCE_INLINE auto matched() {
		matches = true;
		return *this;
	}
	constexpr CTRE_FORCE_INLINE operator bool() const noexcept {
		return matches;
	}
};

template <typename... Items> constexpr auto first_set_of(Items...);

struct opaque {
	static constexpr bool we_can_calculate_intersect = false;
};

template <typename...> struct first {
	unsigned count = 1;
	static constexpr bool we_can_calculate_intersect = true;
};

template <typename...> struct complex_first {
	static constexpr bool we_can_calculate_intersect = false;
};

template <typename... Lhs, typename... Rhs> constexpr auto operator+(first<Lhs...> l, first<Rhs...> r) -> first<Lhs..., Rhs...> {
	return {l.count + r.count};
}

template <typename... Lhs, typename... Rhs> constexpr auto operator+(first<Lhs...>, complex_first<Rhs...>) -> complex_first<Lhs..., Rhs...> {
	return {};
}

template <typename... Lhs, typename... Rhs> constexpr auto operator+(complex_first<Lhs...>, first<Rhs...>) -> complex_first<Lhs..., Rhs...> {
	return {};
}

template <typename... Lhs, typename... Rhs> constexpr auto operator+(complex_first<Lhs...>, complex_first<Rhs...>) -> complex_first<Lhs..., Rhs...> {
	return {};
}

template <typename T> constexpr auto operator+(opaque, T) -> opaque {
	return {};
}

template <typename T> constexpr auto operator+(T, opaque) -> opaque {
	return {};
}

constexpr auto operator+(opaque, opaque) -> opaque {
	return {};
}

template <typename Head, typename... Content> constexpr auto calculate_first(Head, Content...) = delete;
constexpr auto calculate_first() {
	return first<>{0};
}

// IMPLEMENTATION

// fall-through
template <typename... Rest> constexpr auto calculate_first(accept, Rest... rest) { return calculate_first(rest...); }
template <typename... Rest> constexpr auto calculate_first(reject, Rest... rest) { return calculate_first(rest...); }
template <typename... Rest> constexpr auto calculate_first(start_mark, Rest... rest) { return calculate_first(rest...); }
template <typename... Rest> constexpr auto calculate_first(end_mark, Rest... rest) { return calculate_first(rest...); }
template <typename... Rest> constexpr auto calculate_first(end_cycle_mark, Rest... rest) { return calculate_first(rest...); }
template <typename... Rest> constexpr auto calculate_first(end_lookahead_mark, Rest... rest) { return calculate_first(rest...); }
template <typename... Rest> constexpr auto calculate_first(fail_if_empty, Rest... rest) { return calculate_first(rest...); }
template <size_t Id, typename... Rest> constexpr auto calculate_first(numeric_mark<Id>, Rest... rest) { return calculate_first(rest...); }

// any
// TODO what to do with any in multiline?
template <typename... Rest> constexpr auto calculate_first(any, Rest...) { return first<any>{1}; }

// string
template <auto Head, auto... Tail, typename... Rest> constexpr auto calculate_first(string<Head, Tail...>, Rest...) { return first<character<Head>>{1}; }

template <typename... Rest> constexpr auto calculate_first(string<>, Rest... rest) {
	return calculate_first(rest...);
}

// select
template <typename... Options, typename... Rest> constexpr auto calculate_first(select<Options...>, Rest... rest) {
	return (first<>{0} + ... + calculate_first(Options{}, rest...));
}

// sequence
template <typename... Items, typename... Rest> constexpr auto calculate_first(sequence<Items...>, Rest... rest) {
	return calculate_first(Items{}..., rest...);
}

template <typename... Rest> constexpr auto calculate_first(empty, Rest... rest) {
	return calculate_first(rest...);
}

// cycles
template <size_t A, size_t B, typename... Items, typename... Rest> constexpr auto calculate_first(lazy_repeat<A,B,Items...>, Rest... rest) {
	if constexpr (A == 0) {
		return calculate_first(Items{}..., rest...) + calculate_first(rest...);
	} else {
		return calculate_first(Items{}..., rest...);
	}
}

template <size_t A, size_t B, typename... Items, typename... Rest> constexpr auto calculate_first(repeat<A,B,Items...>, Rest... rest) {
	if constexpr (A == 0) {
		return calculate_first(Items{}..., rest...) + calculate_first(rest...);
	} else {
		return calculate_first(Items{}..., rest...);
	}
}

template <size_t A, size_t B, typename... Items, typename... Rest> constexpr auto calculate_first(possessive_repeat<A,B,Items...>, Rest... rest) {
	if constexpr (A == 0) {
		return calculate_first(Items{}..., rest...) + calculate_first(rest...);
	} else {
		return calculate_first(Items{}..., rest...);
	}
}

// capture
template <size_t Id, typename... Items, typename... Rest> constexpr auto calculate_first(capture<Id, Items...>, Rest... rest) {
	return calculate_first(Items{}..., rest...);
}

template <size_t Id, typename Name, typename... Items, typename... Rest> constexpr auto calculate_first(capture_with_name<Id, Name, Items...>, Rest... rest) {
	return calculate_first(Items{}..., rest...);
}

// back-reference
template <size_t Id, typename... Rest> constexpr auto calculate_first(back_reference<Id>, Rest...) {
	return opaque{};
}
template <typename Id, typename... Rest> constexpr auto calculate_first(back_reference_with_name<Id>, Rest...) {
	return opaque{};
}

// lookahead
template <typename T, typename... Rest> constexpr auto calculate_first(look_start<T>, Rest... rest) {
	return calculate_first(rest...);
}

template <typename... Items, typename... Rest> constexpr auto calculate_first(lookahead_positive<Items...>, Rest... rest) {
	return calculate_first(Items{}...) + calculate_first(rest...);
}

template <typename... Items, typename... Rest> constexpr auto calculate_first(lookahead_negative<Items...>, Rest...) {
	return opaque{};
}

// atomic
template <typename... Rest> constexpr auto calculate_first(atomic_start, Rest... rest) {
	return calculate_first(rest...);
}

template <typename... Items, typename... Rest> constexpr auto calculate_first(atomic_group<Items...>, Rest... rest) {
	return calculate_first(Items{}..., rest...);
}

// boundaries
template <typename T, typename... Rest> constexpr auto calculate_first(boundary<T>, Rest... rest) {
	return first<boundary<T>>{} + calculate_first(rest...);
}
template <typename T, typename... Rest> constexpr auto calculate_first(not_boundary<T>, Rest... rest) {
	return first<not_boundary<T>>{} + calculate_first(rest...);
}

// assert
// TODO what to do with asserts?
template <typename... Rest> constexpr auto calculate_first(assert_subject_begin, Rest...) {
	return first<assert_subject_begin>{0};
}
template <typename... Rest> constexpr auto calculate_first(assert_subject_end, Rest...) {
	return first<assert_subject_end>{0};
}
template <typename... Rest> constexpr auto calculate_first(assert_subject_end_line, Rest...) {
	return first<assert_subject_end_line>{1};
}
template <typename... Rest> constexpr auto calculate_first(assert_line_begin, Rest...) {
	return first<assert_line_begin>{0};
}
template <typename... Rest> constexpr auto calculate_first(assert_line_end, Rest...) {
	return first<assert_line_end>{1};
}


// character

template <auto C, typename... Rest> constexpr auto calculate_first(character<C>, Rest...) {
	return first<character<C>>{1};
}

template <auto A, auto B, typename... Rest> constexpr auto calculate_first(char_range<A,B>, Rest...) {
	return first<char_range<A,B>>{1};
}

template <auto... Cs, typename... Rest> constexpr auto calculate_first(enumeration<Cs...>, Rest...) {
	return first<character<Cs>...>{sizeof...(Cs)};
}

template <auto C> constexpr auto set_item_first(character<C>) { return first<character<C>>{1}; }
template <auto A, auto B> constexpr auto set_item_first(char_range<A,B>) { return first<char_range<A,B>>{1}; }
template <auto... Cs> constexpr auto set_item_first(enumeration<Cs...>) { return first<character<Cs>...>{sizeof...(Cs)}; }

template <typename... Set, typename... Rest> constexpr auto calculate_first(set<Set...>, Rest...) {
	return (first<>{0} + ... + set_item_first(Set{}));
}

// negative ranges are with +1, because it's always two ranges around
template <auto C> constexpr auto nset_item_first(character<C>) { return first<character<C>>{2}; }
template <auto A, auto B> constexpr auto nset_item_first(char_range<A,B>) { return first<char_range<A,B>>{2}; }
template <auto... Cs> constexpr auto nset_item_first(enumeration<Cs...>) { return first<character<Cs>...>{sizeof...(Cs)+1}; }

template <typename... Set, typename... Rest> constexpr auto calculate_first(negative_set<Set...>, Rest...) {
	// TODO look at negative things
	return first<negative_set<Set...>>{(nset_item_first(Set{}).count + ... + 1)};
}

// unicode

template <auto V, typename... Rest> constexpr auto calculate_first(binary_property<V>, Rest...) {
	return complex_first<binary_property<V>>{};
}

template <auto T, auto V, typename... Rest> constexpr auto calculate_first(property<T, V>, Rest...) {
	return complex_first<property<T,V>>{};
}

template <auto V, typename... Rest> constexpr auto calculate_first(negate<binary_property<V>>, Rest...) {
	return complex_first<negate<binary_property<V>>>{};
}

template <auto T, auto V, typename... Rest> constexpr auto calculate_first(negate<property<T, V>>, Rest...) {
	return complex_first<negate<property<T,V>>>{};
}


// USER API

template <typename Lhs, typename Rhs> constexpr auto collides(Lhs, Rhs) {
	if constexpr (Lhs::we_can_calculate_intersect && Rhs::we_can_calculate_intersect) {
		// TODO calculate intersect
	}
	return true;
}

// lookahead

template <typename Iterator, typename EndIterator, typename... Content> constexpr CTRE_FORCE_INLINE bool lookahead_first(const Iterator begin, Iterator current, const EndIterator end, const flags & f, first<Content...>) noexcept {
	return (evaluate_split(begin, current, end, f, first_lookahead_result{}, ctll::list<Content, accept>{}) || ... || false);
}

template <typename Iterator, typename EndIterator, typename... Content> constexpr CTRE_FORCE_INLINE bool lookahead_first(const Iterator begin, Iterator current, const EndIterator end, const flags & f, complex_first<Content...>) noexcept {
	return (evaluate_split(begin, current, end, f, first_lookahead_result{}, ctll::list<Content, accept>{}) || ... || false);
}

template <typename Iterator, typename EndIterator, typename... Content> constexpr CTRE_FORCE_INLINE bool lookahead_first(const Iterator, Iterator, const EndIterator, const flags &, opaque) noexcept {
	return true;
}

}


#endif
