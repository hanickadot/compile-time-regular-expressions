#ifndef CTLL__ACTIONS__HPP
#define CTLL__ACTIONS__HPP

namespace ctll {
	struct empty_actions {
		// dummy operator so using Actions::operator() later will not give error
		template <typename Action, typename InputSymbol, typename Subject> static constexpr auto apply(Action, InputSymbol, Subject subject) {
			return subject;
		}
	};
	
	
	template <bool IgnoreUnknown, typename Actions> struct augment_actions: public Actions {
		using Actions::apply;
		// allow flow thru unknown actions
		template <typename Action, auto V, typename Subject> constexpr static auto apply(Action, term<V>, Subject) -> Subject { return {}; }
		template <typename Action, typename Subject> constexpr static auto apply(Action, epsilon, Subject) -> Subject { return {}; }	
	};
	
	template <typename Actions> struct augment_actions<false, Actions>: public Actions {
		using Actions::apply;
		// will fail if unknown action is called
	};
}

#endif
