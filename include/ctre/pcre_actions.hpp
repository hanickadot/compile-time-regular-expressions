#ifndef CTRE__PCRE_ACTIONS__HPP
#define CTRE__PCRE_ACTIONS__HPP

#ifndef CTRE__PCRE__HPP
#include "pcre.hpp"
#endif

#ifndef CTRE__ATOMS__HPP
#include "atoms.hpp"
#endif

#ifndef CTRE__ATOMS_CHARACTERS__HPP
#include "atoms_characters.hpp"
#endif

#ifndef CTRE__ATOMS_UNICODE__HPP
#include "atoms_unicode.hpp"
#endif

#ifndef CTRE__ID__HPP
#include "id.hpp"
#endif

#include <cstdint>
#include <limits>

namespace ctre {
	
template <size_t Counter> struct pcre_parameters {
	static constexpr size_t current_counter = Counter;
};
	
template <typename Stack = ctll::list<>, typename Parameters = pcre_parameters<0>> struct pcre_context {
	using stack_type = Stack;
	using parameters_type = Parameters;
	static constexpr inline auto stack = stack_type();
	static constexpr inline auto parameters = parameters_type();
	constexpr pcre_context() noexcept { }
	constexpr pcre_context(Stack, Parameters) noexcept { }
};

template <typename... Content, typename Parameters> pcre_context(ctll::list<Content...>, Parameters) -> pcre_context<ctll::list<Content...>, Parameters>;
	
template <size_t Value> struct number { };

template <size_t Id> struct capture_id { };
	
struct pcre_actions {
// i know it's ugly, but it's more readable
#include "actions/asserts.inc.hpp"
#include "actions/backreference.inc.hpp"
#include "actions/capture.inc.hpp"
#include "actions/characters.inc.hpp"
#include "actions/class.inc.hpp"
#include "actions/hexdec.inc.hpp"
#include "actions/look.inc.hpp"
#include "actions/named_class.inc.hpp"
#include "actions/options.inc.hpp"
#include "actions/repeat.inc.hpp"
#include "actions/sequence.inc.hpp"
#include "actions/set.inc.hpp"
#include "actions/properties.inc.hpp"

};


}

#endif
