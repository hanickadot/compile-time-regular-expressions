#ifndef CTRE__FA__SELECT__HPP
#define CTRE__FA__SELECT__HPP

#include "ctll/list.hpp"
#include <utility>

namespace ctfa {


template <typename Id, typename Found, typename NotFound> constexpr inline auto select(ctll::list<>, const Id, Found && found, NotFound && not_found) noexcept {
	return not_found();
}

template <typename Id, typename Head, typename... Tail, typename Found, typename NotFound> constexpr inline auto select(ctll::list<Head, Tail...>, const Id id, Found && found, NotFound && not_found) noexcept {
	if (Head::id == id) {
		return found(Head{});
	} else {
		return ctfa::select(ctll::list<Tail...>{}, id, std::forward<Found>(found), std::forward<NotFound>(not_found));
	}
}

}

#endif
