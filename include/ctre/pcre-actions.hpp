#ifndef CTRE__PCRE_ACTIONS__HPP
#define CTRE__PCRE_ACTIONS__HPP

#include "pcre.hpp"

namespace ctre {
	
template <auto v> struct character { };

template <auto V, typename... Ts> constexpr auto action(pcre::push_character, ctll::term<V>, ctll::list<Ts...> stack) {
	return push(character<V>(), stack);
}



};

#endif
