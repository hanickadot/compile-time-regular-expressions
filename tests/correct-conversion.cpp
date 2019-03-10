#include <iostream>
#include "ctre.hpp"

static constexpr auto pattern = ctll::fixed_string{ ".*" };

int main(void)
{
	std::string str{ "a\ta\nc\td" };
	std::string str2{ ctre::re< pattern >().match( str ).get< 0 >() };
	std::string str3( ctre::re< pattern >().match( str ).get< 0 >() );
	std::cout<<str2.size()<<'\t'<<str2;
	std::cout<<str3.size()<<'\t'<<str3;
}  
