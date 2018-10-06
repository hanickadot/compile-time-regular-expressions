#ifndef CTRE__FA__MATCH__HPP
#define CTRE__FA__MATCH__HPP

#include "select.hpp"

#include <limits>
#include <string_view>

namespace ctfa {
	
template <typename... Jumps> using fa = ctll::list<Jumps...>;

template <auto Id, bool Final, typename... Jumps> struct basic_state {
	static constexpr auto id = Id;
	static constexpr bool final = Final;
	static constexpr auto jumps = ctll::list<Jumps...>{};
};

template <auto Id, typename... Jumps> using final_state = basic_state<Id, true, Jumps...>;
template <auto Id, typename... Jumps> using state = basic_state<Id, false, Jumps...>;

template <auto First, auto Last, unsigned Next> struct range { };

template <auto Value, unsigned Next> struct eq { };


template <unsigned Next> struct sink { };

static constexpr unsigned not_found = std::numeric_limits<unsigned>::max();

template <typename Symbol> constexpr inline unsigned match_jump(Symbol, ctll::list<>) {
	return not_found;
}

template <typename Symbol, auto First, auto Last, unsigned Next, typename... Tail> constexpr inline unsigned match_jump(Symbol s, ctll::list<ctfa::range<First, Last, Next>, Tail...>) {
	if ((First <= s) && (s <= Last)) {
		return Next;
	} else {
		return match_jump(s, ctll::list<Tail...>{});
	}
}

template <typename Symbol, auto Value, unsigned Next, typename... Tail> constexpr inline unsigned match_jump(Symbol s, ctll::list<ctfa::eq<Value, Next>, Tail...>) {
	if (s == Value) {
		return Next;
	} else {
		return match_jump(s, ctll::list<Tail...>{});
	}
}

template <typename Symbol, unsigned Next, typename... Tail> constexpr inline unsigned match_jump(Symbol s, ctll::list<ctfa::sink<Next>, Tail...>) {
	return Next;
}

template <typename Table, typename Iterator> constexpr bool match(Iterator current, const Iterator end) {
	unsigned current_state = 0;
	bool matching{false};
	while (current_state != not_found) {
		current_state = ctfa::select(Table{}, current_state, [&](auto state) {
			if (current == end) {
				matching = decltype(state)::final;
				return not_found;
			} else {
				return match_jump(*current++, state.jumps);
			}
		}, []{
			return not_found;
		});
	}
	return matching;
}

template <typename Table, typename Iterator> constexpr bool search(Iterator current, const Iterator end) {
	unsigned current_state = 0;
	bool matching{false};
	while (current_state != not_found) {
		current_state = ctfa::select(Table{}, current_state, [&](auto state) {
			if (decltype(state)::final) {
				matching = true;
			}
			if (current == end) {
				return not_found;
			} else {
				auto next = match_jump(*current++, state.jumps);
				return next;
			}
		}, []{
			return not_found;
		});
	}
	return matching;
}

template <typename Table> constexpr bool match(std::string_view subject) {
	return match<Table>(subject.begin(), subject.end());
}

template <typename Table> constexpr bool search(std::string_view subject) {
	return search<Table>(subject.begin(), subject.end());
}

}

#endif
