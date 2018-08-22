#ifndef CTRE__PCRE_ACTIONS__HPP
#define CTRE__PCRE_ACTIONS__HPP

#include "pcre.hpp"
#include "atoms.hpp"
#include <cstdint>
#include <limits>

namespace ctre {
	
template <uint64_t Value> struct hexdec_number { };
	
template <uint64_t V> struct id_number;
	
struct pcre_actions {
// i know it's ugly, but it's more readable
#include "actions/characters.inc.hpp"
#include "actions/sequence.inc.hpp"
#include "actions/hexdec.inc.hpp"
#include "actions/options.inc.hpp"
#include "actions/class.inc.hpp"
#include "actions/repeat.inc.hpp"
#include "actions/named_class.inc.hpp"
#include "actions/set.inc.hpp"

};

/*
TODO:
	struct create_number: ctll::action {};
	struct insert_begin: ctll::action {};
	struct insert_end: ctll::action {};
	struct insert_range: ctll::action {};
	struct make_capture: ctll::action {};
	struct make_capture_with_name: ctll::action {};
	struct make_lazy: ctll::action {};
	struct make_possessive: ctll::action {};
	struct make_sequence: ctll::action {};
	struct negate_class_named: ctll::action {};
	struct push_name: ctll::action {};
	struct push_number: ctll::action {};
	struct repeat_ab: ctll::action {};
	struct repeat_at_least: ctll::action {};
	struct repeat_exactly: ctll::action {};
*/


};

#endif
