#ifndef CTRE__PCRE_ACTIONS__HPP
#define CTRE__PCRE_ACTIONS__HPP

#include "pcre.hpp"

namespace ctre {

#define ACTION(name) template <auto V, typename Ts...> constexpr auto action(name, term<V>, list<Ts...>)

ACTION(pcre::push_character) {
	return push(character<V>(),list<Ts...>);
}



};

#endif
