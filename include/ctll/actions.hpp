#ifndef CTLL__ACTIONS__HPP
#define CTLL__ACTIONS__HPP

namespace ctll {
	struct empty_subject { };
	
	struct empty_actions {
		// dummy operator so using Actions::operator() later will not give error
		template <typename Action, typename InputSymbol, typename Subject> static constexpr auto apply(Action, InputSymbol, Subject subject) {
			return subject;
		}
	};
	
	template <typename Actions> struct identity: public Actions {
		using Actions::apply;
		// allow empty_subject to exists
		template <typename Action, auto V> constexpr static auto apply(Action, term<V>, empty_subject) -> empty_subject { return {}; }
		template <typename Action> constexpr static auto apply(Action, epsilon, empty_subject) -> empty_subject { return {}; }	
	};
	
	template <typename Actions> struct ignore_unknown: public Actions {
		using Actions::apply;
		// allow flow thru unknown actions
		template <typename Action, auto V, typename Subject> constexpr static auto apply(Action, term<V>, Subject) -> Subject { return {}; }
		template <typename Action, typename Subject> constexpr static auto apply(Action, epsilon, Subject) -> Subject { return {}; }	
	};
}

#endif
