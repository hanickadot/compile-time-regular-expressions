#ifndef CTRE__HAS_IMPLICIT_ANCHOR__HPP
#define CTRE__HAS_IMPLICIT_ANCHOR__HPP

#include "atoms.hpp"
#include "atoms_unicode.hpp"

namespace ctre {

	template<typename... Content>
	constexpr bool has_implicit_anchor(ctll::list<repeat<0, 0, any>, Content...>) noexcept {
		return true;
	}
	template<typename... Content>
	constexpr bool has_implicit_anchor(ctll::list<repeat<1, 0, any>, Content...>) noexcept {
		return true;
	}

	template<typename... Content>
	constexpr bool has_implicit_anchor(ctll::list<possessive_repeat<0, 0, any>, Content...>) noexcept {
		return true;
	}

	template<typename... Content>
	constexpr bool has_implicit_anchor(ctll::list<possessive_repeat<1, 0, any>, Content...>) noexcept {
		return true;
	}

	template<typename... Content>
	constexpr bool has_implicit_anchor(ctll::list<lazy_repeat<0, 0, any>, Content...>) noexcept {
		return true;
	}

	template<typename... Content>
	constexpr bool has_implicit_anchor(ctll::list<lazy_repeat<1, 0, any>, Content...>) noexcept {
		return true;
	}

	template<typename... Content, typename... Tail>
	constexpr bool has_implicit_anchor(ctll::list<sequence<Content...>, Tail...>) noexcept {
		return has_implicit_anchor(ctll::list<Content..., Tail...>{});
	}

	//TODO: we may check captures as implicit anchors*, but they must not be backreferenced because for example "(.+)X\1" will not work properly with "1234X2345"
	/*
	template<size_t Id, typename... Content, typename... Tail>
	constexpr bool has_implicit_anchor(ctll::list<capture<Id, Content...>, Tail...>) noexcept {
		//Id must not be backreferenced
		return !id_backreference(Id) && has_implicit_anchor(ctll::list<Content..., Tail...>{});
	}

	template<size_t Id, typename Name, typename... Content, typename... Tail>
	constexpr bool has_implicit_anchor(ctll::list<capture_with_name<Id, Name, Content...>, Tail...>) noexcept {
		//Id must not be backreferenced
		return !id_backreference(Id) && has_implicit_anchor(ctll::list<Content..., Tail...>{});
	}
	*/

	template<typename... Opts, typename... Tail>
	constexpr bool has_implicit_anchor(ctll::list<select<Opts...>, Tail...>) noexcept {
		return (has_implicit_anchor(ctll::list<Opts, Tail...>{}) && ...);
	}

	constexpr bool has_implicit_anchor(...) noexcept {
		return false;
	}
}

#endif