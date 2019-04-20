#ifndef CTFA__BASIC__FA__HPP
#define CTFA__BASIC__FA__HPP

#include "fa.hpp"

namespace ctfa {

template <const auto & Fa, typename TransitionCB, typename FinalCB> void debug(TransitionCB && tran, FinalCB && final) {
	for (const auto & t: Fa.transitions) {
		tran(t.source, t.target, t.cond, Fa.is_final(t.source));
	}
	for (const auto & f: Fa.final_states) {
		if (Fa.transitions.find(f) == Fa.transitions.end()) final(f);
	}
}

}

#endif
