#include <ctre/tuple.hpp>
#include <iostream>

//bool match(std::string_view in) {
//	auto r = ctre::search<"([a-z]+)">(in);
//	return r;
//}

const char * print_compare(auto val) {
	if (std::is_eq(val)) {
		return "==";
	} else if (std::is_lt(val)) {
		return "<";
	} else if (std::is_gt(val)) {
		return ">";
	} else {
		return "!=";
	}
}

void print(const auto & a, const auto & b) {
	std::cout << "(" << a << ") " << print_compare(a <=> b) << " (" << b << ")\n";
}

int main() {
	
	int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;
	
	print(ctre::tie(a,b,c), ctre::tie(d,e,f));

	ctre::tuple<int, int, int> tpl{};
	ctre::tuple<int, int, int> tpl1{1,2,3};
	ctre::tuple<int, int, int> tpl2{tpl1};
	
	print(tpl1, tpl2);
	
	ctre::tuple<int, int, int> tpl3{std::move(tpl2)};
	
	print(tpl2, tpl3);
	
	tpl3 = tpl;
	
	print(tpl3, tpl);
	
	tpl3 = std::move(tpl1);
	
	print(tpl3, tpl1);
}