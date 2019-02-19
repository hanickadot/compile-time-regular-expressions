#include <ctfmt.hpp>
#include <iostream>


static constexpr ctll::basic_fixed_string pattern = "Hello {1} from {0}!\n";

template <typename T> struct identify;

int main() {
	auto fmt = ctfmt::format<pattern>();
	//identify<decltype(fmt)> hello;
	std::array<char, 64> buffer;
	
	std::string_view a = fmt(buffer, "Hana","std::format");
	fwrite(a.data(), a.size(), 1, stdout);
	std::string_view b = fmt(buffer, "C++20","programmers");
	fwrite(b.data(), b.size(), 1, stdout);
}