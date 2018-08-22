#ifndef CTLL__ACTIONS__HPP
#define CTLL__ACTIONS__HPP

namespace ctll {
	struct empty_actions {
		// dummy operator so using Actions::operator() later will not give error
		constexpr void operator()() const {};
	};
	
	
	template <bool IgnoreUnknown, typename Actions> struct augment_actions: public Actions {
		using Actions::operator();
		// allow flow thru unknown actions
		template <typename Action, auto V, typename Subject> constexpr auto operator()(Action, term<V>, Subject) const -> Subject { return {}; }
		template <typename Action, typename Subject> constexpr auto operator()(Action, epsilon, Subject) const -> Subject { return {}; }	
	};
	
	template <typename Actions> struct augment_actions<false, Actions>: public Actions {
		using Actions::operator();
		// will fail if unknown action is called
	};
}

#endif
