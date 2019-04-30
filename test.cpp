#include "include/ctre.hpp"
#include "include/ctre/first.hpp"
#include <iostream>

template <typename T> struct identify;

int main() {
	ctre::point_set<4> set;
	set.insert(1,10);
	set.insert(5,20);
	
	std::cout << set.check(2,3) << "\n";
	
	constexpr auto lhs = ctre::calculate_first(ctre::plus<ctre::char_range<'a','z'>>{});
	constexpr auto rhs = ctre::calculate_first(ctre::back_reference<1>{});
	//identify<decltype(lhs)> i_lhs;
	//identify<decltype(rhs)> i_rhs;
	
	static_assert(ctre::collides(lhs, rhs));
	
	
	//auto out = ctre::first(ctll::list<>{}, ctll::list<ctre::select<ctre::string<'a','l','o','h','a'>, ctre::string<'h','a','n','a'>, ctre::character<'a'>> >{});
	//identify<decltype(out)> i;
}

//bool match2(std::string_view subject) {
//	return ctre::match<LR"(^([-dlsDLS][-rwxstlRWXSTL]{9}[+]?)\s+(\d+)?\s+(\S+)?\s+(.*?)?\s+(\d+)\s+(\d{1,2}).\s([^\s,.]{3,4}).?,?\s((\d{4,4})|(\d{1,2}):(\d{2,2}))\s)">(subject);
//}
//
//bool match(std::string_view subject){
//    return ctre::match<LR"(^(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\d+)\s+(\d+)\s*$)">(subject);
//}

static constexpr auto pattern = ctll::fixed_string(LR"(^([-dlsDLS][-rwxstlRWXSTL]{9}[+]?)\s*(\d+)?\s+(\S+)?\s+(.*?)?\s+(\d+|\d+.\d+\S)\s+([A-Za-z][a-z][a-z])\s+(\d{1,2})\s+((\d{4,4})|(\d{1,2}):.)\s)");

bool match3(std::string_view subject) {
	return ctre::search<pattern>(subject);
}