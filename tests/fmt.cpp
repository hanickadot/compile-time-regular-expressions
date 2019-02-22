#include <ctfmt.hpp>
#include <iostream>

static constexpr ctll::basic_fixed_string pattern = "Hello {1} from {0}!\n";


int main(int argc, char ** argv) {
	auto fmt = ctfmt::format<pattern>();
	
	
	std::string a = fmt("Hana", "std::format");
	fwrite(a.data(), a.size(), 1, stdout);
}
