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
};

/*
TODO:
	struct class_named_alnum: ctll::action {};
	struct class_named_alpha: ctll::action {};
	struct class_named_ascii: ctll::action {};
	struct class_named_blank: ctll::action {};
	struct class_named_cntrl: ctll::action {};
	struct class_named_digit: ctll::action {};
	struct class_named_graph: ctll::action {};
	struct class_named_lower: ctll::action {};
	struct class_named_print: ctll::action {};
	struct class_named_punct: ctll::action {};
	struct class_named_space: ctll::action {};
	struct class_named_upper: ctll::action {};
	struct class_named_word: ctll::action {};
	struct class_named_xdigit: ctll::action {};
	struct create_number: ctll::action {};
	struct insert_begin: ctll::action {};
	struct insert_end: ctll::action {};
	struct insert_range: ctll::action {};
	struct make_capture: ctll::action {};
	struct make_capture_with_name: ctll::action {};
	struct make_lazy: ctll::action {};
	struct make_optional: ctll::action {};
	struct make_possessive: ctll::action {};
	struct make_sequence: ctll::action {};
	struct negate_class_named: ctll::action {};
	struct push_name: ctll::action {};
	struct push_number: ctll::action {};
	struct repeat_ab: ctll::action {};
	struct repeat_at_least: ctll::action {};
	struct repeat_exactly: ctll::action {};
	struct repeat_plus: ctll::action {};
	struct repeat_star: ctll::action {};
	struct set_combine: ctll::action {};
	struct set_finish: ctll::action {};
	struct set_neg_start: ctll::action {};
*/


};

#endif
