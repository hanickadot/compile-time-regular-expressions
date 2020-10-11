#include "ctre.hpp"

static constexpr inline auto pattern = ctll::fixed_string{"abcdefghijklmnopqrstuvwxy|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz"};

static constexpr inline auto a = ctre::re<pattern>();

template <typename T> void identify(T) { }

void fnc() {
	identify(a);
}


