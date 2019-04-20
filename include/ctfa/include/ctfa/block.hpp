#ifndef CTFA__BASIC__HPP
#define CTFA__BASIC__HPP

#include "basic/fa.hpp"

namespace ctfa::block {

static constexpr auto empty = finite_automaton<0,1>{{}, {state{0}}};

template <char32_t Value> static constexpr auto unit = finite_automaton<1,1>{{transition{state{0}, state{1}, ctfa::matcher::unit<Value>}}, {state{1}}};

template <char32_t A, char32_t B> static constexpr auto range = finite_automaton<1,1>{{transition{state{0}, state{1}, ctfa::matcher::range<A,B>}}, {state{1}}};

static constexpr auto anything = finite_automaton<1,1>{{transition{state{0}, state{1}, ctfa::matcher::anything}}, {state{1}}};

template <char32_t... Str> static constexpr auto string = []{
	char32_t buffer[] = {Str...};
	finite_automaton<sizeof...(Str),1> out;
	for (int i = 0; i != sizeof...(Str); ++i) {
		out.add(transition{state{i}, state{i+1}, condition{impl::range{buffer[i], buffer[i]}}});
	}
	out.mark_final(state{sizeof...(Str)});
	return out;
}();

template <char32_t... Values> static constexpr auto set = []{
	finite_automaton<sizeof...(Values),1> out;
	(out.add(transition{start_state, state{1}, condition{impl::range{Values, Values}}}), ...);
	out.mark_final(invalid_state);
	return out;
}();


}

#endif
