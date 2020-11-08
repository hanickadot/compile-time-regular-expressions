#ifndef CTRE__FIND_CAPTURES__HPP
#define CTRE__FIND_CAPTURES__HPP

#include "atoms_characters.hpp"
#include "atoms_unicode.hpp"
#include "utility.hpp"
#include "return_type.hpp"

namespace ctre {

template <typename Pattern> constexpr auto find_captures(Pattern) noexcept {
	return find_captures(ctll::list<Pattern>(), ctll::list<>());
}

template <typename... Output> constexpr auto find_captures(ctll::list<>, ctll::list<Output...> output) noexcept {
	return output;
}



template <auto... String, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<string<String...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}

template <typename... Options, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<select<Options...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Options..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<optional<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_optional<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<sequence<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Tail, typename Output> constexpr auto find_captures(ctll::list<empty, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}


template <typename... Tail, typename Output> constexpr auto find_captures(ctll::list<assert_subject_begin, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}


template <typename... Tail, typename Output> constexpr auto find_captures(ctll::list<assert_subject_end, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}


// , typename = std::enable_if_t<(MatchesCharacter<CharacterLike>::template value<char>)
template <typename CharacterLike, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<CharacterLike, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Tail...>(), output);
}


template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<plus<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<star<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <size_t A, size_t B, typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<repeat<A,B,Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}


template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_plus<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_star<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <size_t A, size_t B, typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lazy_repeat<A,B,Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}


template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<possessive_plus<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<possessive_star<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}

template <size_t A, size_t B, typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<possessive_repeat<A,B,Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}



template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lookahead_positive<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}


template <typename... Content, typename... Tail, typename Output> constexpr auto find_captures(ctll::list<lookahead_negative<Content...>, Tail...>, Output output) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), output);
}




template <size_t Id, typename... Content, typename... Tail, typename... Output> constexpr auto find_captures(ctll::list<capture<Id,Content...>, Tail...>, ctll::list<Output...>) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), ctll::list<Output..., captured_content<Id>>());
}


template <size_t Id, typename Name, typename... Content, typename... Tail, typename... Output> constexpr auto find_captures(ctll::list<capture_with_name<Id,Name,Content...>, Tail...>, ctll::list<Output...>) noexcept {
	return find_captures(ctll::list<Content..., Tail...>(), ctll::list<Output..., captured_content<Id, Name>>());
}



}

#endif
