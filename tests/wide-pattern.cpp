#include <ctre.hpp>

static constexpr auto Pattern = ctll::basic_fixed_string{ LR"(^(\S+;\d+)\s+(\d+|\d+/\d+)?\s+((\d{1,2})-([A-Z]{3,3})-(\d{4,4})\s+(\d{1,2}):(\d{2,2})(:(\d{2,2})(\.\d{2,2})?)?(\s+\[(\S+),(\S+)\]\s+\(([A-Z]*),([A-Z]*),([A-Z]*),([A-Z]*)\))?)?)" };

int main()
{
	const wchar_t test[]{ L"abc" };
	const wchar_t* begin = test;
	const wchar_t* end = begin + 3;
	const auto results = ctre::re<Pattern>().search(begin, end);
}