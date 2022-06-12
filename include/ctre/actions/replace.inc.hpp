#ifndef CTRE__ACTIONS__REPLACE__HPP
#define CTRE__ACTIONS__REPLACE__HPP

// appending char to sequence
template <auto V, typename... Ts> static constexpr auto apply(replace_gram::replacement_char_push, ctll::term<V>, sequence<Ts...> stack) {
    return sequence<character<V>,Ts...>();
}

template <auto V, auto Other, typename... Ts> static constexpr auto apply(replace_gram::replacement_char_push, ctll::term<V>, sequence<character<Other>, Ts...> stack) {
    return sequence<string<Other, V>,Ts...>();
}

template <auto V, auto... Others, typename... Ts> static constexpr auto apply(replace_gram::replacement_char_push, ctll::term<V>, sequence<string<Others...>, Ts...> stack) {
    return sequence<string<Others..., V>,Ts...>();
}

// add simple back reference
template <auto V, typename... Ts> static constexpr auto apply(replace_gram::replacement_capture, ctll::term<V>, sequence<Ts...> stack) {
    constexpr size_t id = static_cast<char>(V) - '0';
    return sequence<back_reference<id>, Ts...>();
}

// add another digit to back_reference
template <auto V, size_t Id, typename... Ts> static constexpr auto apply(replace_gram::replacement_capture_push, ctll::term<V>, sequence<back_reference<Id>, Ts...> seq) {
    constexpr size_t id = Id * 10 + (static_cast<char>(V) - '0');
	return sequence<back_reference<id>, Ts...>();
}

// rotate add the end (as C++ doesn't access last element of a parameter pack)
template <typename T> struct sequence_rotate_item {
	template <typename... Ts> friend constexpr auto operator+(sequence<Ts...>, sequence_rotate_item<T>) noexcept -> sequence<T, Ts...> { return {}; }
};

template <typename... Ts> static constexpr auto rotate_sequence(sequence<Ts...>) -> decltype((sequence<>{} + ... + sequence_rotate_item<Ts>{})) {
	return {};
}

template <auto V, typename... Ts> static constexpr auto apply(replace_gram::rotate, ctll::term<V>, sequence<Ts...> seq) {
	return rotate_sequence(seq);
}

#endif
