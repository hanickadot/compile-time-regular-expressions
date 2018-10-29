#ifndef CTRE_V2__CTRE__OPERATORS__HPP
#define CTRE_V2__CTRE__OPERATORS__HPP

template <typename A, typename B> constexpr auto operator|(ctre::regular_expression<A>, ctre::regular_expression<B>) -> ctre::regular_expression<ctre::select<A,B>> {
	return {};
}

template <typename A, typename B> constexpr auto operator>>(ctre::regular_expression<A>, ctre::regular_expression<B>) -> ctre::regular_expression<ctre::sequence<A,B>> {
	return {};
}

#endif
