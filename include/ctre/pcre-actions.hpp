#ifndef CTRE__PCRE_ACTIONS__HPP
#define CTRE__PCRE_ACTIONS__HPP

#include "pcre.hpp"

namespace ctre {
	
template <auto v> struct character { };
template <auto... v> struct string { };
	
struct pcre_actions {
	template <auto V, typename... Ts> constexpr auto operator()(pcre::push_character, ctll::term<V>, ctll::list<Ts...> stack) const {
		return ctll::push_front(character<V>(), stack);
	}	
	template <auto V, auto X, typename... Ts> constexpr auto operator()(pcre::push_character, ctll::term<V>, ctll::list<character<X>, Ts...> stack) const {
		return ctll::push_front(string<X,V>(), ctll::list<Ts...>());
	}
	template <auto V, auto... Xs, typename... Ts> constexpr auto operator()(pcre::push_character, ctll::term<V>, ctll::list<string<Xs...>, Ts...> stack) const {
		return ctll::push_front(string<Xs...,V>(), ctll::list<Ts...>());
	}	
};





};

#endif
