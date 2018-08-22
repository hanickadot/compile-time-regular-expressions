#ifndef CTLL__TRAITS__HPP
#define CTLL__TRAITS__HPP

#include "grammars.hpp"

namespace ctll {
	template <typename T> struct IsAction {
		template <typename Y> static constexpr auto test(Y * y) -> decltype(std::declval<typename Y::action_tag>(), std::true_type{});
		template <typename> static constexpr auto test(...) -> std::false_type;
		static constexpr const bool value = decltype(test<T>(nullptr))();
	};
	
	template <typename T> constexpr bool is_action(T) noexcept {
		return IsAction<T>::value;
	}
	
	constexpr bool is_decision(ctll::accept) noexcept { return true; }
	constexpr bool is_decision(ctll::reject) noexcept { return true; }
	constexpr bool is_decision(...) noexcept { return false; }
	
	constexpr bool is_pop_input(ctll::pop_input) noexcept { return true; }
	constexpr bool is_pop_input(...) noexcept { return false; }
}

#endif