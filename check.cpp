#include <cstdio>
#include <ctre-unicode.hpp>
#include <optional>
#include <cstdio>
#include <list>

#include <cstdio>

//constexpr void tokenize(std::string_view input) {
//	for (auto word: ctre::multiline_range<R"((...$))">(input)) {
//		printf("\033[31;1m> '%.*s'\033[0m\n", word.get<1>().size(), word.get<1>().data());
//	}
//}

template <typename T> struct identify;

bool match( std::string_view input )
{
	//identify<decltype(ctre::match<"(abc|abcd)*">)> i;
	return ctre::match<"(?:a|ab)+c">( input );
}

int main() {
	
	if( match("abc") )
	{
		puts("match");
	}
	else
	{
		puts("no match");
	}
}