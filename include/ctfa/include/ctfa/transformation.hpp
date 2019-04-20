#ifndef CTFA__TRANSFORMATION__HPP
#define CTFA__TRANSFORMATION__HPP

#include "transformations/alternative.hpp"
#include "transformations/concat.hpp"
#include "transformations/optional.hpp"
#include "transformations/plus.hpp"
#include "transformations/star.hpp"
#include "transformations/shrink.hpp"
#include "transformations/remove_unneeded.hpp"
#include "transformations/minimize.hpp"
#include "transformations/determinize.hpp"
#include "transformations/character_set.hpp"

namespace ctfa {

namespace utility {

template <template <const auto &, const auto &> typename BinaryOp, const auto & ... Fas> struct apply_2;
	
template <template <const auto &, const auto &> typename BinaryOp, const auto & Fa> struct apply_2<BinaryOp, Fa> {
	static constexpr auto & result = Fa;
};

template <template <const auto &, const auto &> typename BinaryOp, const auto & Lhs, const auto & Rhs> struct apply_2<BinaryOp, Lhs, Rhs> {
	static constexpr auto & result = BinaryOp<Lhs, Rhs>::result;
};

template <template <const auto &, const auto &> typename BinaryOp, const auto & Lhs, const auto & Rhs, const auto & ... Fas> struct apply_2<BinaryOp, Lhs, Rhs, Fas...> {
	static constexpr auto & result = apply_2<BinaryOp, BinaryOp<Lhs, Rhs>::result, Fas...>::result;
};
 

}

template <const auto & ... Fas> static constexpr auto concat = utility::apply_2<concat_two, Fas...>::result;

template <const auto & ... Fas> static constexpr auto alternative = utility::apply_2<alternative_two, Fas...>::result;

template <const auto & ... Fas> static constexpr auto plus = plus_one<utility::apply_2<concat_two, Fas...>::result>::result;

template <const auto & ... Fas> static constexpr auto star = star_one<utility::apply_2<concat_two, Fas...>::result>::result;

template <const auto & ... Fas> static constexpr auto optional = optional_one<utility::apply_2<concat_two, Fas...>::result>::result;

template <const auto & ... Fas> static constexpr auto shrink = shrink_one<utility::apply_2<concat_two, Fas...>::result>::result;

template <const auto & ... Fas> static constexpr auto remove_unneeded = remove_unneeded_one<utility::apply_2<concat_two, Fas...>::result>::result;

template <const auto & ... Fas> static constexpr auto minimize = minimize_one<utility::apply_2<concat_two, Fas...>::result>::result;

template <const auto & ... Fas> static constexpr auto determinize = determinize_one<remove_unneeded<Fas...>>::result;

template <const auto & ... Fas> static constexpr auto join_character_set = character_set_n<Fas...>::result;


}

#endif
