#ifndef CTRE__ACTIONS__CAPTURE__HPP
#define CTRE__ACTIONS__CAPTURE__HPP

// prepare_capture
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::prepare_capture, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_id<Counter+1>(), ctll::list<Ts...>()), pcre_parameters<Counter+1>()};
}

// reset_capture
template <auto V, typename... Ts, size_t Id, size_t Counter> static constexpr auto apply(pcre::reset_capture, ctll::term<V>, pcre_context<ctll::list<capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::list<Ts...>(), pcre_parameters<Counter-1>()};
}

// capture
template <auto V, typename A, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture, ctll::term<V>, pcre_context<ctll::list<A, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture<Id, A>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}
// capture (sequence)
template <auto V, typename... Content, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture<Id, Content...>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}
// push_name
template <auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_name, ctll::term<V>, pcre_context<ctll::list<Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(id<V>(), subject.stack), subject.parameters};
}
// push_name (concat)
template <auto... Str, auto V, typename... Ts, typename Parameters> static constexpr auto apply(pcre::push_name, ctll::term<V>, pcre_context<ctll::list<id<Str...>, Ts...>, Parameters> subject) {
	return pcre_context{ctll::push_front(id<Str..., V>(), ctll::list<Ts...>()), subject.parameters};
}
// capture with name
template <auto... Str, auto V, typename A, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture_with_name, ctll::term<V>, pcre_context<ctll::list<A, id<Str...>, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_with_name<Id, id<Str...>, A>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}
// capture with name (sequence)
template <auto... Str, auto V, typename... Content, size_t Id, typename... Ts, size_t Counter> static constexpr auto apply(pcre::make_capture_with_name, ctll::term<V>, pcre_context<ctll::list<sequence<Content...>, id<Str...>, capture_id<Id>, Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ctll::push_front(capture_with_name<Id, id<Str...>, Content...>(), ctll::list<Ts...>()), pcre_parameters<Counter>()};
}

template<size_t To, auto... Str, typename T>
static constexpr auto replace_captures_with_id(T, string<Str...>) { // fallback case, no transform
	return T{};
}

template<size_t To, auto... Str, size_t From, typename... Content>
static constexpr auto replace_captures_with_id(capture<From, Content...>, string<Str...>) {
	return capture<To, decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}
// named reseted captures must have same Str... for all instances inside the call (otherwise how would we know how to access which by name?)
template<size_t To, auto... Str, size_t From, auto... Str2, typename... Content>
static constexpr auto replace_captures_with_id(capture_with_name<From, id<Str2...>, Content...>, string<Str...>) {
	static_assert((id<Str...>{} == id<Str2...>{}) && "named captures must be named the same in reset group");
	return capture_with_name<To, id<Str...>, decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, typename... Content>
static constexpr auto replace_captures_with_id(sequence<Content...>, string<Str...>) {
	return sequence<decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, size_t Id, typename... Content>
static constexpr auto replace_captures_with_id(reset_group<Id, Content...>, string<Str...>) {
	return reset_group<To, decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, size_t Id, auto... Str2, typename... Content>
static constexpr auto replace_captures_with_id(reset_group_with_name<Id, id<Str2...>, Content...>, string<Str...>) {
	return reset_group_with_name<To, id<Str...>, decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, typename... Content>
static constexpr auto replace_captures_with_id(atomic_group<Content...>, string<Str...>) {
	return atomic_group<decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, typename... Content>
static constexpr auto replace_captures_with_id(lookahead_positive<Content...>, string<Str...>) {
	return lookahead_positive<decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, typename... Content>
static constexpr auto replace_captures_with_id(lookahead_negative<Content...>, string<Str...>) {
	return lookahead_negative<decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, size_t A, size_t B, typename... Content>
static constexpr auto replace_captures_with_id(possessive_repeat<A, B, Content...>, string<Str...>) {
	return possessive_repeat<A, B, decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, size_t A, size_t B, typename... Content>
static constexpr auto replace_captures_with_id(repeat<A, B, Content...>, string<Str...>) {
	return repeat<A, B, decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, size_t A, size_t B, typename... Content>
static constexpr auto replace_captures_with_id(lazy_repeat<A, B, Content...>, string<Str...>) {
	return lazy_repeat<A, B, decltype(replace_captures_with_id<To>(Content{}, string<Str...>{}))...>{};
}

template<size_t To, auto... Str, typename... Opts>
static constexpr auto replace_captures_with_id(select<Opts...>, string<Str...>) {
	return select<decltype(replace_captures_with_id<To>(Opts{}, string<Str...>{}))...>{};
}

// get name (might be a utility already written)
template<size_t Idx, auto... Str>
static constexpr string<Str...> get_capture_name(captured_content<Idx, id<Str...>>) noexcept {
	return string<Str...>{};
}

template<size_t Idx>
static constexpr string<> get_capture_name(captured_content<Idx>) noexcept {
	return string<>{};
}

template<typename T>
static constexpr string<> get_capture_name(T) noexcept {
	return string<>{};
}

template<auto... Str>
static constexpr id<Str...> make_id_from_string(string<Str...>) noexcept {
	return id<Str...>{};
}

// find the first named capture
template<typename H, typename... Tail>
static constexpr auto get_capture_with_name(ctll::list<H, Tail...>) noexcept {
	if constexpr (sizeof...(Tail))
		return get_capture_with_name(ctll::list<Tail...>{});
	else
		return ctll::list<>{};
}

template<size_t Idx, typename Name, typename... Tail>
static constexpr auto get_capture_with_name(ctll::list<captured_content<Idx, Name>, Tail...>) noexcept {
	return ctll::list<captured_content<Idx, Name>>{};
}

// reset group start
template <auto V, typename... Ts, size_t Counter> static constexpr auto apply(pcre::start_reset_group, ctll::term<V>, pcre_context<ctll::list<Ts...>, pcre_parameters<Counter>>) {
	return pcre_context{ ctll::push_front(reset_start<Counter+1>(), ctll::list<Ts...>()), pcre_parameters<Counter>() };
}

// reset group end
template <auto V, typename A, typename... Ts, size_t Id, size_t Counter> static constexpr auto apply(pcre::make_reset_group, ctll::term<V>, pcre_context<ctll::list<A, reset_start<Id>, Ts...>, pcre_parameters<Counter>>) {
	using first_capture = decltype(ctll::front(find_captures(A{})));
	if constexpr (::std::is_same_v<first_capture, ctll::_nothing>) {
		//no captures to reset... easy case
		return pcre_context{ ctll::list<sequence<A>, Ts...>(), pcre_parameters<Counter>() };
	} else {
		using first_named_capture = decltype(ctll::front(get_capture_with_name(find_captures(A{}))));
		using capture_name = decltype(get_capture_name(first_named_capture{}));
		if constexpr (::std::is_same_v<first_named_capture, ctll::_nothing> || ::std::is_same_v<capture_name, string<>>) {
			return pcre_context{ ctll::list<reset_group<Id, decltype(replace_captures_with_id<Id>(A{}, string<>{}))>, Ts...>(), pcre_parameters<Counter>() };
		} else {
			return pcre_context{ ctll::list<reset_group_with_name<Id, decltype(make_id_from_string(capture_name{})), decltype(replace_captures_with_id<Id>(A{}, capture_name{}))>, Ts...>(), pcre_parameters<Counter>() };
		}
	}
}
// reset group end (sequence)
template <auto V, typename... Content, typename... Ts, size_t Id, size_t Counter> static constexpr auto apply(pcre::make_reset_group, ctll::term<V>, pcre_context<ctll::list<ctre::sequence<Content...>, reset_start<Id>, Ts...>, pcre_parameters<Counter>>) {
	using first_capture = decltype(ctll::front(find_captures(sequence<Content...>{})));
	if constexpr (::std::is_same_v<first_capture, ctll::_nothing>) {
		//no captures to reset... easy case
		return pcre_context{ ctll::list<sequence<Content...>, Ts...>(), pcre_parameters<Counter>() };
	} else {
		using first_named_capture = decltype(ctll::front(get_capture_with_name(find_captures(sequence<Content...>{}))));
		using capture_name = decltype(get_capture_name(first_named_capture{}));
		if constexpr (::std::is_same_v<first_named_capture, ctll::_nothing> || ::std::is_same_v<capture_name, string<>>) {
			return pcre_context{ ctll::list<reset_group<Id, decltype(replace_captures_with_id<Id>(Content{}, string<>{}))...>, Ts...>(), pcre_parameters<Counter>() };
		} else {
			return pcre_context{ ctll::list<reset_group_with_name<Id, decltype(make_id_from_string(capture_name{})), decltype(replace_captures_with_id<Id>(Content{}, capture_name{}))...>, Ts...>(), pcre_parameters<Counter>() };
		}
	}
}



#endif
