#include <ctre.hpp>

static constexpr auto pattern = ctll::fixed_string(".?|(..+?)\\g{1}+");

constexpr bool is_prime(std::string_view input) {
	return !ctre::match<pattern>(input);
}

int main()
{
	static_assert(!is_prime(""));
	static_assert(!is_prime("a"));
	static_assert( is_prime("aa"));
	static_assert( is_prime("aaa"));
	static_assert(!is_prime("aaaa"));
	static_assert( is_prime("aaaaa"));
	static_assert(!is_prime("aaaaaa"));
	static_assert( is_prime("aaaaaaa"));
	static_assert(!is_prime("aaaaaaaa"));
	static_assert(!is_prime("aaaaaaaaa"));
	static_assert(!is_prime("aaaaaaaaaa"));
	static_assert( is_prime("aaaaaaaaaaa"));
}