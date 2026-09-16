#ifndef CTRE__REFLECT__HPP
#define CTRE__REFLECT__HPP

// ctre::reflect — C++26-reflection wrappers that automatically convert
// regex result into a user-provided aggregate type. Supports named and
// unnamed capture groups, optional capture groups in the pattern.
// Fields with no matching capture are left default-constructed.

#if defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202603L
#define CTRE_SUPPORTS_CPP26_REFLECTION
#endif

#ifdef CTRE_SUPPORTS_CPP26_REFLECTION

#include "../ctll/fixed_string.hpp"
#include "utility.hpp"
#include "wrapper.hpp"
#ifndef CTRE_IN_A_MODULE
#include <array>
#include <charconv>
#include <meta>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#endif

namespace ctre::reflect::detail {

template <std::meta::info E> consteval auto get_field_name_fs() {
	constexpr std::string_view sv = std::meta::identifier_of(E);
	std::array<char8_t, sv.size()> a{};
	for (std::size_t i = 0; i < sv.size(); ++i) a[i] = static_cast<char8_t>(sv[i]);
	return ctll::fixed_string(a);
}

consteval bool sv_equals_fs(std::string_view id, auto FS) {
	if (id.size() != FS.size()) return false;
	for (std::size_t i = 0; i < id.size(); ++i)
		if (static_cast<char32_t>(static_cast<unsigned char>(id[i])) != FS[i]) return false;
	return true;
}

consteval std::meta::info find_field_by_fs(std::meta::info type_info, auto FS) {
	for (auto m : std::meta::nonstatic_data_members_of(type_info, std::meta::access_context::current()))
		if (sv_equals_fs(std::meta::identifier_of(m), FS)) return m;
	return std::meta::info{};
}

// Wrapper so the static_assert fires inside a consteval context
template <typename T, auto FS>
consteval void require_field_exists() {
	static_assert(std::meta::is_nonstatic_data_member(find_field_by_fs(^^T, FS)),
		"ctre::reflect: named capture has no matching struct field");
}

template <typename T, std::size_t I> consteval std::meta::info get_nth_field() {
	return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[I];
}

template <typename Result> consteval bool are_all_captures_named() {
	return std::remove_cvref_t<Result>::are_all_captures_named();
}

template <typename T> concept optional_like = requires(T t, typename T::value_type v) {
	{ t.has_value() } -> std::convertible_to<bool>;
	{ *t } -> std::same_as<typename T::value_type &>;
	t = v;
};

// Convert a capture into a field. Absent capture leaves the field at its default.
// Supports optional-like fields and sets their inner values.
// For numeric fields, std::from_chars is used to convert captured to numberic value.
// (TODO: add a customization point for arbitrary field conversion.)
// Conversions errors fail the whole regex match.
template <typename Field, typename Capture> constexpr bool assign_value(Field & out, const Capture & cap) {
	if (!static_cast<bool>(cap)) return true; // optional capture group is absent, skip
	if constexpr (optional_like<Field>) {
		typename Field::value_type inner{};
		if (!assign_value(inner, cap)) return false;
		out = std::move(inner);
	} else if constexpr (std::is_arithmetic_v<Field> && !std::is_same_v<Field, bool>) {
		static_assert(std::is_same_v<typename std::decay_t<decltype(cap.to_view())>::value_type, char>,
			"ctre::reflect: numeric field conversion requires a char subject (std::from_chars only supports const char*)");
		const auto sv = cap.to_view();
		const auto * first = std::to_address(sv.begin());
		const auto [ptr, ec] = std::from_chars(first, first + sv.size(), out);
		return ec == std::errc{} && ptr == first + sv.size();
	} else {
		out = static_cast<Field>(cap);
	}
	return true;
}

template <typename T, std::size_t CaptureId, typename Result>
constexpr bool assign_field_from_named_capture(T & out, const Result & m) {
	using cap_name_tag = typename std::remove_cvref_t<decltype(m.template get<CaptureId>())>::name;
	require_field_exists<T, cap_name_tag::name>();
	constexpr auto e = find_field_by_fs(^^T, cap_name_tag::name);
	using field_t = std::remove_cvref_t<decltype(out.[:e:])>;
	return assign_value<field_t>(out.[:e:], m.template get<CaptureId>());
}

template <typename T, std::size_t CaptureId, typename Result>
constexpr bool assign_field_from_positional_capture(T & out, const Result & m) {
	constexpr std::size_t n = std::meta::nonstatic_data_members_of(
		^^T, std::meta::access_context::current()).size();
	static_assert(CaptureId <= n, "ctre::reflect: more captures than struct fields");
	constexpr auto e = get_nth_field<T, CaptureId - 1>();
	using cap_t = std::remove_cvref_t<decltype(m.template get<CaptureId>())>;
	if constexpr (!std::is_void_v<typename cap_t::name>)
		static_assert(get_field_name_fs<e>().is_same_as(cap_t::name::name),
			"ctre::reflect: named capture does not match field name at this position");
	using field_t = std::remove_cvref_t<decltype(out.[:e:])>;
	return assign_value<field_t>(out.[:e:], m.template get<CaptureId>());
}

// Fill T from a match. If all capture groups in the regex pattern are named, the order of fields
// is not important. Otherwise struct fields assigned by position, and named group must match
// corresponding fields order. Any mismatch is a compile error.
// Extra struct fields stay default-constructed.
template <typename T, typename Result> constexpr std::optional<T> to_struct(const Result & m) {
	T out{};
	constexpr std::size_t cnt = std::remove_cvref_t<Result>::count() - 1; // count() includes capture 0
	const bool ok = [&]<std::size_t... I>(std::index_sequence<I...>) {
		if constexpr (are_all_captures_named<Result>())
			return (assign_field_from_named_capture<T, I + 1>(out, m) && ...);
		else
			return (assign_field_from_positional_capture<T, I + 1>(out, m) && ...);
	}(std::make_index_sequence<cnt>{});
	if (!ok) return std::nullopt;
	return out;
}

template <typename T, typename RE, typename... Args>
constexpr std::optional<T> one(RE re, Args &&... args)
	noexcept(noexcept(re(std::forward<Args>(args)...)) &&
	         std::is_nothrow_default_constructible_v<T> &&
	         std::is_nothrow_move_constructible_v<T>) {
	if (auto m = re(std::forward<Args>(args)...)) return to_struct<T>(m);
	return std::nullopt;
}

// Drops matches whose conversion has failed, yields T.
template <typename T, typename RE, typename... Args>
constexpr auto many(RE re, Args &&... args)
	noexcept(noexcept(re(std::forward<Args>(args)...))) {
	return re(std::forward<Args>(args)...)
		| std::views::transform([](auto && m) { return to_struct<T>(m); })
		| std::views::filter([](const auto & o) { return o.has_value(); })
		| std::views::transform([](auto o) -> T { return *std::move(o); });
}

template <typename T, auto RE>
struct one_functor {
	template <typename... A> requires requires { RE(std::forward<A>(std::declval<A>())...); }
	constexpr auto operator()(A &&... a) const noexcept(noexcept(one<T>(RE, std::forward<A>(a)...))) {
		return one<T>(RE, std::forward<A>(a)...);
	}
};

template <typename T, auto RE>
struct many_functor {
	template <typename... A> requires requires { RE(std::forward<A>(std::declval<A>())...); }
	constexpr auto operator()(A &&... a) const noexcept(noexcept(many<T>(RE, std::forward<A>(a)...))) {
		return many<T>(RE, std::forward<A>(a)...);
	}
};

} // namespace ctre::reflect::detail

namespace ctre {
// A "simple struct" ctre::reflect can default-construct and fill: an aggregate class.
CTRE_EXPORT template <typename T> concept reflectable = std::is_class_v<T> && std::is_aggregate_v<T>;
}

namespace ctre::reflect {

CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto match                 = detail::one_functor<T, ctre::match<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto search                = detail::one_functor<T, ctre::search<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto starts_with           = detail::one_functor<T, ctre::starts_with<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto multiline_match       = detail::one_functor<T, ctre::multiline_match<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto multiline_search      = detail::one_functor<T, ctre::multiline_search<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto multiline_starts_with = detail::one_functor<T, ctre::multiline_starts_with<p, Modifiers...>>{};

CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto search_all           = detail::many_functor<T, ctre::search_all<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto multiline_search_all = detail::many_functor<T, ctre::multiline_search_all<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto tokenize             = detail::many_functor<T, ctre::tokenize<p, Modifiers...>>{};
CTRE_EXPORT template <ctre::reflectable T, ctll::fixed_string p, typename... Modifiers> constexpr auto multiline_tokenize   = detail::many_functor<T, ctre::multiline_tokenize<p, Modifiers...>>{};

} // namespace ctre::reflect

#endif // CTRE_SUPPORTS_CPP26_REFLECTION
#endif
